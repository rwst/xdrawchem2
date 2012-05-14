// bracket.h -- subclass of Drawable for brackets

#ifndef BRACKET_H
#define BRACKET_H

#include <QString>
#include <QRect>
#include <QColor>

#include "drawable.h"
#include "dpoint.h"

class Render2D;

class Bracket : public Drawable
{
Q_OBJECT
public:
    Bracket();
    void Render(Render2D * r);  // draw this object
    void Edit( Render2D* r );
    void Move(double, double);
    void Rotate(DPoint *, double);
    void Flip(DPoint *, int);
    void Resize(DPoint *, double);
    bool Find( DPoint * ); // does this Bracket contain this DPoint?
    DPoint *FindNearestPoint( DPoint *, double & );
    double distanceTo ( DPoint *);
    const QRect BoundingBox() const;
    DPoint *End() { return end; }
    QString ToXML( QString );
    QString ToCDXML( QString );
    void FromXML( QString );

    void setPoints( DPoint *, DPoint * );
    bool isWithinRect( QRect, bool );

    int Style() { return style; }
    void SetStyle( int s1 ) { style = s1; }
    void setText( QString s1 ) { subtext = s1; }
    QString getText() { return subtext; }
    void setFill( bool b1 ) { fill = b1; }
    bool getFill() { return fill; }
    void setFillColor( QColor fc1 ) { fillColor = fc1; }
    QColor getFillColor() { return fillColor; }
    void SetFillColorFromXML( QString );

    bool fillable()
    {
        if (style > 3)
            return true;  // see defs.h for BRACKET_<types>
        return false;
    }

private:
    // subscript text, if applicable
    QString subtext;
    // fill options, where applicable
    bool fill;
    QColor fillColor;
    int style;
    DPoint *end;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
