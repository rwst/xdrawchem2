/*********************************************************************
 * clipboard.h
 *
 * Copyright (C)
 * 2004, 2005 Bryan Herger -- bherger@users.sourceforge.net
 * 2012 Ralf Stephan -- ralf@ark.in-berlin.de
 *
 * LICENSE: GPL v. 2 see GPL.txt
 *********************************************************************/

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QList>

class Drawable;

class Clipboard
{
public:
    QList<QSharedPointer<Drawable> > objects;
    void clear() { objects.clear(); }
    static Clipboard *get() {
        static Clipboard *ptr = NULL;
        if (ptr == NULL)
            ptr = new Clipboard;
        return ptr;
    }

private:
    Clipboard() {}
    ~Clipboard() {}
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;

