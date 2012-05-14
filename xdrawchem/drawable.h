/*********************************************************************
 * drawable.h
 *
 * Copyright (C)
 * 2004, 2005 Bryan Herger -- bherger@users.sourceforge.net
 * 2012 Ralf Stephan -- ralf@ark.in-berlin.de
 *
 * LICENSE: GPL v. 2 see GPL.txt
 *********************************************************************/

#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <QColor>
#include <QRect>

#include "drawable_interface.h"

class DPoint;
class Render2D;

class Drawable : public DrawableInterface
{
    Q_OBJECT

public:

    // stuff that all Drawables should know about
    DPoint *Start() { return start; }
    void setStart(DPoint *ns) { start = ns; }
    void SetColor(QColor c) { color = c; }
    void SetColorIfHighlighted(QColor c) { if ( highlighted ) color = c; }
    QColor GetColor() { return color; }
    bool Highlighted() { return highlighted; }
    void Highlight();
    void Highlight(bool);
    static double getAngle(DPoint *, DPoint *);
    double DistanceToLine(DPoint *, DPoint *, DPoint *);
    double DistanceBetween(QPoint, QPoint);
    bool DPointInRect(DPoint *, QRect);

    // TODO: separate this from this class
    QColor GetColorFromXML(QString);
    void SetColorFromXML(QString);
    DPoint *StartFromXML(QString);
    DPoint *EndFromXML(QString);

protected:
    // highlighted?
    bool highlighted;
    // points which define this Drawable (only start needed for TEXT)
    DPoint *start;
    // color
    QColor color;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
