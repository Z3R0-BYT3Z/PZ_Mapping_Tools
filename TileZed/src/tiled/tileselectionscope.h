#ifndef TILESELECTIONSCOPE_H
#define TILESELECTIONSCOPE_H
#include <QObject>
#include <QStringList>
#include <functional>
class QMenu;
class QToolButton;
class QWidget;
namespace Tiled {
namespace Internal {
class TileSelectionScope : public QObject
{
    Q_OBJECT
public:
    enum LayerMode {
        CurrentLayer,
        VisibleLayers,
        AllLayers,
        SpecificLayers
    };
    enum LevelMode {
        CurrentLevel,
        AllLevels
    };
    explicit TileSelectionScope(const QString &levelLabel,
                                QObject *parent = nullptr);
    LayerMode layerMode() const { return mLayerMode; }
    LevelMode levelMode() const { return mLevelMode; }
    const QStringList &specificLayers() const { return mSpecificLayers; }
    bool includesLayer(const QString &layerName,
                       bool visible,
                       bool current) const;
    QToolButton *createToolButton(
            QWidget *parent,
            const std::function<QStringList()> &layerNamesProvider);
signals:
    void scopeChanged();
private:
    void rebuildSpecificLayersMenu(
            QMenu *menu,
            const std::function<QStringList()> &layerNamesProvider);
    void updateButtons();
    QString summaryText() const;
    QString mLevelLabel;
    LayerMode mLayerMode;
    LevelMode mLevelMode;
    QStringList mSpecificLayers;
    QList<QToolButton*> mButtons;
};
}
}
#endif
