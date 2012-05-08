/*********************************************************************
 * version.h
 *
 * Copyright (C)
 * 2004, 2005 Bryan Herger -- bherger@users.sourceforge.net
 * 2012 Ralf Stephan -- ralf@ark.in-berlin.de
 *
 * LICENSE: GPL v. 2 see GPL.txt
 *********************************************************************/

#ifndef VERSION_H
#define VERSION_H

#include <QString>

const QString VERSION = QString::fromLatin1("2.0-1205");

// version string (makes updates easier :)
#ifdef UNIX
const QString XDC_VERSION = QString::fromLatin1("XDrawChem ") + VERSION;
#endif

#ifdef WIN32
const QString XDC_VERSION = QString::fromLatin1("WinDrawChem ") + VERSION;
#endif

#ifdef MACOSX
const QString XDC_VERSION = QString::fromLatin1("MacDrawChem ") + VERSION;
#endif

#endif
