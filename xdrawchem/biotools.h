// biotools.h -- subclass of Drawable for representing various biochemical
// structures:  membranes, antibodies, etc.

#ifndef BIOTOOLS_H
#define BIOTOOLS_H

#include <QString>

#include "drawable.h"

class Molecule;
class Render2D;

class BioTools : public Drawable
{
public:
  BioTools();
  void Render( Render2D * );  // draw this object
  int Type();  // return type of object
  void SetWhich( QString );
  QString GetWhich() { return which; }

private:
  QString which;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
