// drawable.cpp - DrawProp's implementation of functions

#include "drawable.h"
#include "dpoint.h"
#include "defs.h"


double Drawable::DistanceBetween( QPoint a, QPoint b )
{
    double dx = a.x() - b.x();
    double dy = a.y() - b.y();

    return sqrt( dx * dx + dy * dy );
}

// convert XML <color> tag to QColor and set current
void Drawable::SetColorFromXML( QString xml_tag )
{
    qDebug() << "SetColorFromXML:" << xml_tag;
    int i1, i2;
    int d1, d2, d3;

    i1 = xml_tag.indexOf( "<color>" );
    i2 = xml_tag.indexOf( "</color>" );
    xml_tag.remove( i2, 999 );
    xml_tag.remove( i1, 7 );

    QTextStream ts( &xml_tag, QIODevice::ReadOnly );

    ts >> d1 >> d2 >> d3;

    color = QColor( d1, d2, d3 );
}

// convert XML <color> tag to QColor and return QColor
QColor Drawable::GetColorFromXML( QString xml_tag )
{
    qDebug() << "GetColorFromXML:" << xml_tag;
    QColor ncolor;
    int i1, i2;
    int d1, d2, d3;

    i1 = xml_tag.indexOf( "<color>" );
    i2 = xml_tag.indexOf( "</color>" );
    xml_tag.remove( i2, 999 );
    xml_tag.remove( i1, 7 );

    QTextStream ts( &xml_tag, QIODevice::ReadOnly );

    ts >> d1 >> d2 >> d3;

    ncolor = QColor( d1, d2, d3 );
    return ncolor;
}

// set DPoint *start from XML
DPoint *Drawable::StartFromXML( QString xml_tag )
{
    qDebug() << "SetStartFromXML:" << xml_tag;
    int i1, i2;
    double x1, y1;

    i1 = xml_tag.indexOf( "<Start>" );
    i2 = xml_tag.indexOf( "</Start>" );
    xml_tag.remove( i2, 999 );
    xml_tag.remove( i1, 7 );

    QTextStream ts( &xml_tag, QIODevice::ReadOnly );

    ts >> x1 >> y1;

    return new DPoint( x1, y1 );      // necessary
}

// get DPoint *end from XML
DPoint *Drawable::EndFromXML( QString xml_tag )
{
    qDebug() << "SetEndFromXML:" << xml_tag;
    int i1, i2;
    double x1, y1;

    i1 = xml_tag.indexOf( "<End>" );
    i2 = xml_tag.indexOf( "</End>" );
    xml_tag.remove( i2, 999 );
    xml_tag.remove( i1, 5 );

    QTextStream ts( &xml_tag, QIODevice::ReadOnly );

    ts >> x1 >> y1;

    return new DPoint( x1, y1 );      // necessary
}

void Drawable::Highlight()
{
    if ( start->isHighlighted() == true ) {
        highlighted = true;
    } else {
        highlighted = false;
    }
}

void Drawable::Highlight( bool hl )
{
    highlighted = hl;
    if ( hl == true ) {
        start->setHighlighted( true );
    } else {
    }
}


//
// end common functions
// ********************

// math functions all DrawProps might need
double Drawable::getAngle( DPoint * a, DPoint * b )
{
    double dx = b->x - a->x;
    double dy = b->y - a->y;
    double ang = atan( dy / dx ) * ( 180.0 / M_PI );

    if ( ang < 0.0 )
        ang *= -1.0;

    if ( b->x > a->x && b->y > a->y )
        ang = 0.0 + ang;
    if ( b->x < a->x && b->y > a->y )
        ang = 180.0 - ang;
    if ( b->x < a->x && b->y < a->y )
        ang = 180.0 + ang;
    if ( b->x > a->x && b->y < a->y )
        ang = 360.0 - ang;
    if ( dx == 0.0 ) {
        if ( dy < 0.0 )
            ang = 270.0;
        else
            ang = 90.0;
    }
    if ( dy == 0.0 ) {
        if ( dx < 0.0 )
            ang = 180.0;
        else
            ang = 0.0;
    }

    return ang;
}

// start->end is the line; target is the point we're measuring to
double Drawable::DistanceToLine( DPoint * start1, DPoint * end1, DPoint * target )
{
    // check if close enough to even worry
#if 0
    double xmin, xmax, ymin, ymax;

    if ( start1->x > end1->x ) {
        xmax = start1->x;
        xmin = end1->x;
    } else {
        xmin = start1->x;
        xmax = end1->x;
    }
    if ( start1->y > end1->y ) {
        ymax = start1->y;
        ymin = end1->y;
    } else {
        ymin = start1->y;
        ymax = end1->y;
    }
    xmin -= 6.0;
    ymin -= 6.0;
    xmax += 6.0;
    ymax += 6.0;
    if ( ( target->x < xmin ) || ( target->x > xmax ) || ( target->y < ymin ) || ( target->y > ymax ) )
        return 999999.0;
    // find distance if target is inside "bounding box"
    double x1 = start1->x;
    double y1 = start1->y;
    double x2 = end1->x;
    double y2 = end1->y;
    double x3 = target->x;
    double y3 = target->y;
    double unum = ( x3 - x1 ) * ( x2 - x1 ) + ( y3 - y1 ) * ( y2 - y1 );
    double dx = x2 - x1;
    double dy = y2 - y1;
    double rt = ( double ) sqrt( dx * dx + dy * dy );
    double uden = rt * rt;
    double u = unum / uden;
    double xp = x1 + u * ( x2 - x1 );
    double yp = y1 + u * ( y2 - y1 );

    dx = xp - x3;
    dy = yp - y3;
    return ( double ) sqrt( dx * dx + dy * dy );
#else
    double x1 = start1->x;
    double y1 = start1->y;
    double x2 = end1->x;
    double y2 = end1->y;
    double x3 = target->x;
    double y3 = target->y;
    double dx = x2 - x1;
    double dy = y2 - y1;
    double rt = ( double ) sqrt( dx * dx + dy * dy );

    if ( rt <= 0.0 ) {
        dx = x3 - x1;
        dy = y3 - y1;
        return sqrt( dx * dx + dy * dy );
    }
    if ( dy * ( y3 - y1 ) + dx * ( x3 - x1 ) < 0.0 ) {
        dx = x3 - x1;
        dy = y3 - y1;
        return sqrt( dx * dx + dy * dy );
    }
    if ( dy * ( y3 - y2 ) + dx * ( x3 - x2 ) > 0.0 ) {
        dx = x3 - x2;
        dy = y3 - y2;
        return sqrt( dx * dx + dy * dy );
    }
    double unum = dx * ( y3 - y1 ) - dy * ( x3 - x1 );

    if ( unum < 0.0 )
        unum = -unum;
    return unum / rt;
#endif
}

// DPointInRect: companion to WithinRect() in subclasses.
bool Drawable::DPointInRect( DPoint * d, QRect r )
{
    return r.contains( QPoint( ( int ) d->x, ( int ) d->y ), true );
}

//cmake#include "drawable.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
