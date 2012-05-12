// arrow.h -- subclass of Drawable for arrows

#ifndef ARROW_H
#define ARROW_H

#include <QRect>
#include <QPoint>

#include "render2d.h"
#include "drawable.h"
#include "dpoint.h"

class Arrow : public Drawable
{
    Q_OBJECT

public:
    Arrow( Render2D *, QObject *parent = 0 );
    void Render();  // draw this object
    void Edit();  // edit this object
    bool Find( DPoint * ); // does this Arrow contain this DPoint?
    DPoint *FindNearestPoint( DPoint *, double & );
    double distanceTo (DPoint*);
    void setPoints( DPoint *, DPoint * );
    const QRect BoundingBox() const;
    bool isWithinRect( QRect, bool );
    QString ToXML( QString );
    QString ToCDXML( QString );
    void FromXML( QString );
    int Orientation();
    QPoint Midpoint();
    int Style() { return style; }
    void SetStyle( int s1 ) { style = s1; }
    void setThick(int t) { thick = t; }

private:
    // Renderer
    Render2D *m_renderer;
    int thick, style;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
