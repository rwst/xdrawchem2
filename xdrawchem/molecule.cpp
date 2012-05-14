/*********************************************************************
 * molecule.cpp
 *
 * Copyright (C)
 * 2004, 2005 Bryan Herger -- bherger@users.sourceforge.net
 * 2012 Ralf Stephan -- ralf@ark.in-berlin.de
 *
 * LICENSE: GPL v. 2 see GPL.txt
 *********************************************************************/

#include "moldata.h"
#include "render2d.h"
#include "drawable.h"
#include "molecule.h"
#include "text.h"
#include "dpoint.h"
#include "defs.h"

Molecule::Molecule()
{
    text_mw = 0;
    text_formula = 0;
    group_type = GROUP_NONE;
    showPC = false;

    //peaklist.setAutoDelete(true);
    //bonds.setAutoDelete(true);
    //labels.setAutoDelete(true);
    //symbols.setAutoDelete(true);
}

/// Copy and construct completely new.
Molecule::Molecule (Molecule* m)
{
    start = new DPoint(m->start);
    end = new DPoint(m->end);

    cd = m->cd;

    peaklist = m->peaklist;
    foreach (QSharedPointer<Bond> b, m->bonds) {
        QSharedPointer<Bond> nb (new Bond());
        *nb = *b;
        bonds.append(nb);
    }
    foreach (QSharedPointer<Text> l, m->labels)
        labels.append(l);
    foreach (QSharedPointer<Symbol> s, m->symbols)
        symbols.append(s);
    foreach (DPoint *d, m->up)
        up.append(new DPoint(d));
    text_mw = m->text_mw;
    text_formula = m->text_formula;
    group_type = m->group_type;
    showPC = m->showPC;
}


/// clears all lists
Molecule::~Molecule()
{
    qDeleteAll( peaklist );
    peaklist.clear();
    bonds.clear();
    labels.clear();
    symbols.clear();
}

/// Calls AllPoints(), update double bond states, draw group box if needed, render bonds, texts, symbols.
void Molecule::Render(Render2D *r)
{
    doChanged();

    /// collect all points
    AllPoints();

    /// update double bond states
    int nleft = 0, nright = 0;
    double ang;

    foreach ( QSharedPointer<Bond> tb2, bonds ) {
        if ( tb2->Order() == 2 ) {
            nleft = 0;
            nright = 0;
            foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
                if ( tmp_bond == tb2 )
                    continue;
                if ( tmp_bond->Find( tb2->Start() ) ) {
                    ang = Angle( tb2, tmp_bond );
                    if ( ang < 180.0 )
                        nleft++;
                    else
                        nright++;
                }
                if ( tmp_bond->Find( tb2->End() ) ) {
                    ang = Angle( tb2, tmp_bond );
                    if ( ang < 180.0 )
                        nleft++;
                    else
                        nright++;
                }
            }
            if ( nleft > nright )
                tb2->RenderSide( 0 );
            else
                tb2->RenderSide( 1 );
        }
    }
    // draw group box if needed
    if ( group_type != GROUP_NONE ) {
        QRect gb1 = BoundingBoxAll();

        gb1.setLeft( gb1.left() - 6 );
        gb1.setRight( gb1.right() + 6 );
        gb1.setBottom( gb1.bottom() + 6 );
        r->drawLine( gb1.topLeft(), gb1.bottomLeft(), 1, QColor( 0, 0, 0 ), 0 );
        r->drawLine( gb1.bottomLeft(), gb1.bottomRight(), 1, QColor( 0, 0, 0 ), 0 );
        r->drawLine( gb1.bottomRight(), gb1.topRight(), 1, QColor( 0, 0, 0 ), 0 );
        QPoint tl1( gb1.topLeft().x() + 2, gb1.topLeft().y() - 4 );
        QString label1;

        if ( group_type == GROUP_REACTANT )
            label1 = tr( "Reactant" );
        if ( group_type == GROUP_PRODUCT )
            label1 = tr( "Product" );
        r->drawString( label1, tl1, QColor( 0, 0, 0 ), QFont( "times", 8 ) );
    }

    foreach ( QSharedPointer<Bond> tmp_bond, bonds )
        tmp_bond->Render(r);

    foreach ( QSharedPointer<Text> tmp_text, labels ) {
        // ignore if just "C"
        //if (tmp_text->getText() == "C") continue;
        tmp_text->Render(r);
    }
    CalcOffsets();
    foreach ( QSharedPointer<Symbol> tmp_sym, symbols )
        tmp_sym->Render(r);
}

/// Calculate offsets, i.e., where to put a Symbol.
void Molecule::CalcOffsets()
{
    bool top = true, bottom = true, left = true, right = true;
    DPoint *op;
    double ang;

    foreach ( QSharedPointer<Symbol> tmp_sym, symbols ) {
        top = true;
        bottom = true;
        left = true;
        right = true;
        DPoint *tpt = tmp_sym->Start();
        foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
            if ( tmp_bond->Find( tpt ) ) {
                op = tmp_bond->otherPoint( tpt );
                ang = tmp_bond->getAngle( tpt, op );
                if ( ( ang > 300.0 ) || ( ang < 60.0 ) )
                    right = false;
                if ( ( ang > 30.0 ) && ( ang < 150.0 ) )
                    bottom = false;
                if ( ( ang > 120.0 ) && ( ang < 240.0 ) )
                    left = false;
                if ( ( ang > 210.0 ) && ( ang < 330.0 ) )
                    top = false;
            }
        }
        if ( bottom == true ) {
            tmp_sym->SetOffset( QPoint( 0, 12 ) );
            tmp_sym->SetRotate( 0.0 );
            continue;
        }
        if ( top == true ) {
            tmp_sym->SetOffset( QPoint( 0, -12 ) );
            tmp_sym->SetRotate( 180.0 );
            continue;
        }
        if ( right == true ) {
            tmp_sym->SetOffset( QPoint( 12, 0 ) );
            tmp_sym->SetRotate( -90.0 );
            continue;
        }
        if ( left == true ) {
            tmp_sym->SetOffset( QPoint( -12, 0 ) );
            tmp_sym->SetRotate( 90.0 );
            continue;
        }
        tmp_sym->SetOffset( QPoint( 0, 12 ) );
    }
}

/// Returns if this has a bond that returns true for Find().
bool Molecule::Find( DPoint * target )
{
    foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
        if ( tmp_bond->Find( target ) == true )
            return true;
    }
    return false;
}

/// Returns if parts of this are isWithinRect(param), handles shiftdown
bool Molecule::isWithinRect( QRect qr, bool shiftdown )
{
    if ( shiftdown )
        qDebug() << "shiftdown";
    bool ret = false;

    foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
        if ( tmp_bond->isWithinRect( qr, false ) )
            ret = true;
    }
    if ( ( ret == true ) && ( shiftdown == true ) ) {
        qDebug() << "selectall";
        SelectAll();
        return false;
    }
    foreach ( QSharedPointer<Text> tmp_text, labels )
        tmp_text->isWithinRect( qr, false );

    foreach ( QSharedPointer<Symbol> tmp_sym, symbols )
        tmp_sym->isWithinRect( qr, false );
    return false;
}

/// Calls SelectAll() for bonds, labels, symbols
void Molecule::SelectAll()
{
    foreach ( QSharedPointer<Bond> tmp_bond, bonds )
        tmp_bond->SelectAll();

    foreach ( QSharedPointer<Text> tmp_text, labels )
        tmp_text->SelectAll();

    foreach ( QSharedPointer<Symbol> tmp_sym, symbols )
        tmp_sym->SelectAll();
}

/// Calls DeselectAll() for bonds, labels, symbols
void Molecule::DeselectAll()
{
    foreach ( QSharedPointer<Bond> tmp_bond, bonds )
        tmp_bond->DeselectAll();

    foreach ( QSharedPointer<Text> tmp_text, labels )
        tmp_text->DeselectAll();

    foreach ( QSharedPointer<Symbol> tmp_sym, symbols )
        tmp_sym->DeselectAll();
}

/// Calls SetColorIfHighlighted() for bonds, labels, symbols
void Molecule::SetColorIfHighlighted( QColor c )
{
    foreach ( QSharedPointer<Bond> tmp_bond, bonds )
        tmp_bond->SetColorIfHighlighted( c );

    foreach ( QSharedPointer<Text> tmp_text, labels )
        tmp_text->SetColorIfHighlighted( c );

    foreach ( QSharedPointer<Symbol> tmp_sym, symbols )
        tmp_sym->SetColorIfHighlighted( c );
}

/// TODO: Copy Text labels into DPoint::element (needed esp. by save, CalcMW, NMR prediction...)
void Molecule::CopyTextToDPoint()
{
    // outcommented because incomplete and unused
    /*
    foreach ( QSharedPointer<Text> tmp_text, labels ) {
        DPoint *tpt = tmp_text->Start();
        tpt->element = tmp_text->getText();
        // clear aromaticity here
        tmp_pt->aromatic = false;
    }
    */
}

/// Calls SelectAll(), BoundingBox(), DeselectAll()
QRect Molecule::BoundingBoxAll()
{
    QRect fr;

    SelectAll();
    fr = BoundingBox();
    DeselectAll();
    return fr;
}

/// True if this->BoundingBoxAll() contains param.
bool Molecule::WithinBounds( DPoint * target )
{
    QRect fr = BoundingBoxAll();

    return fr.contains( target->toQPoint() );
}

/**
 * Determine if split is necessary (e.g., if this structure contains two
 * or more fragments).  Return empty list if not, return new Molecule's
 * if needed.
 ***/
QList < QSharedPointer <Molecule> > Molecule::MakeSplit()
{
//  QList<DPoint *> up;
    QList < DPoint * >pointqueue;
    QList <QSharedPointer<Bond> > uo (bonds);
    QList <QSharedPointer<Bond> >removequeue;
    QList < QSharedPointer<Molecule> >molecules;
    DPoint *current;

    // Start with first Bond...
    QSharedPointer<Molecule> nm ( new Molecule());
    nm->SetChemdata( cd );
    QSharedPointer<Bond> tmp_bond = uo.first();
    nm->addBond( tmp_bond );
    uo.removeFirst();
    pointqueue.append( tmp_bond->Start() );
    pointqueue.append( tmp_bond->End() );
    do {
        do {
            current = pointqueue.first();
            pointqueue.removeFirst();
            foreach ( QSharedPointer<Bond> tmp_bond, uo ) {
                if ( tmp_bond->Find( current ) ) {
                    pointqueue.append( tmp_bond->otherPoint( current ) );
                    nm->addBond( tmp_bond );
                    removequeue.append( tmp_bond );
                }
            }
            foreach ( QSharedPointer<Bond> t, removequeue ) {
                uo.removeAll( t );
            }
            removequeue.clear();
        } while ( pointqueue.count() > 0 );
        if ( uo.count() > 0 ) {
            // Still bonds left, make new Molecule
            molecules.append( nm );
            nm.clear();
            QSharedPointer<Molecule> nm1 ( new Molecule());
            nm = nm1;
            nm->SetChemdata( cd );
            QSharedPointer<Bond> tb = uo.first();
            nm->addBond( tb );
            uo.removeFirst();
            pointqueue.append( tb->Start() );
            pointqueue.append( tb->End() );
        }
    } while ( uo.count() > 0 );
    molecules.append( nm );
    qDebug() << "Found " << molecules.count() << " distinct structures.";
    qDebug() << nm->Members();
    // Now split Text and Symbol lists
    if ( molecules.count() > 1 ) {
        foreach ( QSharedPointer<Text> tmp_text, labels ) {
            foreach ( nm, molecules ) {
                if ( nm->Find( tmp_text->Start() ) )
                    nm->addText( tmp_text );
            }
        }
        foreach ( QSharedPointer<Symbol> tmp_sym, symbols ) {
            foreach ( QSharedPointer<Molecule> tm , molecules ) {
                if ( tm->Find( tmp_sym->Start() ) )
                    tm->addSymbol( tmp_sym );
            }
        }
    }
    return molecules;
}

/// Create and return a list of all unique DPoint's in this Molecule
QList < DPoint * >Molecule::AllPoints()
{
    QList < DPoint * >pl;

    foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
        DPoint *tpt = tmp_bond->Start();
        if ( pl.contains( tpt ) == 0 )
            pl.append( tpt );
        tpt = tmp_bond->End();
        if ( pl.contains( tpt ) == 0 )
            pl.append( tpt );
    }
    foreach ( QSharedPointer<Text> tmp_text, labels ) {
        DPoint *tpt = tmp_text->Start();
        if ( pl.contains( tpt ) == 0 )
            pl.append( tpt );
    }
    // Copy Text labels to DPoint's as needed
    // tODO: this function's result is NOT used
    CopyTextToDPoint();
    // Determine number of substituents at each point
    int c1 = 0;

    foreach ( DPoint *tpt, pl ) {
        tpt->serial = c1;    // serialize atoms
        c1++;
        tpt->hit = false;    // also reset "hit"
        tpt->substituents = 0;
        foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
            if ( tmp_bond->Find( tpt ) )
                tpt->substituents += tmp_bond->Order();
        }
    }
    // serialize atoms -- copy list place to serial field of DPoint
    //for (int c1 = 0; c1 < pl.count(); c1++) {
    //  tmp_pt = pl.at(c1);
    //  tmp_pt->serial = c1;
    //  tmp_pt->hit = false;  // also reset "hit"
    //}
    //qDebug() << "AllPoints():" << pl.count() ;
    return pl;
}

/// Create and return a list of all unique Drawable's in this Molecule
QList<QSharedPointer<Drawable> > Molecule::AllObjects()
{
    QList < QSharedPointer <Drawable> > dl;

    foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
        dl.append( tmp_bond.objectCast<Drawable>() );
    }
    foreach ( QSharedPointer<Text> tmp_text, labels ) {
        dl.append( tmp_text.objectCast<Drawable>() );
    }
    foreach ( QSharedPointer<Symbol> tmp_sym, symbols ) {
        dl.append( tmp_sym.objectCast<Drawable>() );
    }
    return dl;
}

/// Returns read-only access to bonds
const QList <QSharedPointer<Bond> > Molecule::AllBonds()
{
    return (bonds);
}

/// Returns a list of unique DPoints to move, only used internally
void MakeTomoveList(Molecule* m, QList<DPoint*> &tomove)
{
    tomove.clear();
    foreach ( QSharedPointer<Bond> tmp_bond, m->bonds ) {
        if ( tmp_bond->Highlighted() ) {
            DPoint *tpt = tmp_bond->Start();
            if ( tomove.contains( tpt ) == 0 )
                tomove.append( tpt );
            tpt = tmp_bond->End();
            if ( tomove.contains( tpt ) == 0 )
                tomove.append( tpt );
        }
    }
    foreach ( QSharedPointer<Text> tmp_text, m->labels ) {
        if ( tmp_text->Highlighted() ) {
            DPoint *tpt = tmp_text->Start();
            if ( tomove.contains( tpt ) == 0 )
                tomove.append( tpt );
        }
    }
    foreach ( QSharedPointer<Symbol> tmp_sym, m->symbols ) {
        if ( tmp_sym->Highlighted() ) {
            DPoint *tpt = tmp_sym->Start();
            if ( tomove.contains( tpt ) == 0 )
                tomove.append( tpt );
        }
    }
}

/// Adds param to start, end, and all points to move. Calls Changed().
void Molecule::Move( double dx, double dy )
{
    start->x += dx;
    start->y += dy;
    end->x += dx;
    end->y += dy;

    QList<DPoint*> tomove;
    MakeTomoveList(this, tomove);

    foreach ( DPoint *tpt, tomove ) {
        tpt->x += dx;
        tpt->y += dy;
    }
    Changed();
}

/// Rotates by param all points to move. Calls Changed().
void Molecule::Rotate( DPoint * origin, double angle )
{
    QList<DPoint*> tomove;
    MakeTomoveList(this, tomove);

    foreach ( DPoint *tpt, tomove ) {
        double thisx = tpt->x - origin->x;
        double thisy = tpt->y - origin->y;
        double newx = thisx * cos( angle ) + thisy * sin( angle );
        double newy = -thisx * sin( angle ) + thisy * cos( angle );

        tpt->x = newx + origin->x;
        tpt->y = newy + origin->y;
    }
    Changed();
}

/// Computes center of molecule, rotates by param all points to move. Calls Changed(). TODO: call Rotate(...,...)
void Molecule::Rotate( double angle )
{
    QList<DPoint*> tomove;
    MakeTomoveList(this, tomove);

    double centerx = 0.0, centery = 0.0;
    int n = 0;

    foreach ( DPoint *tpt, tomove ) {
        centerx += tpt->x;
        centery += tpt->y;
        n++;
    }
    centerx /= ( double ) n;
    centery /= ( double ) n;

    foreach ( DPoint *tpt, tomove ) {
        double thisx = tpt->x - centerx;
        double thisy = tpt->y - centery;
        double newx = thisx * cos( angle ) + thisy * sin( angle );
        double newy = -thisx * sin( angle ) + thisy * cos( angle );

        tpt->x = newx + centerx;
        tpt->y = newy + centery;
    }
    Changed();
}

/// Flip by origin param in direction param all points to move. Calls Changed().
void Molecule::Flip( DPoint * origin, int direction )
{
    QList<DPoint*> tomove;
    MakeTomoveList(this, tomove);
    double delta;

    foreach ( DPoint *tpt, tomove ) {
        if ( direction == FLIP_H ) {
            delta = tpt->x - origin->x;
            tpt->x -= 2 * delta;
        } else {                // direction == FLIP_V
            delta = tpt->y - origin->y;
            tpt->y -= 2 * delta;
        }
    }
    Changed();
}

/// Multiply all points tgo move vectors by scale param, taking param as origin. Calls Changed().
void Molecule::Resize( DPoint * origin, double scale )
{
    double dx, dy;
    QList<DPoint*> tomove;
    MakeTomoveList(this, tomove);

    foreach ( DPoint *tpt, tomove ) {
        dx = tpt->x - origin->x;
        dy = tpt->y - origin->y;
        dx *= scale;
        dy *= scale;
        tpt->x = origin->x + dx;
        tpt->y = origin->y + dy;
    }
    Changed();
}

/// Returns bounding box of all bonds and labels in this molecule.
const QRect Molecule::BoundingBox() const
{
    int top = 99999, bottom = 0, left = 99999, right = 0;
    QRect tmprect;

    foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
        tmprect = tmp_bond->BoundingBox();
        if ( tmprect.isValid() ) {
            if ( tmprect.left() < left )
                left = tmprect.left();
            if ( tmprect.right() > right )
                right = tmprect.right();
            if ( tmprect.top() < top )
                top = tmprect.top();
            if ( tmprect.bottom() > bottom )
                bottom = tmprect.bottom();
        }
    }
    foreach ( QSharedPointer<Text> tmp_text, labels ) {
        tmprect = tmp_text->BoundingBox();
        if ( tmprect.isValid() ) {
            if ( tmprect.left() < left )
                left = tmprect.left();
            if ( tmprect.right() > right )
                right = tmprect.right();
            if ( tmprect.top() < top )
                top = tmprect.top();
            if ( tmprect.bottom() > bottom )
                bottom = tmprect.bottom();
        }
    }

    return QRect( QPoint( left, top ), QPoint( right, bottom ) );
}

/// Returns point in this molecule nearest param within param.
DPoint *Molecule::FindNearestPoint( DPoint * target, double &dist )
{
    DPoint *nearest = 0, *d1;
    double mindist = 99999.0, d1dist = 999999.0;

    foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
        d1 = tmp_bond->FindNearestPoint( target, d1dist );
        if ( d1dist < mindist ) {
            mindist = d1dist;
            nearest = d1;
        }
    }
    dist = mindist;
    if ( !nearest )
        return 0;
    nearest->other = 0;
    bool already_found = false;

    foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
        if ( tmp_bond->Find( nearest ) == true ) {
            if ( already_found == true ) {
                nearest->other = 0;
                return nearest;
            } else {
                already_found = true;
                nearest->other = tmp_bond->otherPoint( nearest );
            }
        }
    }
    // check text objects to see if nearest point has a label
    // if it does, then don't select this point.
    //for (tmp_text=labels.first(); tmp_text != 0; tmp_text = labels.next()) {
    //if (tmp_text->Start() == nearest) dist = 9999.0;
    //}
    return nearest;
}

/// Returns smallest distance of this molecule's labels, bonds, symbols to param.
double Molecule::distanceTo ( DPoint * target )
{
    double mindist = 999999.0, d1dist = 999999.0;

    // if on Text label, highlight it...
    foreach ( QSharedPointer<Text> tmp_text, labels ) {
        if ( tmp_text->WithinBounds( target ) ) {
            return 0.01;
        }
    }
    // if on Symbol, highlight it...
    foreach ( QSharedPointer<Symbol> tmp_sym, symbols ) {
        if ( tmp_sym->WithinBounds( target ) ) {
            return 0.01;
        }
    }

    QSharedPointer<Drawable> nearest;
    foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
        d1dist = tmp_bond->distanceTo( target );
        if ( d1dist < mindist )
            mindist = d1dist;
    }
    return mindist;
}

/// Creates new bond from params, append it to bonds if not already there, calls Changed().
void Molecule::addBond( DPoint * s, DPoint * e, int thick, int order, QColor c, bool hl )
{
    int o, p;
    QSharedPointer<Bond> b ( new Bond());

    b->setPoints( s, e );
    b->setThick( thick );
    if ( preferences.getBondCenter() == true ) {
        b->setDoubleBondAuto( 0 );
        b->setDoubleBondAlign( 2 );
    }
    if ( order != 99 ) {
        b->setOrder( order );
    } else {
        b->setDash( 1 );
        b->setOrder( 1 );
    }
    if ( order == 4 ) {         /// MDL molfiles store aromaticity this way
        b->setDash( 1 );
        b->setOrder( 2 );
    }
    b->SetColor( c );
    if ( hl )
        b->Highlight( true );
    /// check if bond exists before adding
    foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
        if ( tmp_bond->Equals( b ) ) {
            o = tmp_bond->Order();
            p = tmp_bond->Dash();
            if ( o < 3 )
                tmp_bond->setOrder( o + 1 );
            if ( order == 99 )
                tmp_bond->setDash( p + 1 );
            Changed();
            return;
        }
    }
    bonds.append( b );
    Changed();
}

/// Append param to bonds if not already there, calls Changed().
void Molecule::addBond(QSharedPointer<Bond> b )
{
    // but only add if it's not already there.
    if ( bonds.count( b ) == 0 )
        bonds.append( b );
    Changed();
}

/// Append param to labels if not already there, calls Changed().
void Molecule::addText( QSharedPointer<Text> t )
{
    // remove Text associated with this point, if any
    DPoint *findpt = t->Start();

    foreach ( QSharedPointer<Text> tmp_text, labels ) {
        if ( tmp_text->Start() == findpt ) {
            labels.removeAll( tmp_text );
            break;
        }
    }

    t->setMolecule( this );
    labels.append( t );
    Changed();
}

/// Append param to symbols, calls Changed().
void Molecule::addSymbol( QSharedPointer<Symbol> s )
{
    symbols.append( s );
    Changed();
}

/// Appends other molecule's bonds and labels to this, calls Changed().
void Molecule::addMolecule( Molecule *m1 )
{
    qDebug() << "Uh-oh!  Need to merge";
    foreach ( QSharedPointer<Bond> tmp_bond , m1->bonds ) {
        bonds.append( tmp_bond );
    }
    foreach ( QSharedPointer<Text> tmp_text , m1->labels ) {
        tmp_text->setMolecule( this );
        labels.append( tmp_text );
    }
    Changed();
}

/// Removes (bond,label,symbol) from this molecule's lists, calls Changed(). TODO: redesign class hierarchy.
bool Molecule::Erase( QSharedPointer<Drawable> d )
{
    bool retval = false;

    if ( d->metaObject() == &Bond::staticMetaObject ) {
        QSharedPointer<Bond> tmp_bond = d.objectCast<Bond>();
        if ( bonds.contains( tmp_bond ) ) {
            if ( ( tmp_bond->Order() == 2 ) || ( tmp_bond->Order() == 3 ) ) {
                tmp_bond->setOrder( tmp_bond->Order() - 1 );
                if ( tmp_bond->Dash() > 0 )
                    tmp_bond->setDash( tmp_bond->Dash() - 1 );
                Changed();
                return true;
            } else {
                retval = bonds.removeAll( tmp_bond );
            }
        }
    } else {                    // deleting TEXT or SYMBOL
        if ( d->metaObject() == &Text::staticMetaObject ) {
            retval = labels.removeAll( d.objectCast<Text>() );
        }
        if ( d->metaObject() == &Symbol::staticMetaObject ) {
            retval = symbols.removeAll( d.objectCast<Symbol>() );
        }
        else {
            qWarning() << "Unhandled case in Mol::Erase()!";
        }
    }
    if ( retval == false )
        return false;
    Changed();
    return true;
}

/// Erases all selected (bonds,labels,symbols), calls Changed().
void Molecule::EraseSelected()
{
    QList <QSharedPointer<Bond> >removebonds;
    QList <QSharedPointer<Text> >removetext;
    QList <QSharedPointer<Symbol> >removesym;

    foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
        if ( tmp_bond->Highlighted() == true ) {
            removebonds.append( tmp_bond );
        }
    }
    foreach ( QSharedPointer<Bond> tmp_bond, removebonds ) {
        bonds.removeAll( tmp_bond );
    }
    foreach ( QSharedPointer<Text> tmp_text, labels ) {
        if ( tmp_text->Highlighted() == true ) {
            removetext.append( tmp_text );
        }
    }
    foreach ( QSharedPointer<Text> tmp_text, removetext ) {
        labels.removeAll( tmp_text );
    }
    foreach ( QSharedPointer<Symbol> tmp_sym, symbols ) {
        if ( tmp_sym->Highlighted() == true ) {
            removesym.append( tmp_sym );
        }
    }
    foreach ( QSharedPointer<Symbol> tmp_sym, removesym ) {
        symbols.removeAll( tmp_sym );
    }

    Changed();
}

/// Return string with XML (XDrawChem format) representation of this Molecule
QString Molecule::ToXML( QString xml_id )
{
    QString s( "" );

    QList < DPoint * >up;
    QList < QSharedPointer<Drawable> > uo (AllObjects());
    QString n1, nfull;
    int n = 0;

    up = AllPoints();

    if ( up.count() == 0 )
        return s;               // don't write an empty file

    // Copy text from Text objects to element field in DPoint
    // also copy Symbol descriptions
    foreach ( QSharedPointer<Drawable> tmp_draw, uo ) {
        tmp_draw->Start()->hit = false;
        if ( tmp_draw->metaObject() == &Text::staticMetaObject ) {
            Text *tmp_text = ( Text * ) tmp_draw.data();
            tmp_text->Start()->element = tmp_text->getText();
            tmp_text->Start()->elementmask = tmp_text->getRichText();
            tmp_text->Start()->color = tmp_text->GetColor();
            tmp_text->Start()->font = tmp_text->getFont();
            tmp_text->Start()->hit = true;
        }
        if ( tmp_draw->metaObject() == &Symbol::staticMetaObject ) {
            Symbol *tmp_sym = ( Symbol * ) tmp_draw.data();
            tmp_sym->Start()->symbol = tmp_sym->GetSymbol();
        }
    }

    // begin Molecule
    s.append( "<molecule id=\"" );
    s.append( xml_id );
    s.append( "\">\n" );

    if ( group_type != GROUP_NONE ) {
        s.append( "<grouptype>" );
        if ( group_type == GROUP_REACTANT )
            s.append( "reactant" );
        if ( group_type == GROUP_PRODUCT )
            s.append( "product" );
        s.append( "</grouptype>\n" );
    }
    // Add XML ID's to DPoint's, write as we go
    foreach ( DPoint *tpt, up ) {
        n1.setNum( n );
        nfull = QString( "a" ) + n1;
        tpt->id = nfull;
        n++;
        s.append( "<atom id=\"" );
        s.append( tpt->id );
        s.append( "\">\n" );
        s.append( "<element>" );
        s.append( tpt->element );
        s.append( "</element>\n" );
        s.append( "<elemask>" );
        s.append( tpt->elementmask );
        s.append( "</elemask>\n" );
        if ( tpt->hit )
            s.append( "<textobject>yes</textobject>\n" );
        if ( tpt->symbol.length() > 1 ) {
            s.append( "<symtype>" );
            s.append( tpt->symbol );
            s.append( "</symtype>\n" );
        }
        s.append( "<coordinate2>" );
        n1.setNum( tpt->x );
        s.append( n1 );
        s.append( " " );
        n1.setNum( tpt->y );
        s.append( n1 );
        s.append( "</coordinate2>\n" );

        // write color
        s.append( "<color>" );
        n1.setNum( tpt->color.red() );
        s.append( n1 );
        s.append( " " );
        n1.setNum( tpt->color.green() );
        s.append( n1 );
        s.append( " " );
        n1.setNum( tpt->color.blue() );
        s.append( n1 );
        s.append( "</color>\n" );

        // write font
        s.append( "<font>" );
        s.append( tpt->font.family() );
        s.append( QString( "#" ) );
        n1.setNum( tpt->font.pointSize() );
        s.append( n1 );
        s.append( "</font>\n" );

        s.append( "</atom>\n" );
    }

    // add XML ID's to Bond's, write as we go
    n = 0;
    foreach ( QSharedPointer<Drawable> tmp_draw, uo ) {
        if ( tmp_draw->metaObject() == &Bond::staticMetaObject ) {
            Bond *tmp_bond = ( Bond * ) tmp_draw.data();
            n1.setNum( n );
            nfull = QString( "b" ) + n1;
            tmp_bond->setID( nfull );
            n++;
            s.append( "<bond id=\"" );
            s.append( tmp_bond->getID() );
            s.append( "\">\n<Start>" );
            s.append( tmp_bond->Start()->id );
            s.append( "</Start>\n<End>" );
            s.append( tmp_bond->End()->id );
            s.append( "</End>\n" );
            s.append( "<order>" );
            n1.setNum( tmp_bond->Order() );
            s.append( n1 );
            s.append( "</order>\n" );
            s.append( "<dash>" );
            n1.setNum( tmp_bond->Dash() );
            s.append( n1 );
            s.append( "</dash>\n" );
            // write thickness
            s.append( "<thick>" );
            n1.setNum( tmp_bond->thick );
            s.append( n1 );
            s.append( "</thick>\n" );
            // write color
            s.append( "<color>" );
            n1.setNum( tmp_bond->GetColor().red() );
            s.append( n1 );
            s.append( " " );
            n1.setNum( tmp_bond->GetColor().green() );
            s.append( n1 );
            s.append( " " );
            n1.setNum( tmp_bond->GetColor().blue() );
            s.append( n1 );
            s.append( "</color>\n" );
            s.append( "</bond>\n" );
        }
    }

    s.append( "</molecule>\n" );
    return s;
}

/// Return string with XML (ChemDraw(TM) format) representation of this Molecule
QString Molecule::ToCDXML( QString xml_id )
{
    QString s( "" );

    QList < DPoint * >up;
    QList < QSharedPointer<Drawable> > uo (AllObjects());
    QString n1, nfull;
    int n = xml_id.toInt() + 1;

    up = AllPoints();

    if ( up.count() == 0 )
        return s;               // don't write an empty file

    // also copy Symbol descriptions
    foreach ( QSharedPointer<Drawable> tmp_draw, uo ) {
        if ( tmp_draw->metaObject() == &Symbol::staticMetaObject ) {
            Symbol *tmp_sym = ( Symbol * ) tmp_draw.data();
            tmp_sym->Start()->symbol = tmp_sym->GetSymbol();
        }
    }

    // begin Molecule
    s.append( "<fragment id=\"" );
    s.append( xml_id );
    s.append( "\">\n" );

    // Add XML ID's to DPoint's, write as we go
    foreach ( DPoint *tpt, up ) {
        n1.setNum( n );
        tpt->id = n1;
        n++;
        s.append( "<n id=\"" );
        s.append( tpt->id );
        s.append( "\" p=\"" );
        n1.setNum( tpt->x );
        s.append( n1 );
        s.append( " " );
        n1.setNum( tpt->y );
        s.append( n1 );
        s.append( "\"" );
        if ( tpt->element == "C" ) {
            s.append( "/>\n" );
        } else {
            s.append( "><t><s font=\"21\" size=\"10\" face=\"96\">" );
            s.append( tpt->element );
            s.append( "</s></t></n>\n" );
        }
    }

    // add XML ID's to Bond's, write as we go
    foreach ( QSharedPointer<Drawable> tmp_draw, uo ) {
        if ( tmp_draw->metaObject() == &Bond::staticMetaObject ) {
            Bond *tmp_bond = ( Bond * ) tmp_draw.data();
            n1.setNum( n );
            tmp_bond->setID( n1 );
            n++;
            s.append( "<b id=\"" );
            s.append( tmp_bond->getID() );
            s.append( "\" B=\"" );
            s.append( tmp_bond->Start()->id );
            s.append( "\" E=\"" );
            s.append( tmp_bond->End()->id );
            if ( tmp_bond->Order() < 4 ) {
                s.append( "\" Order=\"" );
                n1.setNum( tmp_bond->Order() );
                s.append( n1 );
            }
            if ( tmp_bond->Order() == 5 ) {
                s.append( "\" Display=\"WedgeBegin" );
            }
            if ( tmp_bond->Order() == 7 ) {
                s.append( "\" Display=\"WedgedHashBegin" );
            }
            s.append( "\"/>\n" );
        }
    }

    s.append( "</fragment>\n" );
    return s;
}

/// Returns string with MDL Molfile to export to generate SMILES, export for 3-D
QString Molecule::ToMDLMolfile( int coords )
{
  QTextDocument localtextdocument;
    QList < DPoint * >up;
//   Q3PtrList<Drawable> uo;
    QString tmpline;

    /// get all unique points and objects
    up = AllPoints();

    QString molfile;

    QTextStream t( &molfile, QIODevice::WriteOnly );

    /// write first three lines -- blank, unless we want to add comments etc.
    t << endl << endl << endl;

    /// find counts...
    int acount = up.count();
    int bcount = bonds.count();
    // write counts line
    t.setFieldWidth( 3 );
    t << acount;
    t.setFieldWidth( 3 );
    t << bcount << "  0  0  0  0  0  0  0  0  1" << endl;

    // Write atom list
    foreach ( DPoint *tpt, up ) {
      localtextdocument.setHtml(tpt->element);
      tmpline = localtextdocument.toPlainText();
      //tmpline = tmp_pt->element;
        if ( tmpline.length() < 3 )
            tmpline.append( ' ' );
        if ( tmpline.length() < 3 )
            tmpline.append( ' ' );
        if ( coords == 0 ) {    // 2D
            t.setFieldWidth( 10 );
            t << tpt->x;
            t.setFieldWidth( 10 );
            t << -tpt->y << "    0.0000 " << tmpline;
        } else {                // 3D
            t.setFieldWidth( 10 );
            t << tpt->x;
            t.setFieldWidth( 10 );
            t << tpt->y;
            t.setFieldWidth( 10 );
            t << tpt->z;
            t << tmpline;
        }
        t << " 0  0  0  0  0  0  0  0  0  0  0  0" << endl;
    }
    // write bond list
    foreach ( QSharedPointer<Bond> tmp_bond, bonds ) {
        t.setFieldWidth( 3 );
        t << up.indexOf( tmp_bond->Start() ) + 1;
        t.setFieldWidth( 3 );
        t << up.indexOf( tmp_bond->End() ) + 1;
        t.setFieldWidth( 3 );
        t << tmp_bond->Order();
        if ( tmp_bond->Order() < 4 )
            t << "  0  0  0  0";
        if ( tmp_bond->Order() == 5 )
            t << "  1  0  0  0";
        if ( tmp_bond->Order() == 7 )
            t << "  6  0  0  0";
	t << endl;
    }

    t << "M  END";

    return molfile;
}

/// Adds points, bonds, labels, symbols from XML to this.
void Molecule::FromXML( QString xml_tag )
{
    QString bondtag, subtag, startid, endid;

    int i1 = xml_tag.indexOf( "<grouptype>" );
    if (i1 > 0){
        subtag = xml_tag.mid( i1, 20 );
        if ( subtag.toLower().contains( "reactant" ) > 0 )
            group_type = GROUP_REACTANT;
        if ( subtag.toLower().contains( "product" ) > 0 )
            group_type = GROUP_PRODUCT;
    }
    // Extract atom's
    QList < DPoint * >points;
    do {
        int i1 = xml_tag.indexOf( "<atom " );
        int i2 = xml_tag.indexOf( "</atom>" ) + 7;
        if ( i1 >= 0 ) {
            DPoint *tpt = new DPoint;
            tpt->FromXML( xml_tag.mid( i1, i2 - i1 ) );
            points.append( tpt );
            xml_tag.remove( i1, i2 - i1 );
        } else {
            break;
        }
    } while ( 1 );
    // Add bond's
    do {
        int i1 = xml_tag.indexOf( "<bond " );
        int i2 = xml_tag.indexOf( "</bond>" ) + 7;
        if ( i1 >= 0 ) {
            QSharedPointer<Bond> tmp_bond ( new Bond());
            bondtag = xml_tag.mid( i1, i2 - i1 );
            xml_tag.remove( i1, i2 - i1 );
            qDebug() << bondtag;
            // parse Start
            i1 = bondtag.indexOf( "<Start>" );
            i2 = bondtag.indexOf( "</Start>" ) + 8;
            subtag = bondtag.mid( i1, i2 - i1 );
            i1 = subtag.indexOf( "<Start>" );
            i2 = subtag.indexOf( "</Start>" );
            subtag.remove( i2, 999 );
            subtag.remove( i1, 7 );
            DPoint *s1 = NULL;
            foreach ( DPoint *tpt, points ) {
                if ( tpt->id == subtag ) {
                    s1 = tpt;
                    break;
                }
            }
            // parse End
            i1 = bondtag.indexOf( "<End>" );
            i2 = bondtag.indexOf( "</End>" ) + 6;
            subtag = bondtag.mid( i1, i2 - i1 );
            i1 = subtag.indexOf( "<End>" );
            i2 = subtag.indexOf( "</End>" );
            subtag.remove( i2, 999 );
            subtag.remove( i1, 5 );
            DPoint *e1 = NULL;
            foreach ( DPoint *tpt, points ) {
                if ( tpt->id == subtag ) {
                    e1 = tpt;
                    break;
                }
            }
            // add points
            tmp_bond->setPoints( s1, e1 );
            // parse Order
            i1 = bondtag.indexOf( "<order>" );
            i2 = bondtag.indexOf( "</order>" ) + 8;
            subtag = bondtag.mid( i1, i2 - i1 );
            i1 = subtag.indexOf( "<order>" );
            i2 = subtag.indexOf( "</order>" );
            subtag.remove( i2, 999 );
            subtag.remove( i1, 7 );
            tmp_bond->setOrder( subtag.toInt() );
            // parse Dash
            i1 = bondtag.indexOf( "<dash>" );
            i2 = bondtag.indexOf( "</dash>" ) + 7;
            subtag = bondtag.mid( i1, i2 - i1 );
            i1 = subtag.indexOf( "<dash>" );
            i2 = subtag.indexOf( "</dash>" );
            subtag.remove( i2, 999 );
            subtag.remove( i1, 6 );
            tmp_bond->setDash( subtag.toInt() );
            // parse thickness
            i1 = bondtag.indexOf( "<thick>" );
            i2 = bondtag.indexOf( "</thick>" ) + 8;
            subtag = bondtag.mid( i1, i2 - i1 );
            i1 = subtag.indexOf( "<thick>" );
            i2 = subtag.indexOf( "</thick>" );
            subtag.remove( i2, 999 );
            subtag.remove( i1, 7 );
            tmp_bond->setThick( subtag.toInt() );
            // parse Dash
            i1 = bondtag.indexOf( "<color>" );
            i2 = bondtag.indexOf( "</color>" ) + 8;
            subtag = bondtag.mid( i1, i2 - i1 );
            tmp_bond->SetColor( GetColorFromXML( subtag ) );
            bonds.append( tmp_bond );
        } else {
            break;
        }
    } while ( 1 );
    // add Text and Symbol as needed
    foreach ( DPoint *tpt, points ) {
        if ( ( tpt->element != QString( "C" ) ) || ( tpt->hit == true ) ) {
            QSharedPointer<Text> tmp_text ( new Text());
            tmp_text->setPoint( tpt );
            tmp_text->SetColor( tpt->color );
            tmp_text->setFont( tpt->font );
            tmp_text->setText( tpt->elementmask );
            if ( tpt->elementmask.length() > 0 ) {
//                tmp_text->setTextMask( tmp_pt->elementmask );
            } else {
                tpt->element.fill( ' ' );
//                tmp_text->setTextMask( tmp_pt->element );
            }
            labels.append( tmp_text );
        }
        if ( tpt->symbol.length() > 0 ) {
            QSharedPointer<Symbol> tmp_sym ( new Symbol());
            tmp_sym->setPoint( tpt );
            tmp_sym->SetSymbol( tpt->symbol );
            symbols.append( tmp_sym );
        }
    }
}

void Molecule::Changed()
{
    changed = true;
}

/// update MW and formula, calls AddHydrogens(), BoundingBox(), sets start, end.
void Molecule::doChanged()
{
    if (!changed)
        return;
    // add hydrogens and correct labels
    //qDebug() << "changed" ;
    AddHydrogens();

    QRect r = BoundingBox();
    start = new DPoint (r.topLeft());
    end = new DPoint (start->x + r.width(), start->y + r.height());
    changed = false;
    return;
}

QDebug operator<<(QDebug dbg, const Molecule &m)
{
    // Drawable part
    dbg.space() << m.highlighted << m.start << m.end << m.color;
    dbg.space() << m.BoundingBox();
    return dbg.space();
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
