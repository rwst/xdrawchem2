#include "drawable.h"
#include "molecule.h"
#include "bond.h"
#include "arrow.h"
#include "curvearrow.h"
#include "bracket.h"
#include "text.h"
#include "symbol.h"
#include "biotools.h"
#include "chemdata.h"
#include "defs.h"

ChemData::ChemData( QObject *parent )
    : QObject( parent )
{
    thick_kludge = -1;
}

void ChemData::drawAll()
{
    // draw all objects in ChemData
    QSharedPointer<Drawable> tmp_draw;
    foreach ( tmp_draw, drawlist ) {
        qDebug() << "drawAll: " << tmp_draw;
        tmp_draw->Render();
    }
}

// update Molecules after move
void ChemData::FinishMove()
{
    QSharedPointer<Drawable> d;

    foreach ( d, drawlist ) {
        if ( d->metaObject() == &Molecule::staticMetaObject ) {
            (( Molecule * ) d.data())->Changed();
        }
    }
}

QSharedPointer<Molecule> ChemData::firstMolecule()
{
    QSharedPointer<Drawable> d;

    foreach ( d, drawlist ) {
        if ( d->metaObject() == &Molecule::staticMetaObject ) {
            return d.objectCast<Molecule>();
        }
    }
    d.clear();
    return d.objectCast<Molecule>();
}

void ChemData::addMolecule( Molecule * m1 )
{
    QSharedPointer<Drawable> p(m1);
    drawlist.append( p );
}

void ChemData::addArrow( DPoint * s, DPoint * e, QColor c, int t, int p2, bool hl )
{
    Arrow *a1 = new Arrow( r );

    a1->setPoints( s, e );
    a1->SetColor( c );
    a1->SetStyle( t );
    a1->setThick( p2 );
    if ( hl )
        a1->Highlight( true );
    QSharedPointer<Drawable> p(a1);
    drawlist.append( p );
}

void ChemData::addCurveArrow( DPoint * s, DPoint * e, QColor c, QString s1, bool hl )
{
    CurveArrow *a1 = new CurveArrow( r );

    a1->setPoints( s, e );
    a1->SetColor( c );
    a1->SetCurve( s1 );
    if ( hl )
        a1->Highlight( true );
    QSharedPointer<Drawable> p(a1);
    drawlist.append( p );
}

void ChemData::addBracket( DPoint * s, DPoint * e, QColor c, int type, bool hl )
{
    Bracket *a1 = new Bracket( r );

    a1->setPoints( s, e );
    a1->SetColor( c );
    a1->SetStyle( type );
    if ( hl )
        a1->Highlight( true );
    QSharedPointer<Drawable> p(a1);
    drawlist.append( p );
}

void ChemData::addText(QSharedPointer<Text> t )
{
    qDebug() << "addText";
    if ( t->Justify() == JUSTIFY_TOPLEFT ) {  // add to drawing
        QSharedPointer<Drawable> p(t);
        drawlist.append( p );
    } else {                    // add label to specific Molecule
        QSharedPointer<Drawable> td;
        foreach ( td, drawlist ) {
            if ( td->metaObject() == &Molecule::staticMetaObject
                 && td->Find( t->Start() ) == true ) {
                ((Molecule*)td.data())->addText( t );
                return;
            }
        }
        qDebug() << "FYI, add text failed";
    }
}

void ChemData::addGraphicObject( GraphicObject * t )
{
    QSharedPointer<Drawable> p(t);
    drawlist.append( p );
}

void ChemData::addBond( DPoint * s, DPoint * e, int thick, int order, QColor c, bool hl )
{
    //qDebug() << "Request to add bond:" ;
    //qDebug() << "(" << s->x << "," << s->y << ")-(" << e->x << "," << e->y << ")";
    //qDebug() ;

    QSharedPointer<Drawable> td;
    QSharedPointer<Molecule> m1, m2;
    foreach ( td, drawlist )
        if ( td->metaObject() == &Molecule::staticMetaObject ) {
            if ( td->Find( s ) == true )
                m1 = td.objectCast<Molecule>();
            if ( td->Find( e ) == true )
                m2 = td.objectCast<Molecule>();
        }

    // neither point exists -- create new Molecule
    if ( ( m1 == 0 ) && ( m2 == 0 ) ) {
        Molecule *m = new Molecule( r );

        m->SetChemdata( this );
        m->addBond( s, e, thick, order, c, hl );
        QSharedPointer<Drawable> d (m);
        drawlist.append( d );
        return;
    }
    // one point exists, or both in same molecule
    if ( ( m1 == 0 ) && ( m2 != 0 ) ) {
        m2->addBond( s, e, thick, order, c, hl );
        return;
    }
    if ( ( ( m1 != 0 ) && ( m2 == 0 ) ) || ( m1 == m2 ) ) {
        m1->addBond( s, e, thick, order, c, hl );
        return;
    }
    // both points exist in different molecules
    if ( m1 != m2 ) {
        m1->addBond( s, e, thick, order, c, hl );
        m1->addMolecule( m2.data() );
        drawlist.removeAll( m2 );
    }
}

void ChemData::addSymbol( DPoint * a, QString symbolfile, bool hl )
{
    QSharedPointer<Symbol> s1 ( new Symbol( r ));
    s1->setPoint( a );
    s1->SetSymbol( symbolfile );
    if ( hl )
        s1->Highlight( true );

    // determine whether point exists or not; if exists, add to Molecule
    QSharedPointer<Drawable> td;
    foreach ( td, drawlist ) {
        if ( ( td->Find( a ) == true )
             && ( td->metaObject() == &Molecule::staticMetaObject )) {
            ((Molecule*)td.data())->addSymbol( s1 );
            return;
        }
    }

    // DANGER! Object may exist twice now!!!
    drawlist.append( s1 );
}

QSharedPointer<Molecule> ChemData::insideMolecule( DPoint * t1 )
{
    //qDebug() << t1->x << "," << t1->y;

    QSharedPointer<Drawable> td;
    foreach ( td, drawlist ) {
        if ( td->metaObject() == &Molecule::staticMetaObject ) {
            //QRect tr1 = m1->BoundingBoxAll();
            //qDebug() << tr1.left() << "," << tr1.top() << ";";
            //qDebug() << tr1.right() << "," << tr1.bottom();
            QSharedPointer<Molecule> m (td.objectCast<Molecule>());
            if (m->BoundingBoxAll().contains( t1->toQPoint(), false ) )
                return m;
        }
    }
    if (td) td.clear();
    return td.objectCast<Molecule>();
}

DPoint *ChemData::FindNearestPoint( DPoint * target, double &dist )
{
    DPoint *nearest = 0, *d1;
    double mindist = 9999.0, d1dist = 999999.0;

    QSharedPointer<Drawable> tmp_draw;
    foreach ( tmp_draw, drawlist ) {
        d1 = tmp_draw->FindNearestPoint( target, d1dist );
        if ( d1dist < mindist ) {
            mindist = d1dist;
            nearest = d1;
        }
    }
    dist = mindist;
    return nearest;
}

QSharedPointer<Drawable> ChemData::FindNearestObject( DPoint * target, double &dist )
{
    double mindist = 2000.0, d1dist = 999999.0;

    QSharedPointer<Drawable> nearest;
    foreach ( QSharedPointer<Drawable> tmp_draw, drawlist ) {
//        if (tmp_draw->metaObject() == &Arrow::staticMetaObject)

        d1dist = tmp_draw.data()->distanceTo( target );
        if ( d1dist < mindist ) {
            mindist = d1dist;
            nearest = tmp_draw;
        }
    }
    dist = mindist;
    return nearest;
}

void ChemData::Erase( QSharedPointer<Drawable> d )
{
    if ( drawlist.removeAll( d ) == false ) {
        qDebug() << "do we get called at all?";
        QSharedPointer<Drawable> td;
        foreach ( td, drawlist ) {
            bool erased = td->Erase( d );
            // collect empty Molecules for removal
            if ( td->Members() == 0 )
                drawlist.removeAll( td );
            qDebug() << "erased:" << erased;
            if ( erased == true )
                break;          //should only be one instance of d to remove!
        }
    } else {                    // drawlist.remove(d) == true
        d.clear();
    }

    // Split Molecules as needed
    DetectSplit();
}

void ChemData::EraseSelected()
{
    QList < QSharedPointer<Drawable> >removelist;
    QSharedPointer<Drawable> tmp_draw;
    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->metaObject() == &Molecule::staticMetaObject ) {
            ((Molecule*)tmp_draw.data())->EraseSelected();
            // collect empty Molecules for removal
            if ( tmp_draw->Members() == 0 )
                removelist.append( tmp_draw );
        } else {
            if ( tmp_draw->Highlighted() == true ) {
                removelist.append( tmp_draw );
            }
        }
    }
    foreach ( tmp_draw, removelist ) {
        /*
           if (tmp_draw->Type() == TYPE_TEXT) {
           Text *tmp_text = (Text *)tmp_draw;
           if (tmp_text->getDataType() == TEXT_DATA_MW) {
           tmp_text->getMolecule()->MWLabelDeleted();
           }
           if (tmp_text->getDataType() == TEXT_DATA_FORMULA) {
           tmp_text->getMolecule()->FormulaLabelDeleted();
           }
           }
         */
        drawlist.removeAll( tmp_draw );
    }
    // Split Molecules as needed
    DetectSplit();
}

// Split Molecule's which hold multiple structures (e.g. after delete)
void ChemData::DetectSplit()
{
    QList<QSharedPointer<Drawable> > removelist;
    QSharedPointer<Drawable> tmp_draw, td2;

    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->metaObject() == &Molecule::staticMetaObject ) {
            QList<QSharedPointer<Molecule> > split_list (((Molecule*)tmp_draw.data())->MakeSplit());
            if ( split_list.count() > 1 ) {
                qDebug() << "Split needed";
                removelist.append( tmp_draw );
                foreach ( td2, split_list ) {
                    drawlist.append( td2 );
                }
                split_list.clear();
            }
        }
    }
    // remove old Molecules
    foreach ( tmp_draw, removelist ) {
        drawlist.removeAll( tmp_draw );
    }
}

void ChemData::SelectAll()
{
    QList < DPoint * >allpts = UniquePoints();

    QSharedPointer<Drawable> tmp_draw;
    foreach ( tmp_draw, drawlist ) {
        tmp_draw->SelectAll();
    }
    foreach ( tmp_pt, allpts ) {
        tmp_pt->setHighlighted( true );
    }
}

void ChemData::DeselectAll()
{
    QList < DPoint * >allpts = UniquePoints();
    QSharedPointer<Drawable> tmp_draw;

    foreach ( tmp_draw, drawlist ) {
        tmp_draw->DeselectAll();
    }
    foreach ( tmp_pt, allpts ) {
        tmp_pt->setHighlighted( false );
    }
}

void ChemData::SetColorIfHighlighted( QColor c )
{
    QSharedPointer<Drawable> tmp_draw;
    foreach ( tmp_draw, drawlist )
        tmp_draw->SetColorIfHighlighted( c );
}

void ChemData::Move( double dx, double dy )
{
    QSharedPointer<Drawable> tmp_draw;
    foreach ( tmp_draw, drawlist )
        tmp_draw->Move( dx, dy );
}

void ChemData::Resize( DPoint * d1, double dy )
{
    QSharedPointer<Drawable> tmp_draw;
    foreach ( tmp_draw, drawlist )
        tmp_draw->Resize( d1, dy );
}

void ChemData::Rotate( DPoint * d1, double dy )
{
    QSharedPointer<Drawable> tmp_draw;
    foreach ( tmp_draw, drawlist )
        tmp_draw->Rotate( d1, dy );
}

void ChemData::Flip( DPoint * d1, int dy )
{
    QSharedPointer<Drawable> tmp_draw;
    foreach ( tmp_draw, drawlist )
        tmp_draw->Flip( d1, dy );
}

// Find minimum rectangle needed to enclose selection
QRect ChemData::selectionBox()
{
    int top = 99999, bottom = 0, left = 99999, right = 0;
    QRect tmprect;
    QSharedPointer<Drawable> tmp_draw;

    foreach ( tmp_draw, drawlist ) {
        tmprect = tmp_draw->BoundingBox();
        qDebug() << tmprect.width() << "X" << tmprect.height();
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

    left -= 3;
    right += 5;
    top -= 3;
    bottom += 3;
    if ( left < 0 )
        left = 0;
    if ( top < 0 )
        top = 0;
    return QRect( QPoint( left, top ), QPoint( right, bottom ) );
}

// when doing multiple selection via MODE_SELECT_MULTIPLE, we will
// have to highlight/unhighlight regions of the drawing as the selection
// box changes.  This function is called to start checking whether objects
// fall within the select box.
void ChemData::NewSelectRect( QRect n, bool shiftdown )
{
    QList < DPoint * >allpts = UniquePoints();

    foreach ( tmp_pt, allpts ) {
        if ( n.contains( tmp_pt->toQPoint() ) == true ) {
            tmp_pt->setHighlighted( true );
        } else {
            tmp_pt->setHighlighted( false );
        }
    }

    QSharedPointer<Drawable> tmp_draw;
    foreach ( tmp_draw, drawlist ) {
        tmp_draw->isWithinRect( n, shiftdown );
    }
}

// Get list of unique points contained in all Molecules.
QList < DPoint * >ChemData::UniquePoints()
{
    QList < DPoint * >up, tp;

    QSharedPointer<Drawable> tmp_draw;
    foreach ( tmp_draw, drawlist ) {
        tp = tmp_draw->AllPoints();
        foreach ( tmp_pt, tp )
            up.append( tmp_pt );
    }

    qDebug() << up.count();
    return up;
}

QList < QSharedPointer <Drawable> > ChemData::UniqueObjects()
// TODO: return all Molecule objects or these plus all other Drawables?
{
    QList < QSharedPointer <Drawable> > uo;
    QSharedPointer <Drawable> td1;

    foreach ( td1, drawlist ) {
        if (td1->metaObject() == &Molecule::staticMetaObject) {
            QList < QSharedPointer <Drawable> > to (((Molecule*)td1.data())->AllObjects());
            QSharedPointer <Drawable> td2;
            foreach ( td2, to )
                uo.append( td2 );
        }
    }

    qDebug() << uo.count();
    return uo;
}

//cmake#include "chemdata.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
