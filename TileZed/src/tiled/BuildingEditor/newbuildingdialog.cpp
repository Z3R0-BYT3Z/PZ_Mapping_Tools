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

#include "newbuildingdialog.h"
#include "ui_newbuildingdialog.h"

#include "building.h"
#include "buildingeditorwindow.h"
#include "buildingpreferences.h"
#include "buildingtemplates.h"

#include <QSettings>

static const char *KEY_TEMPLATE = "NewBuildingDialog/Template";

using namespace BuildingEditor;

NewBuildingDialog::NewBuildingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewBuildingDialog)
{
    ui->setupUi(this);

    ui->width->setMaximum(MAX_BUILDING_DIMENSION);
    ui->height->setMaximum(MAX_BUILDING_DIMENSION);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Breeze's QGroupBox min-height rule can collapse these legacy nested
    // layouts to the title line on Qt 5.14. Keep the actual controls visible
    // regardless of the selected application theme.
    ui->groupBox->setStyleSheet(QLatin1String(
                "QGroupBox#groupBox { min-height: 64px; }"));
    ui->groupBox_2->setStyleSheet(QLatin1String(
                "QGroupBox#groupBox_2 { min-height: 104px; }"));

    QSettings &settings = BuildingPreferences::instance()->settings();
    QString templateName = settings.value(QLatin1String(KEY_TEMPLATE)).toString();

    ui->comboBox->addItem(QLatin1String("<None>"));
    foreach (BuildingTemplate *def, BuildingTemplates::instance()->templates()) {
        ui->comboBox->addItem(def->name());
        if (templateName == def->name()) {
            ui->comboBox->setCurrentIndex(ui->comboBox->count() - 1);
        }
    }

    ui->width->setFocus();
}

NewBuildingDialog::~NewBuildingDialog()
{
    delete ui;
}

int NewBuildingDialog::buildingWidth() const
{
    return ui->width->value();
}

int NewBuildingDialog::buildingHeight() const
{
    return ui->height->value();
}

BuildingTemplate *NewBuildingDialog::buildingTemplate() const
{
    int index = ui->comboBox->currentIndex();
    return index ? BuildingTemplates::instance()->templateAt(index - 1) : 0;
}

void NewBuildingDialog::accept()
{
    QSettings &settings = BuildingPreferences::instance()->settings();
    BuildingTemplate *btemplate = buildingTemplate();
    settings.setValue(QLatin1String(KEY_TEMPLATE),
                      btemplate ? btemplate->name() : QString());

    QDialog::accept();
}
