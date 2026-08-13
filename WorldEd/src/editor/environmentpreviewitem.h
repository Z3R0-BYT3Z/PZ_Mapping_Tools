#ifndef ENVIRONMENTPREVIEWITEM_H
#define ENVIRONMENTPREVIEWITEM_H

#include <QGraphicsItem>
#include <QImage>
#include <QTransform>
#include <QVector>

struct EnvironmentPreviewSprite
{
    QImage image;
    QTransform transform;
};

class EnvironmentPreviewItem : public QGraphicsItem
{
public:
    explicit EnvironmentPreviewItem(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    void setBounds(const QRectF &bounds);
    void setSprites(const QVector<EnvironmentPreviewSprite> &sprites);

private:
    QRectF mBounds;
    QVector<EnvironmentPreviewSprite> mSprites;
};

#endif
