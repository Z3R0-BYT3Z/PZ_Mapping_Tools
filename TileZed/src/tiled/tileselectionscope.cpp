#include "tileselectionscope.h"
#include <QAction>
#include <QActionGroup>
#include <QIcon>
#include <QMenu>
#include <QToolButton>
using namespace Tiled::Internal;
TileSelectionScope::TileSelectionScope(const QString &levelLabel,
                                       QObject *parent)
    : QObject(parent)
    , mLevelLabel(levelLabel)
    , mLayerMode(CurrentLayer)
    , mLevelMode(CurrentLevel)
{
}
bool TileSelectionScope::includesLayer(const QString &layerName,
                                       bool visible,
                                       bool current) const
{
    switch (mLayerMode) {
    case CurrentLayer:
        return current;
    case VisibleLayers:
        return visible;
    case AllLayers:
        return true;
    case SpecificLayers:
        return mSpecificLayers.contains(layerName);
    }
    return false;
}
QToolButton *TileSelectionScope::createToolButton(
        QWidget *parent,
        const std::function<QStringList()> &layerNamesProvider)
{
    QToolButton *button = new QToolButton(parent);
    button->setObjectName(QStringLiteral("tileSelectionScopeButton"));
    button->setAutoRaise(true);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setIcon(QIcon(QStringLiteral(
            ":images/22x22/stock-tool-rect-select.png")));
    button->setToolTip(tr("Choose which tile layers and %1s are affected by "
                          "copy, cut, delete and paste")
                       .arg(mLevelLabel.toLower()));
    QMenu *menu = new QMenu(button);
    QMenu *layersMenu = menu->addMenu(tr("Layers"));
    QActionGroup *layerGroup = new QActionGroup(layersMenu);
    layerGroup->setExclusive(true);
    auto addLayerMode = [this, layersMenu, layerGroup](
            const QString &text, LayerMode mode) {
        QAction *action = layersMenu->addAction(text);
        action->setCheckable(true);
        action->setChecked(mLayerMode == mode);
        layerGroup->addAction(action);
        connect(action, &QAction::triggered, this, [this, mode]() {
            mLayerMode = mode;
            updateButtons();
            emit scopeChanged();
        });
    };
    addLayerMode(tr("Current Layer"), CurrentLayer);
    addLayerMode(tr("Visible Layers"), VisibleLayers);
    addLayerMode(tr("All Layers"), AllLayers);
    QMenu *specificMenu = layersMenu->addMenu(tr("Specific Layers"));
    connect(specificMenu, &QMenu::aboutToShow, this,
            [this, specificMenu, layerNamesProvider]() {
        rebuildSpecificLayersMenu(specificMenu, layerNamesProvider);
    });
    QMenu *levelsMenu = menu->addMenu(mLevelLabel + QLatin1Char('s'));
    QActionGroup *levelGroup = new QActionGroup(levelsMenu);
    levelGroup->setExclusive(true);
    QAction *currentLevel = levelsMenu->addAction(
                tr("Current %1").arg(mLevelLabel));
    currentLevel->setCheckable(true);
    currentLevel->setChecked(true);
    levelGroup->addAction(currentLevel);
    connect(currentLevel, &QAction::triggered, this, [this]() {
        mLevelMode = CurrentLevel;
        updateButtons();
        emit scopeChanged();
    });
    QAction *allLevels = levelsMenu->addAction(
                tr("All %1s").arg(mLevelLabel));
    allLevels->setCheckable(true);
    levelGroup->addAction(allLevels);
    connect(allLevels, &QAction::triggered, this, [this]() {
        mLevelMode = AllLevels;
        updateButtons();
        emit scopeChanged();
    });
    button->setMenu(menu);
    mButtons.append(button);
    updateButtons();
    return button;
}
void TileSelectionScope::rebuildSpecificLayersMenu(
        QMenu *menu,
        const std::function<QStringList()> &layerNamesProvider)
{
    menu->clear();
    QStringList names = layerNamesProvider ? layerNamesProvider()
                                           : QStringList();
    names.removeDuplicates();
    names.sort(Qt::CaseInsensitive);
    if (names.isEmpty()) {
        QAction *empty = menu->addAction(tr("No tile layers"));
        empty->setEnabled(false);
        return;
    }
    for (const QString &name : names) {
        QAction *action = menu->addAction(name);
        action->setCheckable(true);
        action->setChecked(mSpecificLayers.contains(name));
        connect(action, &QAction::toggled, this,
                [this, name](bool checked) {
            if (checked) {
                if (!mSpecificLayers.contains(name))
                    mSpecificLayers.append(name);
            } else {
                mSpecificLayers.removeAll(name);
            }
            mLayerMode = SpecificLayers;
            updateButtons();
            emit scopeChanged();
        });
    }
}
QString TileSelectionScope::summaryText() const
{
    QString layers;
    switch (mLayerMode) {
    case CurrentLayer:
        layers = tr("Current layer");
        break;
    case VisibleLayers:
        layers = tr("Visible layers");
        break;
    case AllLayers:
        layers = tr("All layers");
        break;
    case SpecificLayers:
        layers = mSpecificLayers.isEmpty()
                ? tr("No layers")
                : tr("%1 selected layer(s)").arg(mSpecificLayers.size());
        break;
    }
    const QString levels = mLevelMode == CurrentLevel
            ? tr("current %1").arg(mLevelLabel.toLower())
            : tr("all %1s").arg(mLevelLabel.toLower());
    return tr("%1, %2").arg(layers, levels);
}
void TileSelectionScope::updateButtons()
{
    const QString text = summaryText();
    for (QToolButton *button : mButtons) {
        if (button)
            button->setText(text);
    }
}
