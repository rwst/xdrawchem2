// curvearrow.h -- subclass of Drawable for curvearrows

#ifndef CURVEARROW_H
#define CURVEARROW_H

#include <QRect>
#include <QString>

#include "drawable.h"
#include "dpoint.h"

class Render2D;

class CurveArrow : public Drawable
{
Q_OBJECT
public:
    CurveArrow();
    void Render(Render2D *r);  // draw this object
    void Edit( Render2D* r );  // edit this object
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
    void FromXML( QString );
    QString ToCDXML( QString ) { return "Unexpected call to CurveArrow::ToCDXML"; } // TODO

    void setPoints( DPoint *, DPoint * );
    bool isWithinRect( QRect, bool );
    void SetCurve( const QString &t ) { which = t; }
    QString GetCurve() { return which; }

private:
    QString which;
    DPoint *end;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
