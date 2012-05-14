// gobject.h -- subclass of Drawable for graphic objects

#ifndef GOBJECT_H
#define GOBJECT_H

#include <QPolygon>
#include <QRect>
#include <QString>

#include "drawable.h"
#include "dpoint.h"

class Render2D;

#define TYPE_BEZIER 1
#define TYPE_GRAPHIC_LINE 2
#define TYPE_GRAPHIC_BONDMARK 3

class GraphicObject : public Drawable
{
Q_OBJECT

public:
    GraphicObject( );
    void Render(Render2D * r);  // draw this object
    void Edit( Render2D* r );
    void Move(double, double);
    void Rotate(DPoint *, double);
    void Flip(DPoint *, int);
    void Resize(DPoint *, double);
    bool Find( DPoint * ); // does this GraphicObject contain this DPoint?
    DPoint *FindNearestPoint( DPoint *, double & );
    double distanceTo ( DPoint * );
    DPoint *End() { return end; }
    QString ToXML( QString );
    QString ToCDXML( QString );
    void FromXML( QString );

    void setPoints( DPoint *, DPoint * );
    void setPointArray( QPolygon );
    bool isWithinRect( QRect, bool );
    const QRect BoundingBox() const;
    int Style() { return style; }
    void SetStyle( int s ) { style = s; }
    int objectType() { return ot; }
    void setObjectType( int t ) { ot = t; }

private:
    // Point array (for cubic bezier and possibly others)
    QPolygon objectPoints;
    // graphic object type
    int ot;
    int style;
    DPoint *end;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
