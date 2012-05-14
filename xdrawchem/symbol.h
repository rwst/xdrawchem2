// symbol.h -- subclass of Drawable for symbols

#ifndef SYMBOL_H
#define SYMBOL_H

#include <QPixmap>

#include "drawable.h"
#include "molpart.h"
#include "dpoint.h"

class Render2D;

class Symbol : public Drawable, public MolPart
{
Q_OBJECT
public:
    Symbol();
    void Render(Render2D *m_renderer);  // draw this object
    void Edit( Render2D *r );  // edit this object
    void Move(double, double);
    void Rotate(DPoint *, double);
    void Flip(DPoint *, int);
    void Resize(DPoint *, double);
    bool Find( DPoint * ); // does this symbol contain this DPoint?
    DPoint *FindNearestPoint( DPoint *, double & );
    double distanceTo ( DPoint * );
    const QRect BoundingBox() const;
    DPoint *End() { return end; }
    QString ToXML( QString );
    void FromXML( QString );
    QString ToCDXML( QString ) { return "Unexpected call to Symbol::ToCDXML"; } // TODO

    bool Highlighted() { return highlighted; }
    bool isWithinRect( QRect n, bool );
    void SelectAll() { highlighted = true; }
    void DeselectAll() { highlighted = false; }

    void setPoint( DPoint * );
    bool WithinBounds( DPoint * );
    void SetSymbol( QString );
    QString GetSymbol() { return which; }
    void SetOffset( QPoint d )
    {
        if ( ( d.x() == 0 ) && ( d.y() == 0 ) ) {
            need_offset = false; offset = d;
        } else {
            need_offset = true; offset = d;
        }
    }
    QPoint GetOffset() { return offset; }
    void SetRotate( double );
    double GetRotate() { return rotation; }

private:
    // Pixmap of original and rotated, regular and highlighted symbol
    QPixmap originalRegular;
    QPixmap originalHighlighted;
    QPixmap rotateRegular;
    QPixmap rotateHighlighted;
    // Offset (if needed -- to avoid label or bond)
    bool need_offset;
    QPoint offset;
    DPoint *end;
    QString which;
    // Rotation (if needed -- if not below or on point)
    double rotation;
    // scale (if needed)
    double scale;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
