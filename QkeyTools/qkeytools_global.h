#ifndef QKEYTOOLS_GLOBAL_H
#define QKEYTOOLS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QKEYTOOLS_LIBRARY)
#  define QKEYTOOLSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QKEYTOOLSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QKEYTOOLS_GLOBAL_H
