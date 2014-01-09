/************************************************************************
 **
 **  @file   varc.cpp
 **  @author Roman Telezhinsky <dismine@gmail.com>
 **  @date   November 15, 2013
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013 Valentina project
 **  <https://bitbucket.org/dismine/valentina> All Rights Reserved.
 **
 **  Valentina is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Valentina is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Valentina.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#include "varc.h"
#include "vspline.h"
#include "../exception/vexception.h"

class QRectF;

VArc::VArc ()
    :VGObject(GObject::Arc), f1(0), formulaF1(QString()), f2(0), formulaF2(QString()), radius(0),
      formulaRadius(QString()), center(VPointF())
{
}

VArc::VArc (VPointF center, qreal radius, QString formulaRadius, qreal f1, QString formulaF1, qreal f2,
            QString formulaF2, qint64 idObject, Draw::Draws mode)
    : VGObject(GObject::Arc, idObject, mode), f1(f1), formulaF1(formulaF1), f2(f2), formulaF2(formulaF2),
      radius(radius), formulaRadius(formulaRadius), center(center)
{
    _name = QString ("Arc_%1").arg(this->GetCenter().name());
}

VArc::VArc(const VArc &arc)
    : VGObject(arc), f1(arc.GetF1()), formulaF1(arc.GetFormulaF1()), f2(arc.GetF2()),
    formulaF2(arc.GetFormulaF2()), radius(arc.GetRadius()), formulaRadius(arc.GetFormulaRadius()),
    center(arc.GetCenter())
{
}

VArc &VArc::operator =(const VArc &arc)
{
    VGObject::operator=(arc);
    this->f1 = arc.GetF1();
    this->formulaF1 = arc.GetFormulaF1();
    this->f2 = arc.GetF2();
    this->formulaF2 = arc.GetFormulaF2();
    this->radius = arc.GetRadius();
    this->formulaRadius = arc.GetFormulaRadius();
    this->center = arc.GetCenter();
    return *this;
}

QPointF VArc::GetP1() const
{
    QPointF p1 ( GetCenter().x () + radius, GetCenter().y () );
    QLineF centerP1(GetCenter().toQPointF(), p1);
    centerP1.setAngle(f1);
    return centerP1.p2();
}

QPointF VArc::GetP2 () const
{
    QPointF p2 ( GetCenter().x () + radius, GetCenter().y () );
    QLineF centerP2(GetCenter().toQPointF(), p2);
    centerP2.setAngle(f2);
    return centerP2.p2();
}

QPainterPath VArc::GetPath() const
{
    QPainterPath Path;
    QPointF center = GetCenter().toQPointF();
    QRectF rect(center.x()-radius, center.y()-radius, radius*2, radius*2);
    Path.moveTo(GetP1());
    qreal angle = QLineF(center, GetP1()).angleTo(QLineF(center, GetP2()));
    Path.arcTo(rect, GetF1(), angle);
    return Path;
}

qreal VArc::AngleArc() const
{
    QLineF l1(0, 0, 100, 100);
    l1.setAngle(f1);
    QLineF l2(0, 0, 100, 100);
    l2.setAngle(f2);
    return l1.angleTo(l2);
}

qint32 VArc::NumberSplOfArc() const
{
    qint32 angArc = static_cast<qint32> (AngleArc ());
    switch ( angArc )
    {
        case 0:
        {
            QString error = QString(tr("Angle of arc can't be 0 degree."));
            throw VException(error);
        }
        case 90:
            return 1;
        case 180:
            return 2;
        case 270:
            return 3;
        case 360:
            return 4;
        default:
            return static_cast<qint32> (AngleArc ( ) / 90 + 1);
    }
}

QVector<QPointF> VArc::GetPoints() const
{
    QVector<QPointF> points;
    qint32 numberSpl =  NumberSplOfArc();
    for (qint32 i = 1; i <= numberSpl; ++i)
    {
        points<<SplOfArc ( i );
    }
    return points;
}

QVector<QPointF> VArc::SplOfArc(qint32 number) const
{
    qint32 n = NumberSplOfArc ();
    if ( number > n )
    {
        QString error = QString(tr("Arc has not this number of parts."));
        throw VException(error);
    }
    qreal f1 = GetF1 ();
    qreal f2 = GetF2 ();
    qint32 i;
    for ( i = 0; i < n; ++i )
    {
        if ( i == n - 1 )
        {
            f2 = GetF2 ();
        }
        else
        {
            if ( f1 + 90 > 360 )
            {
                f2 = f1 + 90 - 360;
            }
            else
            {
                f2 = f1 + 90;
            }
        }
        qreal anglF1, anglF2;
        if ( f1 + 90 > 360 )
        {
            anglF1 =  f1 + 90 - 360;
        }
        else
        {
            anglF1 = f1 + 90;
        }
        if ( f2 - 90 < 0 )
        {
            anglF2 = 360 + f2 - 90;
        }
        else
        {
            anglF2 = f2 - 90;
        }
        if ( i + 1 == number )
        {
            f1 = f2;
            return VSpline::SplinePoints(GetP1 (), GetP2 (), anglF1, anglF2, 1., 1., 1.);
        }
    }
    return QVector<QPointF>();
}

QPointF VArc::CutArc(const qreal &length, VArc &arc1, VArc &arc2) const
{
    //Always need return two arcs, so we must correct wrong length.
    qreal len = 0;
    if (length < this->GetLength()*0.02)
    {
        len = this->GetLength()*0.02;
    }
    else if ( length > this->GetLength()*0.98)
    {
        len = this->GetLength()*0.98;
    }
    else
    {
        len = length;
        qDebug()<<len;
    }

    qreal n = (len*180)/(M_PI*radius);
    qDebug()<<n;
    QLineF line(GetCenter().toQPointF(), GetP1());
    line.setAngle(line.angle()+n);
    QPointF point = line.p2();

    arc1 = VArc (center, radius, formulaRadius, f1, formulaF1, line.angle(), QString().setNum(line.angle()),
                 idObject, mode);

    arc2 = VArc (center, radius, formulaRadius, line.angle(), QString().setNum(line.angle()), f2, formulaF2,
                 idObject, mode);
    return point;
}
