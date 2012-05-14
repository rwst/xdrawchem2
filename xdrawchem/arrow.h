// arrow.h -- subclass of Drawable for arrows

#ifndef ARROW_H
#define ARROW_H

#include <QRect>
#include <QPoint>

#include "drawable.h"
#include "dpoint.h"

class Render2D;

class Arrow : public Drawable
{
    Q_OBJECT

public:
    Arrow();
    void Render(Render2D *r);  // draw this object
    void Edit(Render2D *r);  // edit this object
    void Move(double, double);
    void Rotate(DPoint *, double);
    void Flip(DPoint *, int);
    void Resize(DPoint *, double);
    bool Find( DPoint * ); // does this Arrow contain this DPoint?
    DPoint *FindNearestPoint( DPoint *, double & );
    double distanceTo (DPoint*);
    const QRect BoundingBox() const;
    DPoint *End() { return end; }
    QString ToXML( QString );
    QString ToCDXML( QString );
    void FromXML( QString );

    void setPoints( DPoint *, DPoint * );
    bool isWithinRect( QRect, bool );
    int Orientation();
    QPoint Midpoint();
    int Style() { return style; }
    void SetStyle( int s1 ) { style = s1; }
    void setThick(int t) { thick = t; }

private:
    int thick, style;
    DPoint *end;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
