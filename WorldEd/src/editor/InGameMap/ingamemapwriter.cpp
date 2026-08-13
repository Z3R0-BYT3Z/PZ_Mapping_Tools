/*
 * Copyright 2018, Tim Baker <treectrl@users.sf.net>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ingamemapwriter.h"

#include "world.h"
#include "worldcell.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QXmlStreamWriter>
#include <QDebug>
#include <QtMath>

#include <limits>

namespace {
bool hasInGameMapProperty(InGameMapFeature *feature,
                          const QString &propertyName)
{
    for (const InGameMapProperty &property : feature->mProperties) {
        if (property.mKey == propertyName)
            return true;
    }
    return false;
}

double distanceToSegmentSquared(const InGameMapPoint &point,
                                const InGameMapPoint &start,
                                const InGameMapPoint &end)
{
    const double dx = end.x - start.x;
    const double dy = end.y - start.y;
    if (qFuzzyIsNull(dx) && qFuzzyIsNull(dy)) {
        const double px = point.x - start.x;
        const double py = point.y - start.y;
        return px * px + py * py;
    }
    const double lengthSquared = dx * dx + dy * dy;
    const double t = qBound(0.0,
            ((point.x - start.x) * dx +
             (point.y - start.y) * dy) / lengthSquared,
            1.0);
    const double px = point.x - (start.x + t * dx);
    const double py = point.y - (start.y + t * dy);
    return px * px + py * py;
}

void simplifySection(const QVector<InGameMapPoint> &points,
                     int first, int last,
                     double toleranceSquared,
                     QVector<bool> &keep)
{
    if (last <= first + 1)
        return;
    double farthestDistance = -1.0;
    int farthestIndex = -1;
    for (int index = first + 1; index < last; ++index) {
        const double distance = distanceToSegmentSquared(
                    points.at(index), points.at(first), points.at(last));
        if (distance > farthestDistance) {
            farthestDistance = distance;
            farthestIndex = index;
        }
    }
    if (farthestIndex < 0 || farthestDistance <= toleranceSquared)
        return;
    keep[farthestIndex] = true;
    simplifySection(points, first, farthestIndex,
                    toleranceSquared, keep);
    simplifySection(points, farthestIndex, last,
                    toleranceSquared, keep);
}

InGameMapCoordinates simplifyClosedCoordinates(
        const InGameMapCoordinates &source, double tolerance)
{
    if (source.size() <= 3 || tolerance <= 0.0)
        return source;
    int opposite = 1;
    double farthestDistance = -1.0;
    for (int index = 1; index < source.size(); ++index) {
        const double dx = source.at(index).x - source.first().x;
        const double dy = source.at(index).y - source.first().y;
        const double distance = dx * dx + dy * dy;
        if (distance > farthestDistance) {
            farthestDistance = distance;
            opposite = index;
        }
    }
    if (opposite <= 0 || opposite >= source.size())
        return source;
    QVector<InGameMapPoint> unwrapped;
    unwrapped.reserve(source.size() + 1);
    for (int index = 0; index < source.size(); ++index)
        unwrapped += source.at(index);
    unwrapped += source.first();
    QVector<bool> keep(unwrapped.size(), false);
    keep[0] = true;
    keep[opposite] = true;
    keep[unwrapped.size() - 1] = true;
    const double toleranceSquared = tolerance * tolerance;
    simplifySection(unwrapped, 0, opposite,
                    toleranceSquared, keep);
    simplifySection(unwrapped, opposite, unwrapped.size() - 1,
                    toleranceSquared, keep);
    InGameMapCoordinates result;
    for (int index = 0; index < unwrapped.size() - 1; ++index) {
        if (keep.at(index))
            result += unwrapped.at(index);
    }
    return result.size() >= 3 ? result : source;
}

int rendererIndexEstimate(const QList<InGameMapExportFeature> &features)
{
    qint64 estimate = 0;
    for (const InGameMapExportFeature &item : features) {
        if (!item.geometry.isPolygon())
            continue;
        int pointCount = 0;
        for (const InGameMapCoordinates &coordinates :
             item.geometry.mCoordinates)
            pointCount += coordinates.size();
        const int holeCount = qMax(0,
                item.geometry.mCoordinates.size() - 1);
        const qint64 baseTriangleIndices = qMax<qint64>(
                    3, qint64(3) *
                    (pointCount + holeCount * 2 - 2));
        estimate += baseTriangleIndices *
                (hasInGameMapProperty(item.feature,
                                      QStringLiteral("highway")) ? 7 : 1);
    }
    return int(qMin<qint64>(estimate,
                std::numeric_limits<int>::max()));
}

int exportPointCount(const QList<InGameMapExportFeature> &features)
{
    int count = 0;
    for (const InGameMapExportFeature &item : features) {
        for (const InGameMapCoordinates &coordinates :
             item.geometry.mCoordinates)
            count += coordinates.size();
    }
    return count;
}
}

class InGameMapWriterPrivate
{
    Q_DECLARE_TR_FUNCTIONS(InGameMapWriter)

public:
    InGameMapWriterPrivate()
        : mWorld(nullptr)
        , mFeatureScope(InGameMapFeatureScope::AllFeatures)
    {
    }

    bool openFile(QFile *file)
    {
        if (!file->open(QIODevice::WriteOnly)) {
            mError = tr("Could not open file for writing.");
            return false;
        }

        return true;
    }

    void writeWorld(World *world, QIODevice *device, const QString &absDirPath)
    {
        mError.clear();
        mMapDir = QDir(absDirPath);
        mWorld = world;
        mWrittenFeatures = 0;
        mRepairedFeatures = 0;
        mRejectedFeatures = 0;

        QXmlStreamWriter writer(device);
        writer.setAutoFormatting(true);
        writer.setAutoFormattingIndent(1);

        writer.writeStartDocument();

        writeWorld(writer, world);

        writer.writeEndDocument();

        qInfo() << "InGameMap XML geometry validation:"
                << "output" << mOutputPath
                << "written" << mWrittenFeatures
                << "repaired" << mRepairedFeatures
                << "rejected" << mRejectedFeatures;
    }

    void writeWorld(QXmlStreamWriter &w, World *world)
    {
        w.writeStartElement(QLatin1String("world"));

        w.writeAttribute(QLatin1String("version"), QLatin1String("1.0"));

        for (int y = 0; y < world->height(); y++) {
            for (int x = 0; x < world->width(); x++) {
                WorldCell *cell = world->cellAt(x, y);
                writeCell(w, cell);
                if (!mError.isEmpty()) {
                    w.writeEndElement();
                    return;
                }
            }
        }

        w.writeEndElement();
    }

    void writeCell(QXmlStreamWriter &w, WorldCell *cell)
    {
        QList<InGameMapExportFeature> exportFeatures;
        for (InGameMapFeature *feature : std::as_const(cell->inGameMap().mFeatures)) {
            if (!inGameMapFeatureMatchesScope(feature, mFeatureScope))
                continue;
            InGameMapExportFeature item{feature, InGameMapGeometry()};
            QStringList diagnostics;
            if (!sanitizeInGameMapGeometryForExport(feature->mGeometry,
                                                    item.geometry,
                                                    diagnostics)) {
                ++mRejectedFeatures;
                qWarning().noquote()
                        << QStringLiteral("InGameMap XML rejected feature: output=\"%1\" cell=%2,%3 feature=%4 properties={%5} reason=\"%6\"")
                           .arg(mOutputPath)
                           .arg(cell->x()).arg(cell->y())
                           .arg(feature->index())
                           .arg(propertySummary(feature))
                           .arg(diagnostics.join(QStringLiteral("; ")));
                continue;
            }
            if (!diagnostics.isEmpty()) {
                ++mRepairedFeatures;
                qWarning().noquote()
                        << QStringLiteral("InGameMap XML repaired feature: output=\"%1\" cell=%2,%3 feature=%4 properties={%5} action=\"%6\"")
                           .arg(mOutputPath)
                           .arg(cell->x()).arg(cell->y())
                           .arg(feature->index())
                           .arg(propertySummary(feature))
                           .arg(diagnostics.join(QStringLiteral("; ")));
            }
            exportFeatures += item;
        }

        if (!fitInGameMapRendererBudget(
                    exportFeatures, cell, mOutputPath,
                    QStringLiteral("XML"), &mRepairedFeatures, &mError))
            return;

        if (exportFeatures.isEmpty())
            return;

        const QPoint worldOrigin = cell->world()->getGenerateLotsSettings().worldOrigin;

        w.writeStartElement(QLatin1String("cell"));
        w.writeAttribute(QLatin1String("x"), QString::number(worldOrigin.x() + cell->x()));
        w.writeAttribute(QLatin1String("y"), QString::number(worldOrigin.y() + cell->y()));

        for (const InGameMapExportFeature &item : std::as_const(exportFeatures)) {
            writeFeature(w, item.feature, item.geometry);
            ++mWrittenFeatures;
        }

        w.writeEndElement();
    }

    QString propertySummary(InGameMapFeature *feature) const
    {
        QStringList properties;
        for (const InGameMapProperty &property : feature->mProperties)
            properties += property.mKey + QLatin1Char('=') + property.mValue;
        return properties.join(QLatin1Char(','));
    }

    void writeFeature(QXmlStreamWriter &w, InGameMapFeature* feature,
                      const InGameMapGeometry &geometry)
    {
        w.writeStartElement(QLatin1String("feature"));

        w.writeStartElement(QLatin1String("geometry"));
        w.writeAttribute(QLatin1String("type"), geometry.mType);
        for (const InGameMapCoordinates &coords : geometry.mCoordinates) {
            w.writeStartElement(QLatin1String("coordinates"));
            for (auto& point : coords) {
                w.writeStartElement(QLatin1String("point"));
                w.writeAttribute(QLatin1String("x"), QString::number(point.x));
                w.writeAttribute(QLatin1String("y"), QString::number(point.y));
                w.writeEndElement();
            }
            w.writeEndElement();
        }
        w.writeEndElement();

        w.writeStartElement(QLatin1String("properties"));
        for (auto& property : feature->mProperties) {
            w.writeStartElement(QLatin1String("property"));
            w.writeAttribute(QLatin1String("name"), property.mKey);
            w.writeAttribute(QLatin1String("value"), property.mValue);
            w.writeEndElement();
        }
        w.writeEndElement();

        w.writeEndElement();
    }

    World *mWorld;
    QString mError;
    QDir mMapDir;
    QString mOutputPath = QStringLiteral("<device>");
    int mWrittenFeatures = 0;
    int mRepairedFeatures = 0;
    int mRejectedFeatures = 0;
    InGameMapFeatureScope mFeatureScope;
};

/////

bool inGameMapFeatureMatchesScope(
        const InGameMapFeature *feature,
        InGameMapFeatureScope scope)
{
    if (scope == InGameMapFeatureScope::AllFeatures)
        return true;
    const bool forest = feature && feature->mProperties.contains(
                QStringLiteral("natural"), QStringLiteral("forest"));
    return scope == InGameMapFeatureScope::ForestFeatures
            ? forest
            : !forest;
}

bool fitInGameMapRendererBudget(
        QList<InGameMapExportFeature> &features,
        WorldCell *cell,
        const QString &outputPath,
        const QString &formatName,
        int *repairedFeatures,
        QString *error)
{
    static const int safeIndexBudget = 24000;
    static const int safePointBufferShorts = 28000;
    const int originalPoints = exportPointCount(features);
    const int originalEstimate = rendererIndexEstimate(features);
    if (originalPoints * 2 <= safePointBufferShorts &&
            originalEstimate <= safeIndexBudget)
        return true;
    int changedFeatures = 0;
    double usedTolerance = 0.0;
    const QVector<double> tolerances =
            {1.0, 2.0, 3.0, 4.0, 6.0, 8.0,
             12.0, 16.0, 24.0, 32.0, 48.0, 64.0};
    for (double tolerance : tolerances) {
        changedFeatures = 0;
        for (InGameMapExportFeature &item : features) {
            if (!item.geometry.isPolygon() ||
                    !hasInGameMapProperty(item.feature,
                                          QStringLiteral("highway")))
                continue;
            InGameMapGeometry simplified = item.geometry;
            for (InGameMapCoordinates &coordinates :
                 simplified.mCoordinates) {
                coordinates = simplifyClosedCoordinates(
                            coordinates, tolerance);
            }
            InGameMapGeometry sanitized;
            QStringList diagnostics;
            if (!sanitizeInGameMapGeometryForExport(
                        simplified, sanitized, diagnostics))
                continue;
            if (rendererIndexEstimate(
                        {InGameMapExportFeature{
                             item.feature, sanitized}}) <
                    rendererIndexEstimate({item})) {
                item.geometry = sanitized;
                ++changedFeatures;
            }
        }
        usedTolerance = tolerance;
        if (exportPointCount(features) * 2 <= safePointBufferShorts &&
                rendererIndexEstimate(features) <= safeIndexBudget)
            break;
    }
    const int finalPoints = exportPointCount(features);
    const int finalEstimate = rendererIndexEstimate(features);
    const QPoint worldCoordinates =
            cell->world()->getGenerateLotsSettings().worldOrigin +
            QPoint(cell->x(), cell->y());
    if (changedFeatures > 0) {
        if (repairedFeatures)
            *repairedFeatures += changedFeatures;
        qWarning().noquote()
                << QStringLiteral("InGameMap %1 simplified renderer-heavy highways: output=\"%2\" cell=%3,%4 tolerance=%5 points=%6->%7 estimated-indices=%8->%9 changed-features=%10")
                   .arg(formatName, outputPath)
                   .arg(worldCoordinates.x()).arg(worldCoordinates.y())
                   .arg(usedTolerance)
                   .arg(originalPoints).arg(finalPoints)
                   .arg(originalEstimate).arg(finalEstimate)
                   .arg(changedFeatures);
    }
    if (finalPoints * 2 > safePointBufferShorts ||
            finalEstimate > safeIndexBudget) {
        if (error) {
            *error = QCoreApplication::translate(
                        "InGameMapWriter",
                        "InGameMap cell %1,%2 is too complex for the "
                        "game's 16-bit world-map renderer after safe "
                        "simplification (%3 points, estimated %4 indices).")
                    .arg(worldCoordinates.x()).arg(worldCoordinates.y())
                    .arg(finalPoints).arg(finalEstimate);
            qCritical().noquote() << *error;
        }
        return false;
    }
    return true;
}

InGameMapWriter::InGameMapWriter()
    : d(new InGameMapWriterPrivate)
{
}

InGameMapWriter::~InGameMapWriter()
{
    delete d;
}

bool InGameMapWriter::writeWorld(World *world, const QString &filePath)
{
    d->mOutputPath = QFileInfo(filePath).absoluteFilePath();
    QTemporaryFile tempFile;
    if (!d->openFile(&tempFile))
        return false;

    d->writeWorld(world, &tempFile, QFileInfo(filePath).absolutePath());

    if (!d->mError.isEmpty())
        return false;

    if (tempFile.error() != QFile::NoError) {
        d->mError = tempFile.errorString();
        return false;
    }

    // foo.pzw -> foo.pzw.bak
    QFileInfo destInfo(filePath);
    QString backupPath = filePath + QLatin1String(".bak");
    QFile backupFile(backupPath);
    if (destInfo.exists()) {
        if (backupFile.exists()) {
            if (!backupFile.remove()) {
                d->mError = QString(QLatin1String("Error deleting file!\n%1\n\n%2"))
                        .arg(backupPath)
                        .arg(backupFile.errorString());
                return false;
            }
        }
        QFile destFile(filePath);
        if (!destFile.rename(backupPath)) {
            d->mError = QString(QLatin1String("Error renaming file!\nFrom: %1\nTo: %2\n\n%3"))
                    .arg(filePath)
                    .arg(backupPath)
                    .arg(destFile.errorString());
            return false;
        }
    }

    // /tmp/tempXYZ -> foo.pzw
    tempFile.close();
    if (tempFile.rename(filePath)) {
        // If anything above failed, the temp file should auto-remove, but not after
        // a successful save.
        tempFile.setAutoRemove(false);
    } else {
        QFile destination(filePath);
        if (destination.exists() && !destination.remove()) {
            d->mError = QString(QLatin1String("Error replacing file!\n%1\n\n%2"))
                    .arg(filePath).arg(destination.errorString());
            if (backupFile.exists())
                backupFile.rename(filePath);
            return false;
        }
        if (!tempFile.copy(filePath)) {
            d->mError = QString(QLatin1String("Error copying file!\nFrom: %1\nTo: %2\n\n%3"))
                    .arg(tempFile.fileName()).arg(filePath)
                    .arg(tempFile.errorString());
            if (backupFile.exists())
                backupFile.rename(filePath);
            return false;
        }
    }

    return true;
}

void InGameMapWriter::writeWorld(World *world, QIODevice *device, const QString &absDirPath)
{
    d->mOutputPath = QStringLiteral("<device>");
    d->writeWorld(world, device, absDirPath);
}

void InGameMapWriter::setFeatureScope(InGameMapFeatureScope scope)
{
    d->mFeatureScope = scope;
}

QString InGameMapWriter::errorString() const
{
    return d->mError;
}
