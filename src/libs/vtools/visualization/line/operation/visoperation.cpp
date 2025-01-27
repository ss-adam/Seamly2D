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
 **  @file
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   12 9, 2016
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2016 Seamly2D project
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

#include "visoperation.h"
#include "../vgeometry/vabstractcurve.h"
#include "../vgeometry/varc.h"
#include "../vgeometry/vcubicbezier.h"
#include "../vgeometry/vcubicbezierpath.h"
#include "../vgeometry/vellipticalarc.h"
#include "../vgeometry/vgeometrydef.h"
#include "../vgeometry/vgobject.h"
#include "../vgeometry/vpointf.h"
#include "../vgeometry/vspline.h"
#include "../vgeometry/vsplinepath.h"
#include "../vmisc/vcommonsettings.h"

//---------------------------------------------------------------------------------------------------------------------
VisOperation::VisOperation(const VContainer *data, QGraphicsItem *parent)
    : VisLine(data, parent),
      objects(),
      supportColor2(QColor(qApp->Settings()->getSecondarySupportColor())),
      supportColor3(QColor(qApp->Settings()->getTertiarySupportColor())),
      points(),
      curves()
{
}

//---------------------------------------------------------------------------------------------------------------------
VisOperation::~VisOperation()
{
    qDeleteAll(points);
    qDeleteAll(curves);
}

//---------------------------------------------------------------------------------------------------------------------
void VisOperation::setObjects(QVector<quint32> objects)
{
    this->objects = objects;
}

//---------------------------------------------------------------------------------------------------------------------
void VisOperation::VisualMode(const quint32 &pointId)
{
    Q_UNUSED(pointId)
    VMainGraphicsScene *scene = qobject_cast<VMainGraphicsScene *>(qApp->getCurrentScene());
    SCASSERT(scene != nullptr)

    Visualization::scenePos = scene->getScenePos();
    RefreshGeometry();

    AddOnScene();
}

//---------------------------------------------------------------------------------------------------------------------
VScaledEllipse *VisOperation::GetPoint(quint32 i, const QColor &color)
{
    return GetPointItem(points, i, color, this);
}

//---------------------------------------------------------------------------------------------------------------------
VCurvePathItem *VisOperation::GetCurve(quint32 i, const QColor &color)
{
    if (not curves.isEmpty() && static_cast<quint32>(curves.size() - 1) >= i)
    {
        return curves.at(static_cast<int>(i));
    }
    else
    {
        auto curve = InitItem<VCurvePathItem>(color, this);
        curves.append(curve);
        return curve;
    }
    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wswitch-default")
void VisOperation::refreshMirroredObjects(const QPointF &firstPoint, const QPointF &secondPoint)
{
    int iPoint = -1;
    int iCurve = -1;
    for (int i = 0; i < objects.size(); ++i)
    {
        const quint32 id = objects.at(i);
        const QSharedPointer<VGObject> obj = Visualization::data->GetGObject(id);

        // This check helps to find missed objects in the switch
        Q_STATIC_ASSERT_X(static_cast<int>(GOType::Unknown) == 7, "Not all objects were handled.");

        switch(static_cast<GOType>(obj->getType()))
        {
            case GOType::Point:
            {
                const QSharedPointer<VPointF> p = Visualization::data->GeometricObject<VPointF>(id);

                ++iPoint;
                VScaledEllipse *point = GetPoint(static_cast<quint32>(iPoint), supportColor2);
                DrawPoint(point, static_cast<QPointF>(*p), supportColor2);

                ++iPoint;
                point = GetPoint(static_cast<quint32>(iPoint), supportColor);

                if (object1Id != NULL_ID)
                {
                    DrawPoint(point, static_cast<QPointF>(p->Flip(QLineF(firstPoint, secondPoint))), supportColor);
                }
                break;
            }
            case GOType::Arc:
            {
                iCurve = addFlippedCurve<VArc>(firstPoint, secondPoint, id, iCurve);
                break;
            }
            case GOType::EllipticalArc:
            {
                iCurve = addFlippedCurve<VEllipticalArc>(firstPoint, secondPoint, id, iCurve);
                break;
            }
            case GOType::Spline:
            {
                iCurve = addFlippedCurve<VSpline>(firstPoint, secondPoint, id, iCurve);
                break;
            }
            case GOType::SplinePath:
            {
                iCurve = addFlippedCurve<VSplinePath>(firstPoint, secondPoint, id, iCurve);
                break;
            }
            case GOType::CubicBezier:
            {
                iCurve = addFlippedCurve<VCubicBezier>(firstPoint, secondPoint, id, iCurve);
                break;
            }
            case GOType::CubicBezierPath:
            {
                iCurve = addFlippedCurve<VCubicBezierPath>(firstPoint, secondPoint, id, iCurve);
                break;
            }
            case GOType::Unknown:
                break;
        }
    }
}
QT_WARNING_POP
