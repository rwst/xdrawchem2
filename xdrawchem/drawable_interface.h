/*********************************************************************
 * drawable_interface.h
 *
 * Copyright (C)
 * 2012 Ralf Stephan -- ralf@ark.in-berlin.de
 *
 * LICENSE: GPL v. 2 see GPL.txt
 *********************************************************************/
#ifndef DRAWINTF_H
#define DRAWINTF_H

#include <QObject>
#include <QRect>
#include <QString>

class DPoint;
class Render2D;

class DrawableInterface : public QObject
{
    Q_OBJECT

public:
    virtual void Render( Render2D* ) = 0;  // draw this object
    virtual void Edit( Render2D* r ) = 0;  // edit this object
    virtual DPoint *FindNearestPoint(DPoint *, double &) = 0;
    virtual double distanceTo (DPoint *) = 0;
    virtual bool Find(DPoint *) = 0;
    virtual void Move(double, double) = 0;
    virtual void Rotate(DPoint *, double) = 0;
    virtual void Flip(DPoint *, int) = 0;
    virtual void Resize(DPoint *, double) = 0;
    virtual const QRect BoundingBox() const = 0;
    virtual DPoint *End() = 0;

    // TODO: separate out this interface
    virtual QString ToXML(QString) = 0;
    virtual QString ToCDXML(QString) = 0;
    virtual void FromXML(QString) = 0;

    enum type { drawable=0, arrow=1, bond, bracket, curvearrow, symbol };
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
