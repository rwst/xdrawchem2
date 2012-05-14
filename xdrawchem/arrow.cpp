// arrow.cpp - Arrow's implementation of functions

#include "drawable.h"
#include "arrow.h"
#include "bondedit.h"
#include "defs.h"
#include "render2d.h"

Arrow::Arrow()
{
    highlighted = false;
    style = ARROW_REGULAR;
    thick = 1;
}

QString Arrow::ToXML( QString xml_id )
{
    QString s, n1;

    // begin arrow
    s.append( "<arrow id=\"" );
    s.append( xml_id );
    s.append( "\">\n" );

    // write Start
    s.append( "<Start>" );
    n1.setNum( start->x );
    s.append( n1 );
    s.append( " " );
    n1.setNum( start->y );
    s.append( n1 );
    s.append( "</Start>\n" );

    // write End
    s.append( "<End>" );
    n1.setNum( end->x );
    s.append( n1 );
    s.append( " " );
    n1.setNum( end->y );
    s.append( n1 );
    s.append( "</End>\n" );

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

    // write style
    s.append( "<style>" );
    n1.setNum( style );
    s.append( n1 );
    s.append( "</style>\n" );

    // write thickness
    s.append( "<thick>" );
    n1.setNum( thick );
    s.append( n1 );
    s.append( "</thick>\n" );

    // end arrow
    s.append( "</arrow>\n" );

    return s;
}

QString Arrow::ToCDXML( QString xml_id )
{
    QString s, n1;

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
    if ( style == ARROW_REGULAR )
        s.append( "GraphicType=\"Line\" ArrowType=\"FullHead\" HeadSize=\"1000\"" );
    if ( style == ARROW_DASH )
        s.append( "GraphicType=\"Line\" LineType=\"Dashed\" ArrowType=\"FullHead\" HeadSize=\"1000\"" );
    if ( style == ARROW_BI1 )
        s.append( "GraphicType=\"Line\" ArrowType=\"Resonance\" HeadSize=\"1000\"" );
    if ( style == ARROW_BI2 )
        s.append( "GraphicType=\"Line\" ArrowType=\"Equilibrium\" HeadSize=\"1000\"" );
    if ( style == ARROW_RETRO )
        s.append( "GraphicType=\"Line\" ArrowType=\"RetroSynthetic\" HeadSize=\"1000\"" );
    s.append( "/>\n" );

    return s;
}

// set Arrow from XDrawChem-format XML
void Arrow::FromXML( QString xml_tag )
{
    qDebug() << "read arrow";
    int i1, i2;

    i1 = xml_tag.indexOf( "<Start>" );
    i2 = xml_tag.indexOf( "</Start>" ) + 8;
    start = StartFromXML( xml_tag.mid( i1, i2 - i1 ) );
    i1 = xml_tag.indexOf( "<End>" );
    i2 = xml_tag.indexOf( "</End>" ) + 6;
    end = EndFromXML( xml_tag.mid( i1, i2 - i1 ) );
    i1 = xml_tag.indexOf( "<style>" );
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</style>" ) + 8;
        qDebug() << "arrow::style = |" << xml_tag.mid( i1 + 7, i2 - i1 - 15 )
            << "|";
        style = xml_tag.mid( i1 + 7, i2 - i1 - 15 ).toInt();
    }
    i1 = xml_tag.indexOf( "<thick>" );
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</thick>" ) + 8;
        thick = xml_tag.mid( i1 + 7, 1 ).toInt();
    }
    i1 = xml_tag.indexOf( "<color>" );
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</color>" ) + 8;
        SetColorFromXML( xml_tag.mid( i1, i2 - i1 ) );
    }
}

void Arrow::Render( Render2D* r )
{
    QColor c1;

    if ( highlighted )
        c1 = QColor( 255, 0, 0 );
    else
        c1 = color;
    r->drawArrow( start->toQPoint(), end->toQPoint(), c1, style, thick );
}

void Arrow::Edit( Render2D* r )
{
    qDebug() << "edit arrow";
    BondEditDialog be( r, start, end, PreviewWidget::ARROW, 0, 0, thick, style, color );

    if ( !be.exec() )
        return;
    qDebug() << "change";
    style = be.Style();
    color = be.Color();
    thick = be.Thick();
}

void Arrow::Move (double dx, double dy)
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

void Arrow::Flip( DPoint *origin, int direction )
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

void Arrow::Rotate( DPoint *origin, double angle )
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

void Arrow::Resize( DPoint *origin, double scale )
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

int Arrow::Orientation()
{
    double a1 = getAngle( start, end );

    if ( a1 < 45.0 )
        return ARROW_HORIZONTAL;
    if ( a1 < 135.0 )
        return ARROW_VERTICAL;
    if ( a1 < 225.0 )
        return ARROW_HORIZONTAL;
    if ( a1 < 315.0 )
        return ARROW_VERTICAL;
    return ARROW_HORIZONTAL;
}

QPoint Arrow::Midpoint()
{
    QPoint a;

    a.setX( qRound( ( start->x + end->x ) / 2.0 ) );
    a.setY( qRound( ( start->y + end->y ) / 2.0 ) );
    return a;
}

bool Arrow::Find( DPoint * target )
{
    if ( start == target )
        return true;
    if ( end == target )
        return true;
    return false;
}

// Do not allow connections to this object.
// Simplest way to do this, I think, is to disallow this function
DPoint *Arrow::FindNearestPoint( DPoint */*target*/, double &dist )
{
    dist = 99999.0;
    return 0;
}

double Arrow::distanceTo( DPoint * target)
{
    return DistanceToLine( start, end, target );
}

void Arrow::setPoints( DPoint * s, DPoint * e )
{
    start = s;
    end = e;
}

const QRect Arrow::BoundingBox() const
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

bool Arrow::isWithinRect( QRect n, bool /*shiftdown*/ )
{
    if ( DPointInRect( start, n ) && DPointInRect( end, n ) )
        highlighted = true;
    else
        highlighted = false;
    return highlighted;
}

//cmake#include "arrow.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
