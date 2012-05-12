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
    GraphicObject( Render2D *, QObject *parent = 0 );
    void Render();  // draw this object
    void Edit();
    bool Find( DPoint * ); // does this GraphicObject contain this DPoint?
    DPoint *FindNearestPoint( DPoint *, double & );
    double distanceTo ( DPoint * );
    void setPoints( DPoint *, DPoint * );
    void setPointArray( QPolygon );
    bool isWithinRect( QRect, bool );
    const QRect BoundingBox() const;
    QString ToXML( QString );
    QString ToCDXML( QString );
    void FromXML( QString );
    int Style() { return style; }
    void SetStyle( int s ) { style = s; }
    int objectType() { return ot; }
    void setObjectType( int t ) { ot = t; }
private:
    // Renderer
    Render2D *r;
    // Point array (for cubic bezier and possibly others)
    QPolygon objectPoints;
    // graphic object type
    int ot;
    int style;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
