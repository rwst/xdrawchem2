#ifndef MOLPART_H
#define MOLPART_H

/*********************************************************************
 * molpart.h
 *
 * Copyright (C)
 * 2012 Ralf Stephan -- ralf@ark.in-berlin.de
 *
 * LICENSE: GPL v. 2 see GPL.txt
 *********************************************************************/
#include <QRect>

class MolPart
{
public:
    virtual bool Highlighted() = 0;
    virtual bool isWithinRect( QRect, bool ) = 0;
    virtual void SelectAll() = 0;
    virtual void DeselectAll() = 0;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
