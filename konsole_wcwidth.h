/*
 * Modifications and refactoring. Part of QtTerminalWidget:
 * https://github.com/cybercatalyst/qtterminalwidget
 *
 * Copyright (C) 2015 Jacob Dawid <jacob@omg-it.works>
 */

/* $XFree86: xc/programs/xterm/wcwidth.h,v 1.2 2001/06/18 19:09:27 dickey Exp $ */

/* Markus Kuhn -- 2001-01-12 -- public domain */
/* Adaptions for KDE by Waldo Bastian <bastian@kde.org> */
/*
    Rewritten for QT4 by e_k <e_k at users.sourceforge.net>
*/


#pragma once

// Qt includes
#include <QtGlobal>
class QString;

int konsole_wcwidth(quint16 ucs);
int string_width( QString txt );
