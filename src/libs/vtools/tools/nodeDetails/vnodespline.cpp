/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017  Seamly, LLC                                       *
 *                                                                         *
 *   https://github.com/fashionfreedom/seamly2d                            *
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
 **  @file   vnodespline.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   November 15, 2013
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013-2015 Seamly2D project
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

#include "vnodespline.h"

#include <QDomElement>
#include <QStaticStringData>
#include <QStringData>
#include <QStringDataPtr>

#include "../ifc/xml/vdomdocument.h"
#include "../ifc/ifcdef.h"
#include "../vabstracttool.h"
#include "../vdatatool.h"
#include "vabstractnode.h"

const QString VNodeSpline::ToolType = QStringLiteral("modelingSpline");

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VNodeSpline constructor.
 * @param doc dom document container.
 * @param data container with variables.
 * @param id object id in container.
 * @param idSpline object id in containerSpline.
 * @param typeCreation way we create this tool.
 * @param idTool id node.
 * @param qoParent QObject parent.
 */
VNodeSpline::VNodeSpline(VAbstractPattern *doc, VContainer *data, quint32 id, quint32 idSpline,
                         const Source &typeCreation, const QString &drawName, const quint32 &idTool, QObject *qoParent)
    :VAbstractNode(doc, data, id, idSpline, drawName, idTool, qoParent)
{
    ToolCreation(typeCreation);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Create help create tool.
 * @param doc dom document container.
 * @param data container with variables.
 * @param id object id in container.
 * @param idSpline object id in containerSpline.
 * @param parse parser file mode.
 * @param typeCreation way we create this tool.
 * @param idTool id node.
 * @return pointer to node.
 */
VNodeSpline *VNodeSpline::Create(VAbstractPattern *doc, VContainer *data, quint32 id,
                                 quint32 idSpline, const Document &parse,
                                 const Source &typeCreation, const QString &drawName, const quint32 &idTool)
{
    VNodeSpline *spl = nullptr;
    if (parse == Document::FullParse)
    {
        VAbstractTool::AddRecord(id, Tool::NodeSpline, doc);
        spl = new VNodeSpline(doc, data, id, idSpline, typeCreation, drawName, idTool, doc);

        VAbstractPattern::AddTool(id, spl);
        if (idTool != NULL_ID)
        {
            //Some nodes we don't show on scene. Tool that create this nodes must free memory.
            VDataTool *tool = VAbstractPattern::getTool(idTool);
            SCASSERT(tool != nullptr)
            spl->setParent(tool);// Adopted by a tool
        }
        else
        {
            // Help to delete the node before each FullParse
            doc->AddToolOnRemove(spl);
        }
    }
    else
    {
        doc->UpdateToolData(id, data);
    }
    return spl;
}

//---------------------------------------------------------------------------------------------------------------------
QString VNodeSpline::getTagName() const
{
    return VAbstractPattern::TagSpline;
}

//---------------------------------------------------------------------------------------------------------------------
void VNodeSpline::AllowHover(bool enabled)
{
    Q_UNUSED(enabled)
    // do nothing
}

//---------------------------------------------------------------------------------------------------------------------
void VNodeSpline::AllowSelecting(bool enabled)
{
    Q_UNUSED(enabled)
    // do nothing
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief AddToFile add tag with Information about tool into file.
 */
void VNodeSpline::AddToFile()
{
    QDomElement domElement = doc->createElement(getTagName());

    doc->SetAttribute(domElement, VDomDocument::AttrId, m_id);
    doc->SetAttribute(domElement, AttrType, ToolType);
    doc->SetAttribute(domElement, AttrIdObject, idNode);
    if (idTool != NULL_ID)
    {
        doc->SetAttribute(domElement, AttrIdTool, idTool);
    }

    AddToModeling(domElement);
}
