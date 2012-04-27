#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QList>

#include "drawable.h"

class Clipboard
{
public:
    Clipboard() {}
    Clipboard(Clipboard* c) { objects = c->objects; }
    QList<QSharedPointer<Drawable> > objects;
    void clear() { objects.clear(); }
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;

