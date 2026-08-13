#include "environmentpreviewitem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

EnvironmentPreviewItem::EnvironmentPreviewItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    setAcceptedMouseButtons(Qt::NoButton);
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);
}

QRectF EnvironmentPreviewItem::boundingRect() const
{
    return mBounds;
}

void EnvironmentPreviewItem::setBounds(const QRectF &bounds)
{
    if (bounds == mBounds)
        return;
    prepareGeometryChange();
    mBounds = bounds;
}

void EnvironmentPreviewItem::setSprites(
        const QVector<EnvironmentPreviewSprite> &sprites)
{
    mSprites = sprites;
    update();
}

void EnvironmentPreviewItem::paint(
        QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option)
    const QTransform baseTransform = painter->transform();
    for (const EnvironmentPreviewSprite &sprite : mSprites) {
        if (sprite.image.isNull())
            continue;
        painter->setTransform(sprite.transform * baseTransform);
        painter->drawImage(0, 0, sprite.image);
    }
    painter->setTransform(baseTransform);
}
