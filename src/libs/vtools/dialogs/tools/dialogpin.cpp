/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017  Seamly, LLC                                       *
 *                                                                         *
 *   https://github.com/fashionfreedom/seamly2d                             *
 *                                                                         *
 ***************************************************************************
 **
 **  Seamly2D is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Seamly2D is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Seamly2D.  If not, see <http://www.gnu.org/licenses/>.
 **
 **************************************************************************

 ************************************************************************
 **
 **  @file   dialogpin.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   31 1, 2017
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2017 Seamly2D project
 **  <https://github.com/fashionfreedom/seamly2d> All Rights Reserved.
 **
 **  Seamly2D is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Seamly2D is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Seamly2D.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#include "dialogpin.h"
#include "ui_dialogpin.h"
#include "visualization/line/vistoolpin.h"
#include "../../tools/vabstracttool.h"
#include "../../tools/vtoolseamallowance.h"

//---------------------------------------------------------------------------------------------------------------------
DialogPin::DialogPin(const VContainer *data, quint32 toolId, QWidget *parent)
    : DialogTool(data, toolId, parent),
      ui(new Ui::DialogPin),
      m_showMode(false),
      m_flagPoint(false)
{
    ui->setupUi(this);
    InitOkCancel(ui);

    FillComboBoxPoints(ui->comboBoxPoint);

    flagError = false;
    CheckState();

    connect(ui->comboBoxPiece, &QComboBox::currentTextChanged, this, [this](){ CheckPieces(); });

    vis = new VisToolPin(data);
}

//---------------------------------------------------------------------------------------------------------------------
DialogPin::~DialogPin()
{
    delete ui;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPin::EnbleShowMode(bool disable)
{
    m_showMode = disable;
    ui->comboBoxPiece->setDisabled(m_showMode);
}

//---------------------------------------------------------------------------------------------------------------------
quint32 DialogPin::GetPieceId() const
{
    return getCurrentObjectId(ui->comboBoxPiece);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPin::SetPieceId(quint32 id)
{
    if (ui->comboBoxPiece->count() <= 0)
    {
        ui->comboBoxPiece->addItem(data->GetPiece(id).GetName(), id);
    }
    else
    {
        const qint32 index = ui->comboBoxPiece->findData(id);
        if (index != -1)
        {
            ui->comboBoxPiece->setCurrentIndex(index);
        }
        else
        {
            ui->comboBoxPiece->setCurrentIndex(0);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
quint32 DialogPin::GetPointId() const
{
    return getCurrentObjectId(ui->comboBoxPoint);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPin::SetPointId(quint32 id)
{
    setCurrentPointId(ui->comboBoxPoint, id);

    VisToolPin *point = qobject_cast<VisToolPin *>(vis);
    SCASSERT(point != nullptr)
    point->setObject1Id(id);

    CheckPoint();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPin::SetPiecesList(const QVector<quint32> &list)
{
    FillComboBoxPiecesList(ui->comboBoxPiece, list);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPin::ChosenObject(quint32 id, const SceneObject &type)
{
    if (not prepare)
    {
        if (type == SceneObject::Point)
        {
            if (SetObject(id, ui->comboBoxPoint, ""))
            {
                vis->VisualMode(id);
                CheckPoint();
                prepare = true;
                this->setModal(true);
                this->show();
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPin::CheckState()
{
    SCASSERT(ok_Button != nullptr);
    ok_Button->setEnabled(m_flagPoint && flagError);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPin::ShowVisualization()
{
    AddVisualization<VisToolPin>();

    if (m_showMode)
    {
        VToolSeamAllowance *tool = qobject_cast<VToolSeamAllowance*>(VAbstractPattern::getTool(GetPieceId()));
        SCASSERT(tool != nullptr);
        auto visPoint = qobject_cast<VisToolPin *>(vis);
        SCASSERT(visPoint != nullptr);
        visPoint->setParentItem(tool);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPin::CheckPieces()
{
    if (not m_showMode)
    {
        QColor color = okColor;
        if (ui->comboBoxPiece->count() <= 0 || ui->comboBoxPiece->currentIndex() == -1)
        {
            flagError = false;
            color = errorColor;
        }
        else
        {
            flagError = true;
            color = okColor;
        }
        ChangeColor(ui->labelPiece, color);
        CheckState();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPin::CheckPoint()
{
    QColor color = okColor;
    if (ui->comboBoxPoint->currentIndex() != -1)
    {
        m_flagPoint = true;
        color = okColor;
    }
    else
    {
        m_flagPoint = false;
        color = errorColor;
    }
    ChangeColor(ui->labelPoint, color);
    CheckState();
}
