// This file's extension implies that it's C, but it's really -*- C++ -*-.
// $Id$
/**
 * @file CheckerGccPlugins/src/checker_gccplugins.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2014
 * @brief Framework for running checker plugins in gcc.
 */


#ifndef CHECKERGCCPLUGINS_CHECKER_GCCPLUGINS_H
#define CHECKERGCCPLUGINS_CHECKER_GCCPLUGINS_H


#ifdef PACKAGE_VERSION
# undef PACKAGE_VERSION
#endif
#include "config.h"
#ifdef HAVE_DECL_GETOPT
# undef HAVE_DECL_GETOPT
#endif
#define HAVE_DECL_GETOPT 1

#include "gcc-plugin.h"
#include "input.h"
#include <string.h>

#define CHECKER_GCCPLUGINS_VERSION_FULL "0.1"
#define CHECKER_GCCPLUGINS_C_VERSION "Atlas gcc checker plugins version: " CHECKER_GCCPLUGINS_VERSION_FULL


namespace CheckerGccPlugins {


/// Has DECL been declared thread-safe?
bool is_thread_safe (tree decl);

bool is_thread_safe_location (location_t loc);

void handle_thread_safe_pragma (cpp_reader*);

void inform_url (location_t loc, const char* url);

inline
bool startswith (const char* s, const char* prefix)
{
  return strncmp (s, prefix, strlen(prefix)) == 0;
}


// In gcc6, the type of `gimple' changed: it used to be a pointer to a struct,
// now it's a struct itself.
#if GCC_VERSION < 6000
typedef gimple gimplePtr;
#else
typedef gimple* gimplePtr;
#endif


} // namespace CheckerGccPlugins


// Declare prototypes for the checker initialization functions.
#define CHECKER(NAME, FLAG) void init_##NAME##_checker (plugin_name_args* plugin_info);
#include "checkers.def"
#undef CHECKER


#endif // not CHECKERGCCPLUGINS_CHECKER_GCCPLUGINS_H
