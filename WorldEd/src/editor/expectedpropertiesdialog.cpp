/*
 * Copyright 2026, Alree / Unjammer
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

#include "expectedpropertiesdialog.h"

#include "defaultsfile.h"
#include "world.h"
#include "worldcell.h"
#include "worlddocument.h"
#include "worldobjectvalidation.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QUndoStack>
#include <QVBoxLayout>

namespace {

bool booleanValue(const QString &value)
{
    return value.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0;
}

QStringList commaSeparatedValues(const QString &value)
{
    QStringList result;
    for (const QString &part : value.split(
         QLatin1Char(','), Qt::SkipEmptyParts)) {
        const QString trimmed = part.trimmed();
        if (!trimmed.isEmpty())
            result.append(trimmed);
    }
    return result;
}

QString initialValue(WorldCellObject *object, PropertyDef *definition)
{
    Property *property = object->properties().find(definition);
    if (property)
        return property->mValue;
    const QString resolved = WorldObjectValidation::resolvedValue(
                object, definition->mName);
    return resolved.isEmpty() ? definition->mDefaultValue : resolved;
}

}

bool ExpectedPropertiesDialog::canEdit(WorldCellObject *object)
{
    if (!object || !object->cell() || !object->type())
        return false;
    World *world = object->cell()->world();
    for (const QString &name :
         WorldObjectValidation::expectedPropertyNames(object)) {
        if (world->propertyDefinition(name))
            return true;
    }
    return false;
}

bool ExpectedPropertiesDialog::edit(WorldDocument *worldDocument,
                                    WorldCellObject *object, QWidget *parent)
{
    if (!worldDocument || !canEdit(object))
        return false;
    ExpectedPropertiesDialog dialog(worldDocument, object, parent);
    return dialog.exec() == QDialog::Accepted;
}

bool ExpectedPropertiesDialog::validate(const QString &defaultsPath,
                                        QString *summary, QString *error)
{
    DefaultsFile defaults;
    if (!defaults.read(defaultsPath)) {
        if (error)
            *error = defaults.errorString();
        return false;
    }

    World *world = new World(1, 1);
    for (PropertyEnum *propertyEnum : std::as_const(defaults.mEnums))
        world->insertPropertyEnum(world->propertyEnums().size(), propertyEnum);
    for (PropertyDef *definition : std::as_const(defaults.mPropertyDefs))
        world->addPropertyDefinition(
                    world->propertyDefinitions().size(), definition);
    for (PropertyTemplate *propertyTemplate :
         std::as_const(defaults.mTemplates)) {
        world->addPropertyTemplate(
                    world->propertyTemplates().size(), propertyTemplate);
    }
    for (ObjectType *objectType : std::as_const(defaults.mObjectTypes))
        world->insertObjectType(world->objectTypes().size(), objectType);
    for (WorldObjectGroup *objectGroup :
         std::as_const(defaults.mObjectGroups)) {
        if (objectGroup->type()->isNull())
            objectGroup->setType(world->nullObjectType());
        world->insertObjectGroup(world->objectGroups().size(), objectGroup);
    }
    defaults.mEnums.clear();
    defaults.mPropertyDefs.clear();
    defaults.mTemplates.clear();
    defaults.mObjectTypes.clear();
    defaults.mObjectGroups.clear();

    WorldDocument document(world);
    WorldCell *cell = world->cellAt(0, 0);
    int typeCount = 0;
    int fieldCount = 0;
    for (const QString &typeName :
         WorldObjectValidation::expectedObjectTypes()) {
        ObjectType *objectType = world->objectType(typeName);
        if (!objectType && typeName == QLatin1String("Vehicle"))
            objectType = world->objectType(QLatin1String("ParkingStall"));
        if (!objectType)
            continue;
        WorldObjectGroup *objectGroup = world->objectGroups().find(typeName);
        if (!objectGroup)
            objectGroup = world->nullObjectGroup();

        WorldCellObject object(cell, QString(), objectType, objectGroup,
                               0, 0, 0, 1, 1);
        if (WorldObjectValidation::supportsBasementAccess(&object)
                != (typeName == QLatin1String("Basement"))) {
            if (error)
                *error = tr("%1 has an invalid Basement access policy")
                        .arg(typeName);
            return false;
        }
        ExpectedPropertiesDialog dialog(&document, &object, nullptr);
        const QStringList expectedNames =
                WorldObjectValidation::expectedPropertyNames(typeName);
        if (dialog.mFields.size() != expectedNames.size()) {
            if (error) {
                *error = tr("%1 displayed %2 expected fields instead of %3")
                        .arg(typeName)
                        .arg(dialog.mFields.size())
                        .arg(expectedNames.size());
            }
            return false;
        }
        for (const Field &field : std::as_const(dialog.mFields)) {
            if (!expectedNames.contains(field.definition->mName)) {
                if (error) {
                    *error = tr("%1 displayed unexpected property %2")
                            .arg(typeName, field.definition->mName);
                }
                return false;
            }
        }

        dialog.restoreDefaults();
        dialog.applyChanges();
        if (object.properties().size() != expectedNames.size()) {
            if (error) {
                *error = tr("%1 did not add every expected property")
                        .arg(typeName);
            }
            return false;
        }
        document.undoStack()->undo();
        if (!object.properties().isEmpty()) {
            if (error)
                *error = tr("%1 property edit could not be undone")
                        .arg(typeName);
            return false;
        }
        document.undoStack()->redo();
        if (object.properties().size() != expectedNames.size()) {
            if (error)
                *error = tr("%1 property edit could not be redone")
                        .arg(typeName);
            return false;
        }
        document.undoStack()->clear();
        ++typeCount;
        fieldCount += dialog.mFields.size();
    }

    if (typeCount != WorldObjectValidation::expectedObjectTypes().size()) {
        if (error)
            *error = tr("Only %1 guided property types were validated")
                    .arg(typeCount);
        return false;
    }
    if (summary) {
        *summary = tr("%1 guided object types and %2 expected fields passed "
                      "dialog creation, Basement access policy, apply, "
                      "Undo, and Redo")
                .arg(typeCount).arg(fieldCount);
    }
    return true;
}

ExpectedPropertiesDialog::ExpectedPropertiesDialog(
        WorldDocument *worldDocument, WorldCellObject *object, QWidget *parent)
    : QDialog(parent)
    , mWorldDocument(worldDocument)
    , mObject(object)
{
    const QString typeName = object->type()->name();
    setWindowTitle(tr("Edit %1 Properties").arg(typeName));
    setMinimumWidth(520);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *intro = new QLabel(
                tr("Only the properties expected for this %1 object are "
                   "shown. Saving adds missing properties and updates their "
                   "values. Other properties remain unchanged.")
                .arg(typeName), this);
    intro->setWordWrap(true);
    layout->addWidget(intro);

    QWidget *formWidget = new QWidget(this);
    QFormLayout *form = new QFormLayout(formWidget);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    World *world = object->cell()->world();
    for (const QString &name :
         WorldObjectValidation::expectedPropertyNames(object)) {
        PropertyDef *definition = world->propertyDefinition(name);
        if (!definition)
            continue;

        const QString value = initialValue(object, definition);
        Field field = { definition, nullptr, TextEditor };
        if (definition->mEnum && definition->mEnum->isMulti()) {
            QListWidget *list = new QListWidget(formWidget);
            list->setAlternatingRowColors(true);
            list->setMinimumHeight(210);
            const QStringList selected = commaSeparatedValues(value);
            for (const QString &choice : definition->mEnum->values()) {
                QListWidgetItem *item = new QListWidgetItem(choice, list);
                item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                item->setCheckState(selected.contains(choice)
                                    ? Qt::Checked : Qt::Unchecked);
            }
            field.editor = list;
            field.kind = MultiEnumEditor;
        } else if (definition->mEnum) {
            QComboBox *combo = new QComboBox(formWidget);
            if (definition->mDefaultValue.isEmpty() || value.isEmpty())
                combo->addItem(QString());
            combo->addItems(definition->mEnum->values());
            if (combo->findText(value) == -1 && !value.isEmpty())
                combo->insertItem(0, value);
            combo->setCurrentIndex(qMax(0, combo->findText(value)));
            field.editor = combo;
            field.kind = EnumEditor;
        } else if (definition->mDefaultValue.compare(
                       QLatin1String("true"), Qt::CaseInsensitive) == 0
                   || definition->mDefaultValue.compare(
                       QLatin1String("false"), Qt::CaseInsensitive) == 0) {
            QCheckBox *checkBox = new QCheckBox(tr("Enabled"), formWidget);
            checkBox->setChecked(booleanValue(value));
            field.editor = checkBox;
            field.kind = BooleanEditor;
        } else {
            bool integer = false;
            definition->mDefaultValue.toInt(&integer);
            bool number = false;
            definition->mDefaultValue.toDouble(&number);
            if (integer && !definition->mDefaultValue.contains(
                    QLatin1Char('.'))) {
                QSpinBox *spinBox = new QSpinBox(formWidget);
                spinBox->setRange(-1000000000, 1000000000);
                spinBox->setValue(value.toInt());
                field.editor = spinBox;
                field.kind = IntegerEditor;
            } else if (number) {
                QDoubleSpinBox *spinBox = new QDoubleSpinBox(formWidget);
                spinBox->setRange(-1000000000.0, 1000000000.0);
                spinBox->setDecimals(4);
                spinBox->setSingleStep(0.1);
                spinBox->setValue(value.toDouble());
                field.editor = spinBox;
                field.kind = NumberEditor;
            } else {
                QLineEdit *lineEdit = new QLineEdit(value, formWidget);
                field.editor = lineEdit;
                field.kind = TextEditor;
            }
        }

        field.editor->setToolTip(definition->mDescription);
        QLabel *label = new QLabel(definition->mName, formWidget);
        label->setToolTip(definition->mDescription);
        QWidget *fieldWidget = new QWidget(formWidget);
        QVBoxLayout *fieldLayout = new QVBoxLayout(fieldWidget);
        fieldLayout->setContentsMargins(0, 0, 0, 0);
        fieldLayout->setSpacing(2);
        fieldLayout->addWidget(field.editor);
        if (!definition->mDescription.isEmpty()) {
            QLabel *description = new QLabel(
                        definition->mDescription, fieldWidget);
            description->setWordWrap(true);
            description->setEnabled(false);
            fieldLayout->addWidget(description);
        }
        form->addRow(label, fieldWidget);
        mFields.append(field);
    }

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidget(formWidget);
    layout->addWidget(scrollArea, 1);

    QDialogButtonBox *buttons = new QDialogButtonBox(
                QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    QPushButton *restoreButton = buttons->addButton(
                tr("Restore Expected Defaults"), QDialogButtonBox::ResetRole);
    buttons->button(QDialogButtonBox::Save)->setText(tr("Save Properties"));
    connect(restoreButton, &QPushButton::clicked,
            this, &ExpectedPropertiesDialog::restoreDefaults);
    connect(buttons, &QDialogButtonBox::accepted,
            this, &ExpectedPropertiesDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected,
            this, &ExpectedPropertiesDialog::reject);
    layout->addWidget(buttons);
}

QString ExpectedPropertiesDialog::fieldValue(const Field &field) const
{
    switch (field.kind) {
    case TextEditor:
        return static_cast<QLineEdit *>(field.editor)->text();
    case BooleanEditor:
        return static_cast<QCheckBox *>(field.editor)->isChecked()
                ? QLatin1String("true") : QLatin1String("false");
    case IntegerEditor:
        return QString::number(
                    static_cast<QSpinBox *>(field.editor)->value());
    case NumberEditor:
        return QString::number(
                    static_cast<QDoubleSpinBox *>(field.editor)->value(),
                    'g', 12);
    case EnumEditor:
        return static_cast<QComboBox *>(field.editor)->currentText();
    case MultiEnumEditor: {
        QStringList values;
        QListWidget *list = static_cast<QListWidget *>(field.editor);
        for (int index = 0; index < list->count(); ++index) {
            QListWidgetItem *item = list->item(index);
            if (item->checkState() == Qt::Checked)
                values.append(item->text());
        }
        return values.join(QLatin1Char(','));
    }
    }
    return QString();
}

void ExpectedPropertiesDialog::setFieldValue(const Field &field,
                                             const QString &value)
{
    switch (field.kind) {
    case TextEditor:
        static_cast<QLineEdit *>(field.editor)->setText(value);
        break;
    case BooleanEditor:
        static_cast<QCheckBox *>(field.editor)->setChecked(
                    booleanValue(value));
        break;
    case IntegerEditor:
        static_cast<QSpinBox *>(field.editor)->setValue(value.toInt());
        break;
    case NumberEditor:
        static_cast<QDoubleSpinBox *>(field.editor)->setValue(
                    value.toDouble());
        break;
    case EnumEditor: {
        QComboBox *combo = static_cast<QComboBox *>(field.editor);
        int index = combo->findText(value);
        if (index == -1 && !value.isEmpty()) {
            combo->insertItem(0, value);
            index = 0;
        }
        combo->setCurrentIndex(qMax(0, index));
        break;
    }
    case MultiEnumEditor: {
        const QStringList values = commaSeparatedValues(value);
        QListWidget *list = static_cast<QListWidget *>(field.editor);
        for (int index = 0; index < list->count(); ++index) {
            QListWidgetItem *item = list->item(index);
            item->setCheckState(values.contains(item->text())
                                ? Qt::Checked : Qt::Unchecked);
        }
        break;
    }
    }
}

void ExpectedPropertiesDialog::restoreDefaults()
{
    for (const Field &field : mFields)
        setFieldValue(field, field.definition->mDefaultValue);
}

void ExpectedPropertiesDialog::applyChanges()
{
    bool hasChanges = false;
    for (const Field &field : mFields) {
        Property *property = mObject->properties().find(field.definition);
        if (!property || property->mValue != fieldValue(field)) {
            hasChanges = true;
            break;
        }
    }
    if (!hasChanges)
        return;

    QUndoStack *undoStack = mWorldDocument->undoStack();
    undoStack->beginMacro(tr("Edit %1 Properties")
                          .arg(mObject->type()->name()));
    for (const Field &field : mFields) {
        const QString value = fieldValue(field);
        Property *property = mObject->properties().find(field.definition);
        if (!property) {
            mWorldDocument->addProperty(
                        mObject, field.definition->mName, value);
        } else if (property->mValue != value) {
            mWorldDocument->setPropertyValue(mObject, property, value);
        }
    }
    undoStack->endMacro();
}

void ExpectedPropertiesDialog::accept()
{
    for (const Field &field : mFields) {
        if (mObject->isSpawnPoint()
                && field.definition->mName == QLatin1String("Professions")
                && fieldValue(field).isEmpty()) {
            QMessageBox::warning(
                        this, tr("Profession required"),
                        tr("Select at least one profession for this "
                           "SpawnPoint."));
            return;
        }
        if (mObject->isRoomTone()
                && field.definition->mName == QLatin1String("RoomTone")
                && fieldValue(field).isEmpty()) {
            QMessageBox::warning(
                        this, tr("Room tone required"),
                        tr("Select a RoomTone audio profile."));
            return;
        }
    }
    applyChanges();
    QDialog::accept();
}
