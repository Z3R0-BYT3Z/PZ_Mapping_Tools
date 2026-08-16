/*
 * Copyright 2013, Tim Baker <treectrl@users.sf.net>
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

#include "preferences.h"
#include "../portablesettings.h"

#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QPalette>
#include <QSaveFile>
#include <QSettings>
#include <QTextStream>

namespace {

int basementResourceFileCount(const QString &root,
                              const QStringList &patterns)
{
    if (!QDir(root).exists())
        return 0;
    int count = 0;
    QDirIterator iterator(root, patterns, QDir::Files,
                          QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        ++count;
    }
    return count;
}

struct BuiltInTheme
{
    const char *displayName;
    const char *fileName;
    const char *resourcePath;
    const char *baseResourcePath;
};
const BuiltInTheme builtInThemes[] = {
    { "Breeze (Dark)", "Breeze-Dark.qss", ":breeze/dark/stylesheet.qss", 0 },
    { "Breeze (Dark Blue)", "Breeze-Dark-Blue.qss", ":breeze/dark-blue/stylesheet.qss", 0 },
    { "QDarkStyle (Dark)", "QDarkStyle-Dark.qss", ":qdarkstyle/dark/darkstyle.qss", 0 },
    { "QDarkStyle (Light)", "QDarkStyle-Light.qss", ":qdarkstyle/light/lightstyle.qss", 0 },
    { "Mapping Discord (B42)", "Mapping-Discord-B42.qss",
      ":breeze/mapping-discord/stylesheet.qss", ":breeze/dark/stylesheet.qss" }
};
QString themesDirectoryPath()
{
    return QDir(PortableSettings::installRootPath()).filePath(QStringLiteral("themes"));
}

QString builtInThemePath(const QString &displayName)
{
    for (const BuiltInTheme &theme : builtInThemes) {
        if (displayName == QLatin1String(theme.displayName))
            return QDir(themesDirectoryPath()).filePath(QLatin1String(theme.fileName));
    }
    return QString();
}

QString builtInThemeResource(const QString &displayName)
{
    for (const BuiltInTheme &theme : builtInThemes) {
        if (displayName == QLatin1String(theme.displayName))
            return QLatin1String(theme.resourcePath);
    }
    return QString();
}

bool isBuiltInThemeFile(const QString &fileName)
{
    for (const BuiltInTheme &theme : builtInThemes) {
        if (fileName.compare(QLatin1String(theme.fileName), Qt::CaseInsensitive) == 0)
            return true;
    }
    return false;
}

QString readStyleSheet(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    return stream.readAll();
}
QString mappingDiscordPalette(QString styleSheet)
{
    struct ColorReplacement
    {
        const char *source;
        const char *destination;
    };
    const ColorReplacement replacements[] = {
        { "#eff0f1", "#eef2ef" },
        { "#31363b", "#141c17" },
        { "#2c3034", "#101712" },
        { "#1d2023", "#0a0f0c" },
        { "#76797c", "#4a5c50" },
        { "#626568", "#2d3c32" },
        { "#3daee9", "#65c981" },
        { "#2a79a3", "#4e8f61" },
        { "#2f88b7", "#59ad72" },
        { "#334e5e", "#24452f" },
        { "#454545", "#46544a" },
        { "#454a4f", "#243229" },
        { "#b0b0b0", "#9da9a1" },
        { "#ffffff", "#eef2ef" }
    };
    for (const ColorReplacement &replacement : replacements) {
        styleSheet.replace(QLatin1String(replacement.source),
                           QLatin1String(replacement.destination),
                           Qt::CaseInsensitive);
    }
    return styleSheet;
}
QString builtInThemeStyleSheet(const QString &displayName)
{
    for (const BuiltInTheme &theme : builtInThemes) {
        if (displayName != QLatin1String(theme.displayName))
            continue;
        QString styleSheet;
        if (theme.baseResourcePath) {
            styleSheet = readStyleSheet(QLatin1String(theme.baseResourcePath));
            if (displayName == QLatin1String("Mapping Discord (B42)"))
                styleSheet = mappingDiscordPalette(styleSheet);
            styleSheet += QLatin1Char('\n');
        }
        styleSheet += readStyleSheet(QLatin1String(theme.resourcePath));
        return styleSheet;
    }
    return QString();
}
QPalette defaultApplicationPalette()
{
    static const QPalette palette = qApp->palette();
    return palette;
}
QPalette applicationPaletteForTheme(const QString &themeName)
{
    QPalette palette = defaultApplicationPalette();
    QColor window;
    QColor base;
    QColor alternate;
    QColor text;
    QColor muted;
    QColor highlight;
    QColor highlightedText;
    if (themeName == QLatin1String("Breeze (Dark)")
            || themeName == QLatin1String("Breeze (Dark Blue)")) {
        window = QColor(QStringLiteral("#31363b"));
        base = QColor(QStringLiteral("#1d2023"));
        alternate = QColor(QStringLiteral("#2c3034"));
        text = QColor(QStringLiteral("#eff0f1"));
        muted = QColor(QStringLiteral("#76797c"));
        highlight = QColor(QStringLiteral("#3daee9"));
        highlightedText = text;
    } else if (themeName == QLatin1String("QDarkStyle (Dark)")) {
        window = QColor(QStringLiteral("#19232d"));
        base = QColor(QStringLiteral("#37414f"));
        alternate = QColor(QStringLiteral("#455364"));
        text = QColor(QStringLiteral("#dfe1e2"));
        muted = QColor(QStringLiteral("#788d9c"));
        highlight = QColor(QStringLiteral("#1a72bb"));
        highlightedText = text;
    } else if (themeName == QLatin1String("Mapping Discord (B42)")) {
        window = QColor(QStringLiteral("#141c17"));
        base = QColor(QStringLiteral("#101712"));
        alternate = QColor(QStringLiteral("#19231c"));
        text = QColor(QStringLiteral("#eef2ef"));
        muted = QColor(QStringLiteral("#9da9a1"));
        highlight = QColor(QStringLiteral("#4e8f61"));
        highlightedText = text;
    } else {
        return palette;
    }
    const QPalette::ColorGroup groups[] = {
        QPalette::Active, QPalette::Inactive, QPalette::Disabled
    };
    for (QPalette::ColorGroup group : groups) {
        const QColor groupText = group == QPalette::Disabled ? muted : text;
        palette.setColor(group, QPalette::Window, window);
        palette.setColor(group, QPalette::WindowText, groupText);
        palette.setColor(group, QPalette::Base, base);
        palette.setColor(group, QPalette::AlternateBase, alternate);
        palette.setColor(group, QPalette::ToolTipBase, base);
        palette.setColor(group, QPalette::ToolTipText, groupText);
        palette.setColor(group, QPalette::Text, groupText);
        palette.setColor(group, QPalette::Button, window);
        palette.setColor(group, QPalette::ButtonText, groupText);
        palette.setColor(group, QPalette::BrightText, QColor(QStringLiteral("#ffffff")));
        palette.setColor(group, QPalette::Link, highlight);
        palette.setColor(group, QPalette::Highlight, highlight);
        palette.setColor(group, QPalette::HighlightedText, highlightedText);
    }
    return palette;
}
void ensureBuiltInThemesExtracted()
{
    const QString directoryPath = themesDirectoryPath();
    if (!QDir().mkpath(directoryPath))
        return;
    const QDir directory(directoryPath);
    for (const BuiltInTheme &theme : builtInThemes) {
        const QString destinationPath = directory.filePath(QLatin1String(theme.fileName));
        const QString styleSheet = builtInThemeStyleSheet(QLatin1String(theme.displayName));
        if (styleSheet.isEmpty())
            continue;
        QFile existing(destinationPath);
        if (existing.open(QIODevice::ReadOnly)
                && existing.readAll() == styleSheet.toUtf8()) {
            continue;
        }
        existing.close();
        QSaveFile destination(destinationPath);
        if (!destination.open(QIODevice::WriteOnly))
            continue;
        destination.write(styleSheet.toUtf8());
        destination.commit();
    }
}
}
Preferences *Preferences::mInstance = 0;
Preferences *Preferences::instance()
{
    if (!mInstance)
        mInstance = new Preferences;
    return mInstance;
}
void Preferences::deleteInstance()
{
    delete mInstance;
    mInstance = 0;
}

bool Preferences::snapToGrid() const
{
    return mSnapToGrid;
}

bool Preferences::showCellBorder() const
{
    return mShowCellBorder;
}

bool Preferences::showCoordinates() const
{
    return mShowCoordinates;
}

bool Preferences::showWorldGrid() const
{
    return mShowWorldGrid;
}

bool Preferences::showCellGrid() const
{
    return mShowCellGrid;
}

bool Preferences::showMiniMap() const
{
    return mShowMiniMap;
}

int Preferences::miniMapWidth() const
{
    return mMiniMapWidth;
}

bool Preferences::highlightCurrentLevel() const
{
    return mHighlightCurrentLevel;
}

Preferences::Preferences()
    : QObject()
    , mSettings(new QSettings)
{
    // Retrieve interface settings
    mSettings->beginGroup(QLatin1String("Interface"));
    mSnapToGrid = mSettings->value(QLatin1String("SnapToGrid"), true).toBool();
    mShowCellBorder = mSettings->value(QLatin1String("ShowCellBorder"), true).toBool();
    mShowCoordinates = mSettings->value(QLatin1String("ShowCoordinates"), true).toBool();
    mShowWorldGrid = mSettings->value(QLatin1String("ShowWorldGrid"), true).toBool();
    mShowCellGrid = mSettings->value(QLatin1String("ShowCellGrid"), false).toBool();
    mGridColor = QColor(mSettings->value(QLatin1String("GridColor"),
                                         QColor(Qt::black).name()).toString());
    mGridWidth = qBound(1, mSettings->value(QLatin1String("GridWidth"), 1).toInt(), 10);
    mThumbnailWidth = qBound(32, mSettings->value(QLatin1String("ThumbnailWidth"), 512).toInt(), 8192);
    mTerrainImageMemoryLimitMiB = qBound(
                128,
                mSettings->value(
                    QLatin1String("TerrainImageMemoryLimitMiB"), 512).toInt(),
                65536);
    mRestoreLastSession = mSettings->value(QLatin1String("RestoreLastSession"), false).toBool();
    mAutoSaveIntervalMinutes = mSettings->value(
                QLatin1String("AutoSaveIntervalMinutes"), 0).toInt();
    if (!QList<int>({0, 1, 5, 10, 20, 60}).contains(
                mAutoSaveIntervalMinutes))
        mAutoSaveIntervalMinutes = 0;
    mRoadSimplificationHighway = qBound(0.0, mSettings->value(
            QLatin1String("RoadSimplificationHighway"), 2.0).toDouble(), 32.0);
    mRoadPointSpacingHighway = qBound(1, mSettings->value(
            QLatin1String("RoadPointSpacingHighway"), 40).toInt(), 300);
    mRoadSimplificationTrail = qBound(0.0, mSettings->value(
            QLatin1String("RoadSimplificationTrail"), 2.0).toDouble(), 32.0);
    mRoadPointSpacingTrail = qBound(1, mSettings->value(
            QLatin1String("RoadPointSpacingTrail"), 40).toInt(), 300);
    mRoadSimplificationRailway = qBound(0.0, mSettings->value(
            QLatin1String("RoadSimplificationRailway"), 2.0).toDouble(), 32.0);
    mRoadPointSpacingRailway = qBound(1, mSettings->value(
            QLatin1String("RoadPointSpacingRailway"), 40).toInt(), 300);
    mShowObjects = mSettings->value(QLatin1String("ShowObjects"), true).toBool();
    mShowObjectNames = mSettings->value(QLatin1String("ShowObjectNames"), true).toBool();
    mShowBMPs = mSettings->value(QLatin1String("ShowBMPs"), true).toBool();
    mShowMiniMap = mSettings->value(QLatin1String("ShowMiniMap"), true).toBool();
    mShowZombieSpawnImage = mSettings->value(QLatin1String("ShowZombieSpawnImage"), false).toBool();
    mZombieSpawnImageOpacity = mSettings->value(QLatin1String("ZombieSpawnImageOpacity"), 0.8).toReal();
    mShowBiomeMap = mSettings->value(QLatin1String("ShowBiomeMap"), false).toBool();
    mBiomeMapOpacity = mSettings->value(QLatin1String("BiomeMapOpacity"), 0.65).toReal();
    mShowZonesInWorldView = mSettings->value(QLatin1String("ShowZonesInWorldView"), false).toBool();
    mMiniMapWidth = mSettings->value(QLatin1String("MiniMapWidth"), 256).toInt();
    mHighlightCurrentLevel = mSettings->value(QLatin1String("HighlightCurrentLevel"),
                                              false).toBool();
    mHighlightRoomUnderPointer = mSettings->value(QLatin1String("HighlightRoomUnderPointer"),
                                                  false).toBool();
    mShowLotFloorsOnly = mSettings->value(QLatin1String("ShowLotFloorsOnly"), false).toBool();
    mShowOtherWorlds = mSettings->value(QLatin1String("ShowOtherWorlds"), true).toBool();
    mUseOpenGL = mSettings->value(QLatin1String("OpenGL"), false).toBool();
    mLoadAllWorldThumbnails = mSettings->value(QLatin1String("LoadAllWorldThumbnails"), false).toBool();
    mShowWorldThumbnails = mSettings->value(QLatin1String("ShowWorldThumbnails"), true).toBool();
    mShowAdjacentMaps = mSettings->value(QLatin1String("ShowAdjacentMaps"), true).toBool();
    mShowInvisibleTiles = mSettings->value(QLatin1String("ShowInvisibleTiles"), false).toBool();
    mTheme = mSettings->value(QLatin1String("Theme"), QLatin1String("Default")).toString();
    if (PortableSettings::syncThemeAcrossApplications())
        mTheme = PortableSettings::sharedTheme(mTheme);
    mSettings->endGroup();

    mSettings->beginGroup(QLatin1String("MapsDirectory"));
    mMapsDirectory = mSettings->value(QLatin1String("Current"), QString()).toString();
    mSettings->endGroup();

    mTilesDirectory = PortableSettings::sharedTilesPath();
    mProjectZomboidDirectory = PortableSettings::sharedGamePath();
    mSettings->remove(QLatin1String("TilesDirectory"));

    QSettings settings(QLatin1String("TheIndieStone"), QLatin1String("TileZed"));
    mTilePropertiesFiles = settings.value(QLatin1String("TilePropertiesFiles")).toStringList();
    if (!mProjectZomboidDirectory.isEmpty()) {
        const QStringList builtInTileDefs = {
            QStringLiteral("newtiledefinitions.tiles"),
            QStringLiteral("tiledefinitions_erosion.tiles"),
            QStringLiteral("tiledefinitions_overlays.tiles"),
            QStringLiteral("tiledefinitions_b42chunkcaching.tiles"),
            QStringLiteral("tiledefinitions_noiseworks.patch.tiles"),
            QStringLiteral("jumbo_trees_big.tiles"),
            QStringLiteral("jumbo_trees.tiles")
        };
        for (const QString &fileName : builtInTileDefs) {
            bool alreadyConfigured = false;
            for (const QString &configured : qAsConst(mTilePropertiesFiles)) {
                if (QFileInfo(configured).fileName().compare(
                            fileName, Qt::CaseInsensitive) == 0) {
                    alreadyConfigured = true;
                    break;
                }
            }
            if (alreadyConfigured)
                continue;
            const QFileInfo candidate(QDir(mProjectZomboidDirectory)
                                      .filePath(QStringLiteral("media/") +
                                                fileName));
            if (candidate.exists() && candidate.isFile())
                mTilePropertiesFiles.append(candidate.canonicalFilePath());
        }
    }

    mOpenFileDirectory = mSettings->value(QLatin1String("OpenFileDirectory")).toString();
    mWorldMapXMLFile = mSettings->value(QLatin1String("WorldMapXMLFile")).toString();

    mConfigDirectory = PortableSettings::sharedConfigurationPath();
    qInfo().noquote() << "Effective shared configuration directory"
                      << QDir::toNativeSeparators(mConfigDirectory);
    qInfo().noquote() << "Effective Tiles directory"
                      << QDir::toNativeSeparators(mTilesDirectory);
    qInfo().noquote() << "Effective Project Zomboid directory"
                      << (mProjectZomboidDirectory.isEmpty()
                          ? QLatin1String("<not configured>")
                          : QDir::toNativeSeparators(
                                mProjectZomboidDirectory));
    qInfo().noquote() << "Portable basement source directory"
                      << QDir::toNativeSeparators(
                             PortableSettings::basementSourcePath())
                      << (QDir(PortableSettings::basementSourcePath()).exists()
                          ? QLatin1String("[available]")
                          : QLatin1String("[missing]"))
                      << "TBX/TMX files:"
                      << basementResourceFileCount(
                             PortableSettings::basementSourcePath(),
                             QStringList() << QStringLiteral("*.tbx")
                                           << QStringLiteral("*.tmx"));
    qInfo().noquote() << "Portable basement PZBY directory"
                      << QDir::toNativeSeparators(
                             PortableSettings::basementBinMapPath())
                      << (QDir(PortableSettings::basementBinMapPath()).exists()
                          ? QLatin1String("[available]")
                          : QLatin1String("[missing]"))
                      << "PZBY/TBX/TMX files:"
                      << basementResourceFileCount(
                             PortableSettings::basementBinMapPath(),
                             QStringList() << QStringLiteral("*.pzby")
                                           << QStringLiteral("*.tbx")
                                           << QStringLiteral("*.tmx"));

    // Use the same directory as TileZed.
    mThumbnailsDirectory = settings.value(QLatin1String("Thumbnails/Directory")).toString();
}

Preferences::~Preferences()
{
    delete mSettings;
}

QString Preferences::userPath() const
{
    return PortableSettings::rootPath();
}

QString Preferences::userPath(const QString &fileName) const
{
    return userPath() + QLatin1Char('/') + fileName;
}

QString Preferences::configPath() const
{
    return mConfigDirectory;
}

QString Preferences::configPath(const QString &fileName) const
{
    return configPath() + QLatin1Char('/') + fileName;
}

QString Preferences::appConfigPath() const
{
#ifdef Q_OS_WIN
    return PortableSettings::applicationConfigPath();
#elif defined(Q_OS_MAC)
    return QCoreApplication::applicationDirPath() + QLatin1String("/../Config");
#elif defined(Q_OS_UNIX)
    return QCoreApplication::applicationDirPath() + QLatin1String("/../../TileZed/share/tilezed/config");
#else
#error "Unsupported platform: implement the WorldEd configuration path."
#endif
}

QString Preferences::appConfigPath(const QString &fileName) const
{
    const QString localFile = configPath(fileName);
    if (QFileInfo::exists(localFile))
        return localFile;
    return appConfigPath() + QLatin1Char('/') + fileName;
}

QString Preferences::docsPath() const
{
#ifdef Q_OS_WIN
    return PortableSettings::installPath(QLatin1String("docs"));
#elif defined(Q_OS_MAC)
    return QCoreApplication::applicationDirPath() + QLatin1String("/../Docs");
#elif defined(Q_OS_UNIX)
    return QCoreApplication::applicationDirPath() + QLatin1String("/../share/tilezed/docs");
#else
#error "Unsupported platform: implement the WorldEd documentation path."
#endif
}

QString Preferences::docsPath(const QString &fileName) const
{
    return docsPath() + QLatin1Char('/') + fileName;
}

QString Preferences::luaPath() const
{
#ifdef Q_OS_WIN
    return PortableSettings::installPath(QLatin1String("lua"));
#elif defined(Q_OS_MAC)
    return QCoreApplication::applicationDirPath() + QLatin1String("/../Lua");
#elif defined(Q_OS_UNIX)
    return QCoreApplication::applicationDirPath() + QLatin1String("/../share/tilezed/lua");
#else
#error "Unsupported platform: implement the WorldEd Lua path."
#endif
}

QString Preferences::luaPath(const QString &fileName) const
{
    return luaPath() + QLatin1Char('/') + fileName;
}

QString Preferences::mapsDirectory() const
{
    return mMapsDirectory;
}

void Preferences::setSnapToGrid(bool snapToGrid)
{
    if (snapToGrid == mSnapToGrid)
        return;

    mSnapToGrid = snapToGrid;
    mSettings->setValue(QLatin1String("Interface/SnapToGrid"), mSnapToGrid);
    emit snapToGridChanged(mSnapToGrid);
}

void Preferences::setShowCellBorder(bool showCellBorder)
{
    if (showCellBorder == mShowCellBorder)
        return;

    mShowCellBorder = showCellBorder;
    mSettings->setValue(QLatin1String("Interface/ShowCellBorder"), mShowCellBorder);
    emit showCellBorderChanged(mShowCellBorder);
}

void Preferences::setShowCoordinates(bool showCoords)
{
    if (showCoords == mShowCoordinates)
        return;

    mShowCoordinates = showCoords;
    mSettings->setValue(QLatin1String("Interface/ShowCoordinates"), mShowCoordinates);
    emit showCoordinatesChanged(mShowCoordinates);
}

void Preferences::setShowWorldGrid(bool showGrid)
{
    if (showGrid == mShowWorldGrid)
        return;

    mShowWorldGrid = showGrid;
    mSettings->setValue(QLatin1String("Interface/ShowWorldGrid"), mShowWorldGrid);
    emit showWorldGridChanged(mShowWorldGrid);
}

void Preferences::setShowCellGrid(bool showGrid)
{
    if (showGrid == mShowCellGrid)
        return;

    mShowCellGrid = showGrid;
    mSettings->setValue(QLatin1String("Interface/ShowCellGrid"), mShowCellGrid);
    emit showCellGridChanged(mShowCellGrid);
}

void Preferences::setGridColor(const QColor &gridColor)
{
    if (mGridColor == gridColor)
        return;

    mGridColor = gridColor;
    mSettings->setValue(QLatin1String("Interface/GridColor"), mGridColor.name());
    emit gridColorChanged(mGridColor);
}

void Preferences::setGridWidth(int width)
{
    width = qBound(1, width, 10);
    if (mGridWidth == width)
        return;
    mGridWidth = width;
    mSettings->setValue(QLatin1String("Interface/GridWidth"), mGridWidth);
    emit gridWidthChanged(mGridWidth);
}
void Preferences::setThumbnailWidth(int width)
{
    width = qBound(32, width, 8192);
    if (mThumbnailWidth == width)
        return;
    mThumbnailWidth = width;
    mSettings->setValue(QLatin1String("Interface/ThumbnailWidth"), mThumbnailWidth);
    emit thumbnailWidthChanged(mThumbnailWidth);
}
void Preferences::setTerrainImageMemoryLimitMiB(int limitMiB)
{
    limitMiB = qBound(128, limitMiB, 65536);
    if (mTerrainImageMemoryLimitMiB == limitMiB)
        return;
    mTerrainImageMemoryLimitMiB = limitMiB;
    mSettings->setValue(
                QLatin1String("Interface/TerrainImageMemoryLimitMiB"),
                mTerrainImageMemoryLimitMiB);
}
void Preferences::setRestoreLastSession(bool restore)
{
    if (mRestoreLastSession == restore)
        return;
    mRestoreLastSession = restore;
    mSettings->setValue(QLatin1String("Interface/RestoreLastSession"), mRestoreLastSession);
}
void Preferences::setAutoSaveIntervalMinutes(int minutes)
{
    const int normalized = QList<int>({0, 1, 5, 10, 20, 60}).contains(
                minutes) ? minutes : 0;
    if (mAutoSaveIntervalMinutes == normalized)
        return;
    mAutoSaveIntervalMinutes = normalized;
    mSettings->setValue(QLatin1String("Interface/AutoSaveIntervalMinutes"),
                        mAutoSaveIntervalMinutes);
    emit autoSaveIntervalChanged(mAutoSaveIntervalMinutes);
}
void Preferences::setRoadSimplificationHighway(qreal tolerance)
{
    mRoadSimplificationHighway = qBound(0.0, double(tolerance), 32.0);
    mSettings->setValue(QLatin1String("Interface/RoadSimplificationHighway"),
                        mRoadSimplificationHighway);
}
void Preferences::setRoadPointSpacingHighway(int spacing)
{
    mRoadPointSpacingHighway = qBound(1, spacing, 300);
    mSettings->setValue(QLatin1String("Interface/RoadPointSpacingHighway"),
                        mRoadPointSpacingHighway);
}
void Preferences::setRoadSimplificationTrail(qreal tolerance)
{
    mRoadSimplificationTrail = qBound(0.0, double(tolerance), 32.0);
    mSettings->setValue(QLatin1String("Interface/RoadSimplificationTrail"),
                        mRoadSimplificationTrail);
}
void Preferences::setRoadPointSpacingTrail(int spacing)
{
    mRoadPointSpacingTrail = qBound(1, spacing, 300);
    mSettings->setValue(QLatin1String("Interface/RoadPointSpacingTrail"),
                        mRoadPointSpacingTrail);
}
void Preferences::setRoadSimplificationRailway(qreal tolerance)
{
    mRoadSimplificationRailway = qBound(0.0, double(tolerance), 32.0);
    mSettings->setValue(QLatin1String("Interface/RoadSimplificationRailway"),
                        mRoadSimplificationRailway);
}
void Preferences::setRoadPointSpacingRailway(int spacing)
{
    mRoadPointSpacingRailway = qBound(1, spacing, 300);
    mSettings->setValue(QLatin1String("Interface/RoadPointSpacingRailway"),
                        mRoadPointSpacingRailway);
}
void Preferences::setUseOpenGL(bool useOpenGL)
{
    if (mUseOpenGL == useOpenGL)
        return;

    mUseOpenGL = useOpenGL;
    mSettings->setValue(QLatin1String("Interface/OpenGL"), mUseOpenGL);

    emit useOpenGLChanged(mUseOpenGL);
}

void Preferences::setLoadAllWorldThumbnails(bool thumbs)
{
    if (mLoadAllWorldThumbnails == thumbs)
        return;

    mLoadAllWorldThumbnails = thumbs;
    mSettings->setValue(QLatin1String("Interface/LoadAllWorldThumbnails"), mLoadAllWorldThumbnails);

    emit loadAllWorldThumbnailsChanged(mLoadAllWorldThumbnails);
}

void Preferences::setShowWorldThumbnails(bool thumbs)
{
    if (mShowWorldThumbnails == thumbs)
        return;

    mShowWorldThumbnails = thumbs;
    mSettings->setValue(QLatin1String("Interface/ShowWorldThumbnails"), mShowWorldThumbnails);

    emit showWorldThumbnailsChanged(mShowWorldThumbnails);
}

QString Preferences::openFileDirectory() const
{
    return mOpenFileDirectory;
}

void Preferences::setOpenFileDirectory(const QString &path)
{
    if (mOpenFileDirectory == path)
        return;
    mOpenFileDirectory = path;
    mSettings->setValue(QLatin1String("OpenFileDirectory"), mOpenFileDirectory);
}

QString Preferences::worldMapXMLFile() const
{
    return mWorldMapXMLFile;
}

void Preferences::setWorldMapXMLFile(const QString &path)
{
    if (mWorldMapXMLFile == path)
        return;
    mWorldMapXMLFile = path;
    mSettings->setValue(QLatin1String("WorldMapXMLFile"), mWorldMapXMLFile);
}

void Preferences::setShowAdjacentMaps(bool show)
{
    if (mShowAdjacentMaps == show)
        return;

    mShowAdjacentMaps = show;
    mSettings->setValue(QLatin1String("Interface/ShowAdjacentMaps"), mShowAdjacentMaps);

    emit showAdjacentMapsChanged(mShowAdjacentMaps);
}

void Preferences::setShowObjects(bool show)
{
    if (mShowObjects == show)
        return;

    mShowObjects = show;
    mSettings->setValue(QLatin1String("Interface/ShowObjects"), mShowObjects);

    emit showObjectsChanged(mShowObjects);
}

void Preferences::setShowObjectNames(bool show)
{
    if (mShowObjectNames == show)
        return;

    mShowObjectNames = show;
    mSettings->setValue(QLatin1String("Interface/ShowObjectNames"), mShowObjectNames);

    emit showObjectNamesChanged(mShowObjectNames);
}

void Preferences::setShowBMPs(bool show)
{
    if (mShowBMPs == show)
        return;

    mShowBMPs = show;
    mSettings->setValue(QLatin1String("Interface/ShowBMPs"), mShowBMPs);

    emit showBMPsChanged(mShowBMPs);
}

void Preferences::setShowZombieSpawnImage(bool show)
{
    if (mShowZombieSpawnImage == show)
        return;

    mShowZombieSpawnImage = show;
    mSettings->setValue(QLatin1String("Interface/ShowZombieSpawnImage"), mShowZombieSpawnImage);

    emit showZombieSpawnImageChanged(mShowZombieSpawnImage);
}

void Preferences::setZombieSpawnImageOpacity(qreal opacity)
{
    opacity = qMin(opacity, 1.0);
    opacity = qMax(opacity, 0.0);

    if (mZombieSpawnImageOpacity == opacity)
        return;

    mZombieSpawnImageOpacity = opacity;
    mSettings->setValue(QLatin1String("Interface/ZombieSpawnImageOpacity"), mZombieSpawnImageOpacity);

    emit zombieSpawnImageOpacityChanged(mZombieSpawnImageOpacity);
}

void Preferences::setShowBiomeMap(bool show)
{
    if (mShowBiomeMap == show)
        return;
    mShowBiomeMap = show;
    mSettings->setValue(QLatin1String("Interface/ShowBiomeMap"), mShowBiomeMap);
    emit showBiomeMapChanged(mShowBiomeMap);
}
void Preferences::setBiomeMapOpacity(qreal opacity)
{
    opacity = qBound(0.0, double(opacity), 1.0);
    if (mBiomeMapOpacity == opacity)
        return;
    mBiomeMapOpacity = opacity;
    mSettings->setValue(QLatin1String("Interface/BiomeMapOpacity"), mBiomeMapOpacity);
    emit biomeMapOpacityChanged(mBiomeMapOpacity);
}
void Preferences::setShowZonesInWorldView(bool show)
{
    if (mShowZonesInWorldView == show)
        return;

    mShowZonesInWorldView = show;
    mSettings->setValue(QLatin1String("Interface/ShowZonesInWorldView"), mShowZonesInWorldView);

    emit showZonesInWorldViewChanged(mShowZonesInWorldView);
}

void Preferences::setShowMiniMap(bool show)
{
    if (show == mShowMiniMap)
        return;

    mShowMiniMap = show;
    mSettings->setValue(QLatin1String("Interface/ShowMiniMap"), mShowMiniMap);
    emit showMiniMapChanged(mShowMiniMap);
}

void Preferences::setMiniMapWidth(int width)
{
    width = qMin(width, MINIMAP_WIDTH_MAX);
    width = qMax(width, MINIMAP_WIDTH_MIN);

    if (mMiniMapWidth == width)
        return;
    mMiniMapWidth = width;
    mSettings->setValue(QLatin1String("Interface/MiniMapWidth"), width);
    emit miniMapWidthChanged(mMiniMapWidth);
}

void Preferences::setHighlightCurrentLevel(bool highlight)
{
    if (highlight == mHighlightCurrentLevel)
        return;

    mHighlightCurrentLevel = highlight;
    mSettings->setValue(QLatin1String("Interface/HighlightCurrentLevel"), mHighlightCurrentLevel);
    emit highlightCurrentLevelChanged(mHighlightCurrentLevel);
}

void Preferences::setHighlightRoomUnderPointer(bool highlight)
{
    if (highlight == mHighlightRoomUnderPointer)
        return;
    mHighlightRoomUnderPointer = highlight;
    mSettings->setValue(QLatin1String("Interface/HighlightRoomUnderPointer"),
                        mHighlightRoomUnderPointer);
    emit highlightRoomUnderPointerChanged(mHighlightRoomUnderPointer);
}

void Preferences::setShowLotFloorsOnly(bool show)
{
    if (mShowLotFloorsOnly == show)
        return;
    mShowLotFloorsOnly = show;
    mSettings->setValue(QLatin1String("Interface/ShowLotFloorsOnly"), show);
    emit showLotFloorsOnlyChanged(mShowLotFloorsOnly);
}

void Preferences::setShowOtherWorlds(bool show)
{
    if (show == mShowOtherWorlds)
        return;
    mShowOtherWorlds = show;
    mSettings->setValue(QLatin1String("Interface/ShowOtherWorlds"),
                        mShowOtherWorlds);
    emit showOtherWorldsChanged(mShowOtherWorlds);
}

void Preferences::setMapsDirectory(const QString &path)
{
    if (mMapsDirectory == path)
        return;
    mMapsDirectory = path;
    mSettings->setValue(QLatin1String("MapsDirectory/Current"), path);

    // Put this up, otherwise the progress dialog shows and hides for each lot.
    // Since each open document has its own ZLotManager, this shows and hides for each document as well.
//    ZProgressManager::instance()->begin(QLatin1String("Checking lots..."));

    emit mapsDirectoryChanged();
}

QString Preferences::tilesDirectory() const
{
    return mTilesDirectory;
}

void Preferences::setTilesDirectory(const QString &path)
{
    const QString normalizedPath =
            PortableSettings::normalizedTilesPath(path);
    if (mTilesDirectory == normalizedPath)
        return;
    mTilesDirectory = normalizedPath;
    PortableSettings::setSharedTilesPath(normalizedPath);
    emit tilesDirectoryChanged();
}

QString Preferences::projectZomboidDirectory() const
{
    return mProjectZomboidDirectory;
}
void Preferences::setProjectZomboidDirectory(const QString &path)
{
    const QString normalized = PortableSettings::normalizedGamePath(path);
    if (!path.trimmed().isEmpty() && normalized.isEmpty()) {
        qWarning().noquote()
                << "Ignoring invalid Project Zomboid installation path"
                << QDir::toNativeSeparators(path);
        return;
    }
    if (mProjectZomboidDirectory == normalized)
        return;
    mProjectZomboidDirectory = normalized;
    PortableSettings::setSharedGamePath(normalized);
    emit projectZomboidDirectoryChanged();
}
QString Preferences::gameMediaPath(const QString &relativePath) const
{
    if (mProjectZomboidDirectory.isEmpty())
        return QString();
    const QString media = QDir(mProjectZomboidDirectory).filePath(
                QLatin1String("media"));
    return relativePath.isEmpty()
            ? media : QDir(media).filePath(relativePath);
}
QString Preferences::tiles2xDirectory() const
{
    if (mTilesDirectory.isEmpty())
        return QString();
    return mTilesDirectory + QLatin1Char('/') + QLatin1String("2x");
}

QString Preferences::texturesDirectory() const
{
    return QDir(mTilesDirectory).filePath(QLatin1String("Textures"));
}

void Preferences::setShowInvisibleTiles(bool show)
{
    if (mShowInvisibleTiles == show)
        return;

    mShowInvisibleTiles = show;
    mSettings->setValue(QLatin1String("Interface/ShowInvisibleTiles"), mShowInvisibleTiles);

    emit showInvisibleTilesChanged(mShowInvisibleTiles);
}

void Preferences::setTheme(const QString &theme)
{
    if (mTheme == theme) {
        if (PortableSettings::syncThemeAcrossApplications())
            PortableSettings::setThemeForAllApplications(theme);
        return;
    }
    mTheme = theme;
    applyTheme();
    if (PortableSettings::syncThemeAcrossApplications())
        PortableSettings::setThemeForAllApplications(theme);
}
QStringList Preferences::availableThemes() const
{
    ensureBuiltInThemesExtracted();
    QStringList themes;
    themes << QStringLiteral("Default")
           << QStringLiteral("Breeze (Dark)")
           << QStringLiteral("Breeze (Dark Blue)")
           << QStringLiteral("QDarkStyle (Dark)")
           << QStringLiteral("QDarkStyle (Light)")
           << QStringLiteral("Mapping Discord (B42)");
    QStringList externalThemes;
    const QString applicationDirectory = PortableSettings::installRootPath();
    const QStringList themeDirectories = {
        QDir(applicationDirectory).filePath(QStringLiteral("themes")),
        QDir(applicationDirectory).filePath(QStringLiteral("theme"))
    };
    for (const QString &directoryPath : themeDirectories) {
        const QDir directory(directoryPath);
        const QStringList files = directory.entryList(QDir::Files | QDir::Readable,
                                                       QDir::Name | QDir::IgnoreCase);
        for (const QString &fileName : files) {
            if (QFileInfo(fileName).suffix().compare(QStringLiteral("qss"), Qt::CaseInsensitive) != 0)
                continue;
            if (isBuiltInThemeFile(fileName))
                continue;
            if (!themes.contains(fileName, Qt::CaseInsensitive) &&
                    !externalThemes.contains(fileName, Qt::CaseInsensitive)) {
                externalThemes.append(fileName);
            }
        }
    }
    externalThemes.sort(Qt::CaseInsensitive);
    themes.append(externalThemes);
    return themes;
}

void Preferences::applyTheme() const
{
    ensureBuiltInThemesExtracted();
    mSettings->setValue(QLatin1String("Interface/Theme"), mTheme);
    qApp->setStyleSheet(QString());
    qApp->setPalette(applicationPaletteForTheme(mTheme));
    if (mTheme == QStringLiteral("Default")) {
        return;
    }
    QString styleSheet;
    bool styleSheetLoaded = false;
    QString resource = builtInThemeResource(mTheme);
    if (!resource.isEmpty()) {
        const QString localPath = builtInThemePath(mTheme);
        if (QFileInfo::exists(localPath)) {
            resource = localPath;
        } else {
            styleSheet = builtInThemeStyleSheet(mTheme);
            styleSheetLoaded = !styleSheet.isEmpty();
            resource.clear();
        }
    } else {
        const QString applicationDirectory = PortableSettings::installRootPath();
        const QStringList themeDirectories = {
            QDir(applicationDirectory).filePath(QStringLiteral("themes")),
            QDir(applicationDirectory).filePath(QStringLiteral("theme"))
        };
        for (const QString &directoryPath : themeDirectories) {
            const QDir directory(directoryPath);
            const QStringList files = directory.entryList(QDir::Files | QDir::Readable);
            for (const QString &fileName : files) {
                if (fileName.compare(mTheme, Qt::CaseInsensitive) == 0 &&
                        QFileInfo(fileName).suffix().compare(QStringLiteral("qss"), Qt::CaseInsensitive) == 0) {
                    resource = directory.filePath(fileName);
                    break;
                }
            }
            if (!resource.isEmpty())
                break;
        }
    }
    if (resource.isEmpty() && !styleSheetLoaded) {
        return;
    }
    if (!resource.isEmpty()) {
        styleSheet = readStyleSheet(resource);
        styleSheetLoaded = !styleSheet.isEmpty();
    }
    qApp->setStyleSheet(styleSheetLoaded ? styleSheet : QString());
}
