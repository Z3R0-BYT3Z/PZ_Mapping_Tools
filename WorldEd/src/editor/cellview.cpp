/*
 * Copyright 2012, Tim Baker <treectrl@users.sf.net>
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

#include "cellview.h"

#include "celldocument.h"
#include "cellscene.h"
#include "preferences.h"
#include "world.h"
#include "zoomable.h"

#include "maprenderer.h"

#include <QMouseEvent>
#include <QPainter>

CellView::CellView(QWidget *parent) :
    BaseGraphicsView(PreferenceGL, parent)
{
    zoomable()->setScale(0.25);
}

CellScene *CellView::scene() const
{
    return static_cast<CellScene*>(mScene);
}

void CellView::mouseMoveEvent(QMouseEvent *event)
{
    int level = scene()->document()->currentLevel();
    QPoint tilePos = scene()->renderer()->pixelToTileCoordsInt(mapToScene(event->pos()), level);
    emit statusBarCoordinatesChanged(tilePos.x(), tilePos.y());

    BaseGraphicsView::mouseMoveEvent(event);
}

void CellView::paintEvent(QPaintEvent *event)
{
    if (scene())
        scene()->handlePendingUpdates();
    BaseGraphicsView::paintEvent(event);
}

void CellView::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawForeground(painter, rect);
    if (scene() && scene()->partialChunksEnabled()) {
        painter->save();
        QColor chunkColor = Preferences::instance()->gridColor();
        chunkColor.setAlpha(210);
        QPen pen(chunkColor);
        pen.setCosmetic(true);
        painter->setPen(pen);
        for (int y = 0; y < PZTools::PartialChunkSelection::ChunksPerCell; ++y) {
            for (int x = 0; x < PZTools::PartialChunkSelection::ChunksPerCell; ++x) {
                const QRect chunkRect(
                            x * PZTools::PartialChunkSelection::ChunkSize,
                            y * PZTools::PartialChunkSelection::ChunkSize,
                            PZTools::PartialChunkSelection::ChunkSize,
                            PZTools::PartialChunkSelection::ChunkSize);
                QColor selectedColor = chunkColor;
                selectedColor.setAlpha(22);
                painter->setBrush(scene()->partialChunkPreviewSelected(x, y)
                                  ? selectedColor
                                  : QColor(10, 10, 10, 165));
                painter->drawPolygon(scene()->renderer()->tileToPixelCoords(
                                         chunkRect,
                                         scene()->document()->currentLevel()));
            }
        }
        painter->restore();
    }
    if (scene() && scene()->document() && scene()->document()->world())
        drawProjectGridBadge(painter, scene()->document()->world()->cellSize());
}

QRectF CellView::sceneRectForMiniMap() const
{
    if (!scene() || !scene()->renderer() || !scene()->map())
        return QRectF();
    return scene()->renderer()->boundingRect(
                QRect(QPoint(), scene()->map()->size()));
}
