#include "application.h"
#include "biotools.h"
#include "defs.h"

QToolBar *ApplicationWindow::BuildBioTools()
{
    QToolBar *localbar = new QToolBar( this );

    return localbar;
}

BioTools::BioTools()
{
    which = "";
}

void BioTools::SetWhich( QString w1 )
{
    which = w1;
}

void BioTools::Render(Render2D *)
{
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
