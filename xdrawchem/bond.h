// bond.h -- subclass of Drawable for lines/bonds

#ifndef BOND_H
#define BOND_H

#include <QRect>
#include <QPoint>

#include "drawable.h"
#include "dpoint.h"
#include "moldata.h"
#include "molpart.h"

#define BOND_LEFT 0
#define BOND_RIGHT 1
#define BOND_CENTER 2

class Render2D;

class Bond : public Drawable, public MolPart
{
Q_OBJECT
public:
    Bond();
    Bond& operator= (const Bond &);

    void Render(Render2D * r);  // draw this object
    void Edit( Render2D* r );  // open edit window
    void Move(double, double);
    void Rotate(DPoint *, double);
    void Flip(DPoint *, int);
    void Resize(DPoint *, double);
    bool Find( DPoint * ); // does this Bond contain this DPoint?
    DPoint *FindNearestPoint( DPoint *, double & );
    double distanceTo ( DPoint * );
    const QRect BoundingBox() const;
    DPoint *Start() { return start; }
    DPoint *End() { return end; }
    QString ToXML( QString ) { return "Unexpected call to Bond::ToXML"; }
    QString ToCDXML( QString ) { return "Unexpected call to Bond::ToCDXML"; }
    void FromXML( QString ) {}

    bool Highlighted() { return highlighted; }
    bool isWithinRect( QRect n, bool ) { return ( DPointInRect( start, n ) && DPointInRect( end, n ) ); }
    void SelectAll() { highlighted = true; start->setHighlighted(true); end->setHighlighted(true); }
    void DeselectAll() { highlighted = false; start->setHighlighted(false); end->setHighlighted(false); }

    void RenderSide( int w )
    {
        if ( auto_wside == 1 )
            wside = w;
    }

    void setPoints( DPoint *, DPoint * );
    bool Equals( QSharedPointer<Bond> );
    void setThick(int t) { thick = t; }
    int thickness() { return thick; }
    int Order() { return order; }

    int baseOrder()
    {
        if (order == 5) return 1;
        if (order == 7) return 1;
        return order;
    }

    void setOrder( int );
    int Stereo() { return stereo; }
    void setStereo( int a ) { stereo = a; }
    int Dash() { return dashed; }
    void setDash( int d ) { dashed = d; }
    DPoint *otherPoint( DPoint * );
    QString getID() { return id; }
    void setID(QString x) { id = x; }

    void setIR_shift( double in1 ) { IR_shift = in1; }
    void setIR_intensity( QString in1 ) { IR_intensity = in1; }
    double getIR_shift() { return IR_shift; }
    QString getIR_intensity() { return IR_intensity; }
    double Enthalpy();
    double Length();
    void setDoubleBondAlign( int db1 ) { wside = db1; }
    int getDoubleBondAlign() { return wside; }
    void setDoubleBondAuto( int db1 ) { auto_wside = db1; }
    int getDoubleBondAuto() { return auto_wside; }
    void resetReactions() { partial_charge = -999.0; rxnlist = "NONE"; }
    void setPC( double x1 ) { partial_charge = x1; }
    double getPC() { return partial_charge; }
    void setReactions( QString rxn1 ) { rxnlist = rxn1; }
    QString getReactions() { return rxnlist; }
    void setCName( QString cn1 ) { cname = cn1; }
    QString CName() { return cname; }

    bool isCHBond()
    {
        if (start->element == "H") return true;
        if (end->element == "H") return true;
        return false;
    }

    bool aromatic()
    {
        if ( (start == 0) || (end == 0) )
            return false;
        if (start->aromatic && end->aromatic)
            return true;
        return false;
    }

    double length()
    {
        int e1, e2;
        e1 = start->getAtomicNumber();
        e2 = end->getAtomicNumber();
        return MolData::bondLength(e1, e2, order);
    }

    QPoint midpoint()
    {
        double dx, dy;
        dx = ( end->x + start->x ) / 2.0;
        dy = ( end->x + start->x ) / 2.0;
        QPoint mp1( (int)dx, (int)dy );
        return mp1;
    }

private:
    friend class Molecule;
    DPoint *end;
    int thick;
    // order of bond (1-3, or 5 if up, or 7 if down)
    int order;
    // stereochemistry flag
    // values could be STEREO_UNDEFINED, STEREO_UP, STEREO_DOWN, STEREO_E,
    // STEREO_Z
    int stereo;
    // is one or more of the lines dashed
    int dashed;
    // which side to draw double bond on?
    // standing at start and looking at end
    // 0 = left, 2 = center, 1 = right; see #define's above
    // auto_wside: 0 = no auto, 1 = auto (false/true)
    int wside, auto_wside;
    // IR data
    double IR_shift;
    QString IR_intensity;
    // reactivity
    double partial_charge;
    QString rxnlist;
    // bond name
    QString cname;
    QString id;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
