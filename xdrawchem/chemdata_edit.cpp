/*********************************************************************
 * chemdata_edit.cpp
 *
 * Copyright (C)
 * 2004, 2005 Bryan Herger -- bherger@users.sourceforge.net
 * 2012 Ralf Stephan -- ralf@ark.in-berlin.de
 *
 * LICENSE: GPL v. 2 see GPL.txt
 *********************************************************************/

#include "drawable.h"
#include "molecule.h"
#include "bond.h"
#include "arrow.h"
#include "bracket.h"
#include "curvearrow.h"
#include "symbol.h"
#include "text.h"
#include "chemdata.h"
#include "clipboard.h"
#include "defs.h"

/// Calls Scale() for all molecules in drawlist
void ChemData::ScaleAll( double bond_length )
{
    QSharedPointer<Drawable> tmp_draw;
    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->metaObject() == &Molecule::staticMetaObject ) {
            (( Molecule * ) tmp_draw.data())->Scale( bond_length );
        }
    }
}

/// Copy to clipboard and erase selected
void ChemData::Cut()
{
    Copy();
    EraseSelected();
}

/// Clear clipboard, create (copy of) every molecule in drawlist, append to clipboard
void ChemData::Copy()
{
    Clipboard *clip = Clipboard::get();
    clip->clear();

    QSharedPointer<Drawable> tmp_draw;
    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->metaObject() == &Molecule::staticMetaObject ) {
            Molecule *mp =(Molecule*)tmp_draw.data();

            // Copy to clipboard if selected
            if ( mp->Highlighted() == true ) {
                QSharedPointer<Molecule> d (new Molecule (mp));
                clip->objects.append( d );
            }
        } else {
            qWarning() << "Object other than Molecule in ChemData::Copy()!";
            /*
            QList < Drawable * >unique;
            QList < DPoint * >oldPoints, newPoints;
            Drawable *td2, *tdx;
            DPoint *n;
            // Get list of objects, then copy to clipboard if selected
            unique = tmp_draw->AllObjects();
            oldPoints = tmp_draw->AllPoints();
            // make new DPoint's which correspond to old DPoint's
            newPoints.clear();
            foreach ( tmp_pt, oldPoints ) {
                n = new DPoint( tmp_pt );
                newPoints.append( n );
            }
            qDebug() << "unique:" << unique.count();
            qDebug() << "newPoints:" << newPoints.count();
            foreach ( td2, unique ) {
                if ( td2->Highlighted() == true ) {
                    tdx = td2;
                    tdx->setStart( newPoints.at( oldPoints.indexOf( td2->Start() ) ) );
                    if ( td2->End() != 0 )
                        tdx->setEnd( newPoints.at( oldPoints.indexOf( td2->End() ) ) );
                    clip->objects.append( tdx );
                }
            }
            */
        }
    }
}

/// Deselect all, create (copy of) each molecule on clipboard, add it to self
bool ChemData::Paste()
{
    Clipboard *clip = Clipboard::get();

    DeselectAll();

    foreach ( QSharedPointer<Drawable> td, clip->objects ) {
        if (td->metaObject() == &Molecule::staticMetaObject) {
            QSharedPointer<Molecule> m (new Molecule ((Molecule*)td.data()));
            m->Move(30,30);
            FinishMove();
            addMolecule(m);
        }
        else
            {
            qWarning() << "Unhandled clip->object in ChemData::Paste()" << td->metaObject()->className();
        }
    }
    return true;
}

/*
    QList < DPoint * >oldPoints;
    QList < DPoint * >newPoints;

    // need to deep copy stuff coming off the Clipboard
    // first, find all unique DPoint's

    foreach ( QSharedPointer<Drawable> tmp_draw, clip->objects ) {
            if ( oldPoints.contains( tmp_draw->Start() ) == 0 )
                oldPoints.append( tmp_draw->Start() );
            if ( tmp_draw->End() != 0 ) {
                if ( oldPoints.contains( tmp_draw->End() ) == 0 )
                    oldPoints.append( tmp_draw->End() );
            }
    }
    qDebug() << "ChemData::Paste()";
    qDebug() << oldPoints.count();
    if ( oldPoints.count() == 0 )
        return false;

    // make new DPoint's which correspond to old DPoint's
    foreach ( tmp_pt, oldPoints ) {
        DPoint *n = new DPoint( tmp_pt );
        newPoints.append( n );
    }
    // now add all non-TYPE_TEXT objects back to current
    foreach ( QSharedPointer<Drawable> td1, clip->objects ) {
        if (td1->metaObject() == &Arrow::staticMetaObject) {
            addArrow( newPoints.at( oldPoints.indexOf( td1->Start() ) ), newPoints.at( oldPoints.indexOf( td1->End() ) ), td1->GetColor(), ((Arrow*)td1.data())->Style(), true );
        }
        else if (td1->metaObject() == &Bracket::staticMetaObject) {
            addBracket( newPoints.at( oldPoints.indexOf( td1->Start() ) ), newPoints.at( oldPoints.indexOf( td1->End() ) ), td1->GetColor(), ((Bracket*)td1.data())->Style(), true );
        }
        else if (td1->metaObject() == &Bond::staticMetaObject) {
            addBond( newPoints.at( oldPoints.indexOf( td1->Start() ) ), newPoints.at( oldPoints.indexOf( td1->End() ) ), ((Bond*)td1.data())->Thick(), ((Bond*)td1.data())->Order(), td1->GetColor(), true );
        }
        else if (td1->metaObject() == &CurveArrow::staticMetaObject) {
            addCurveArrow( newPoints.at( oldPoints.indexOf( td1->Start() ) ), newPoints.at( oldPoints.indexOf( td1->End() ) ), td1->GetColor(), ((CurveArrow*)td1.data())->GetCurve(), true );
        }
        else if (td1->metaObject() == &Symbol::staticMetaObject) {
            addSymbol( newPoints.at( oldPoints.indexOf( td1->Start() ) ), ((Symbol*)td1.data())->GetSymbol(), true );
        }
        else if (td1->metaObject() == &Text::staticMetaObject) {
            Text *ot = ((Text *)td1.data());
            QSharedPointer<Text> t ( new Text(r) );
            t->setPoint( newPoints.at( oldPoints.indexOf( td1->Start() ) ) );
            t->setText( ot->getText() );
//            t->setTextMask( ot->getTextMask() );
            t->SetColor( ot->GetColor() );
            t->setFont( ot->getFont() );
            t->setJustify( ot->Justify() );
            t->Highlight( true );
            addText( t );
        }
        else {
            qDebug() << "Unhandled case in ChemData::Paste()!";
            qDebug() << td1->metaObject()->className();
            exit(1);
        }
    }
    */

/// Calls save_native() and, if state unique, appends it on undo stack.
void ChemData::StartUndo( int /*fn*/, DPoint * /*s1*/ )
{
    QString last_undo_file;

    //qDebug() << "Saved undo file" ;
    // checkpoint!
    save_native( "" );
    //qDebug() << "File length: " << current_undo_file.length() ;
    if ( last_states.count() > 0 ) {
        last_undo_file = last_states[last_states.count() - 1];
        if ( last_undo_file == current_undo_file )
            return;
    }
    last_states.append( current_undo_file );
    if ( last_states.count() > UNDO_BUFFER_SIZE )
        last_states.removeFirst();
}

/// If undo stack contains state, pull it off and load_native() it.
bool ChemData::Undo()
{
    if ( last_states.count() == 0 ) {
        return false;
    }
    current_undo_file = last_states[last_states.count() - 1];
    last_states.removeLast();
    return load_native( current_undo_file );
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
