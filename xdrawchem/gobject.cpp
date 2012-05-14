// gobject.cpp - GraphicObject's implementation of functions

#include <QRect>
#include <QString>
#include <QPolygon>
#include <QPoint>

#include "render2d.h"
#include "drawable.h"
#include "gobject.h"
#include "bondedit.h"
#include "defs.h"

GraphicObject::GraphicObject()
{
    highlighted = false;
    style = 0;
    ot = 0;
}

QString GraphicObject::ToXML( QString xml_id )
{
    QString s, n1;

    // begin graphic object
    s.append( "<graphicobject id=\"" );
    s.append( xml_id );
    s.append( "\">\n" );

    // write object type (ot)
    s.append( "<objecttype>" );
    n1.setNum( ot );
    s.append( n1 );
    s.append( "</objecttype>\n" );

    // write Style
    s.append( "<style>" );
    n1.setNum( style );
    s.append( n1 );
    s.append( "</style>\n" );

    // write points
    if ( ot == TYPE_BEZIER ) {
        int ci, xi, yi;

        for ( ci = 0; ci < 4; ci++ ) {
            s.append( "<point>" );
            objectPoints.point( ci, &xi, &yi );
            n1.setNum( xi );
            s.append( n1 );
            s.append( " " );
            n1.setNum( yi );
            s.append( n1 );
            s.append( "</point>\n" );
        }
    }
    // write color
    s.append( "<color>" );
    n1.setNum( color.red() );
    s.append( n1 );
    s.append( " " );
    n1.setNum( color.green() );
    s.append( n1 );
    s.append( " " );
    n1.setNum( color.blue() );
    s.append( n1 );
    s.append( "</color>\n" );

    // end bracket
    s.append( "</graphicobject>\n" );

    return s;
}

QString GraphicObject::ToCDXML( QString xml_id )
{
    QString s, n1;

    return s;                   // until I learn how to do these objects in CDXML

    // begin arrow
    s.append( "<graphic id=\"" );
    s.append( xml_id );
    s.append( "\" BoundingBox=\"" );
    n1.setNum( end->x );
    s.append( n1 );
    s.append( " " );
    n1.setNum( end->y );
    s.append( n1 );
    s.append( " " );
    n1.setNum( start->x );
    s.append( n1 );
    s.append( " " );
    n1.setNum( start->y );
    s.append( n1 );
    s.append( "\" " );
    if ( style == BRACKET_SQUARE )
        s.append( "GraphicType=\"Bracket\" BracketType=\"SquarePair\"" );
    if ( style == BRACKET_CURVE )
        s.append( "GraphicType=\"Bracket\" BracketType=\"RoundPair\"" );
    if ( style == BRACKET_BRACE )
        s.append( "GraphicType=\"Bracket\" BracketType=\"CurlyPair\"" );
    s.append( "/>\n" );

    return s;
}

// set GraphicObject from XDrawChem-format XML
void GraphicObject::FromXML( QString xml_tag )
{
    int i1, i2;

    i1 = xml_tag.indexOf( "<Start>" );
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</Start>" ) + 8;
        start = StartFromXML( xml_tag.mid( i1, i2 - i1 ) );
    }
    i1 = xml_tag.indexOf( "<End>" );
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</End>" ) + 6;
        end = EndFromXML( xml_tag.mid( i1, i2 - i1 ) );
    }
    i1 = xml_tag.indexOf( "<objecttype>" );
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</objecttype>" ) + 13;
        ot = xml_tag.mid( i1 + 12, 2 ).toInt();
    }
    i1 = xml_tag.indexOf( "<style>" );
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</style>" ) + 8;
        style = xml_tag.mid( i1 + 7, i2 - i1 - 15 ).toInt();
    }
    i1 = xml_tag.indexOf( "<color>" );
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</color>" ) + 8;
        SetColorFromXML( xml_tag.mid( i1, i2 - i1 ) );
    }
    if ( ot == TYPE_BEZIER ) {
        objectPoints.resize( 4 );
        int ci, xi, yi, i3 = 0;
        QString xml_subtag;

        for ( ci = 0; ci < 4; ci++ ) {
            i1 = xml_tag.indexOf( "<point>", i3 );
            i2 = xml_tag.indexOf( "</point>", i3 ) + 8;
            i3 = i2 + 1;
            xml_subtag = xml_tag.mid( i1, i2 - i1 );
            i1 = xml_subtag.indexOf( "<point>" );
            i2 = xml_subtag.indexOf( "</point>" );
            xml_subtag.remove( i2, 999 );
            xml_subtag.remove( i1, 7 );

            QTextStream ts( &xml_subtag, QIODevice::ReadOnly );

            ts >> xi >> yi;
            qDebug() << ci << " " << xi << " " << yi;
            objectPoints.setPoint( ci, xi, yi );
        }
        start = new DPoint( objectPoints.point( 0 ) );
        end = new DPoint( objectPoints.point( 1 ) );
    }
}

void GraphicObject::Render( Render2D *r )
{
    QColor c1;

    if ( highlighted )
        c1 = QColor( 255, 0, 0 );
    else
        c1 = color;
    if ( ot == TYPE_BEZIER )
        r->drawBezier( objectPoints, c1, false, style );
    if ( ot == TYPE_GRAPHIC_LINE )
        r->drawLine( start->toQPoint(), end->toQPoint(), 1, GetColor(), 1 );
    if ( ot == TYPE_GRAPHIC_BONDMARK )
        r->drawLine( start->toQPoint(), end->toQPoint(), 1, GetColor(), 1 );
}

void GraphicObject::Edit(Render2D *r)
{
    return;                     // it's not practical to edit any graphic objects this way
    /*
       BondEditDialog be(r, "bracket editor", start, end, TYPE_BRACKET, 0, 0, 0,
       style, color);
       if ( !be.exec() ) return;
       qDebug() << "change" ;
       style = be.Style();
       color = be.Color();
     */
}

void GraphicObject::Move (double dx, double dy)
{
    if ( ( highlighted ) && ( start != 0 ) ) {
        start->x += dx;
        start->y += dy;
        }
    if ( ( highlighted ) && ( end != 0 ) ) {
        end->x += dx;
        end->y += dy;
        }
}

void GraphicObject::Flip( DPoint *origin, int direction )
{
    double delta;

    if ( highlighted == false )
        return;
    if ( start != 0 ) {
        if ( direction == FLIP_H ) {
            delta = start->x - origin->x;
            start->x = start->x - 2 * delta;
        } else {                // direction == FLIP_V
            delta = start->y - origin->y;
            start->y = start->y - 2 * delta;
        }
    }
    if ( end != 0 ) {
        if ( direction == FLIP_H ) {
            delta = end->x - origin->x;
            end->x = end->x - 2 * delta;
        } else {                // direction == FLIP_V
            delta = end->y - origin->y;
            end->y = end->y - 2 * delta;
        }
    }
}

void GraphicObject::Rotate( DPoint *origin, double angle )
{
    //double dx, dy;

    if ( highlighted == false )
        return;
    if ( start != 0 ) {
        double thisx = start->x - origin->x;
        double thisy = start->y - origin->y;
        double newx = thisx * cos( angle ) + thisy * sin( angle );
        double newy = -thisx * sin( angle ) + thisy * cos( angle );

        start->x = ( newx + origin->x );
        start->y = ( newy + origin->y );
    }
    if ( end != 0 ) {
        double thisx = end->x - origin->x;
        double thisy = end->y - origin->y;
        double newx = thisx * cos( angle ) + thisy * sin( angle );
        double newy = -thisx * sin( angle ) + thisy * cos( angle );

        end->x = ( newx + origin->x );
        end->y = ( newy + origin->y );
    }
}

void GraphicObject::Resize( DPoint *origin, double scale )
{
    double dx, dy;

    if ( highlighted == false )
        return;
    if ( start != 0 ) {
        dx = start->x - origin->x;
        dy = start->y - origin->y;
        dx *= scale;
        dy *= scale;
        start->x = origin->x + dx;
        start->y = origin->y + dy;
    }
    if ( end != 0 ) {
        dx = end->x - origin->x;
        dy = end->y - origin->y;
        dx *= scale;
        dy *= scale;
        end->x = origin->x + dx;
        end->y = origin->y + dy;
    }
}

bool GraphicObject::Find( DPoint *target )
{
    if ( start == target )
        return true;
    if ( end == target )
        return true;
    return false;
}

// Do not allow connections to this object.
// Simplest way to do this, I think, is to disallow this function
DPoint *GraphicObject::FindNearestPoint( DPoint * /*target*/, double &dist )
{
    dist = 99999.0;
    return 0;
}

double GraphicObject::distanceTo ( DPoint * target )
{
    DPoint tl, tr, bl, br;
    double tl_x, tl_y, br_x, br_y, swp, dist1, dist2;

    tl_x = start->x;
    tl_y = start->y;
    br_x = end->x;
    br_y = end->y;
    if ( tl_x < br_x ) {
        swp = tl_x;
        tl_x = br_x;
        br_x = swp;
    }
    if ( tl_y < br_y ) {
        swp = tl_y;
        tl_y = br_y;
        br_y = swp;
    }
    tl.x = tl_x;
    tl.y = tl_y;
    bl.x = tl_x;
    bl.y = br_y;
    tr.x = br_x;
    tr.y = tl_y;
    br.x = br_x;
    br.y = br_y;
    dist1 = DistanceToLine( &tl, &bl, target );
    dist2 = DistanceToLine( &tr, &br, target );
    if ( dist1 < dist2 )
        return dist1;
    else
        return dist2;
}

void GraphicObject::setPoints( DPoint *s, DPoint *e )
{
    start = s;
    end = e;
}

bool GraphicObject::isWithinRect( QRect n, bool /*shiftdown*/ )
{
    if ( DPointInRect( start, n ) && DPointInRect( end, n ) )
        highlighted = true;
    else
        highlighted = false;
    return highlighted;
}

const QRect GraphicObject::BoundingBox() const
{
    if ( highlighted == false )
        return QRect( QPoint( 999, 999 ), QPoint( 0, 0 ) );
    int top, bottom, left, right, swp;

    top = ( int ) start->y;
    left = ( int ) start->x;
    bottom = ( int ) end->y;
    right = ( int ) end->x;
    if ( bottom < top ) {
        swp = top;
        top = bottom;
        bottom = swp;
    }
    if ( right < left ) {
        swp = left;
        left = right;
        right = swp;
    }
    return QRect( QPoint( left, top ), QPoint( right, bottom ) );
}

void GraphicObject::setPointArray( QPolygon inp1 )
{
    objectPoints = inp1;
    // kludge around lack opf Start and End in this object type/style
    start = new DPoint( objectPoints.point( 0 ) );
    end = new DPoint( objectPoints.point( 1 ) );
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
