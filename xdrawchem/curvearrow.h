// curvearrow.h -- subclass of Drawable for curvearrows

#ifndef CURVEARROW_H
#define CURVEARROW_H

#include <QRect>
#include <QString>

#include "render2d.h"
#include "drawable.h"
#include "dpoint.h"

class CurveArrow : public Drawable
{
Q_OBJECT
public:
    CurveArrow( Render2D *, QObject *parent = 0 );
    void Render();  // draw this object
    void Edit();
    bool Find( DPoint * ); // does this CurveArrow contain this DPoint?
    double distanceTo( DPoint * );
    DPoint *FindNearestPoint( DPoint * target, double &dist );
    void setPoints( DPoint *, DPoint * );
    const QRect BoundingBox() const;
    bool isWithinRect( QRect, bool );
    QString ToXML( QString );
    void FromXML( QString );
    void SetCurve( const QString &t ) { which = t; }
    QString GetCurve() { return which; }

private:
    // Renderer
    Render2D *r;
    QString which;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
