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
#include "plugin/include/config.h"
#ifdef HAVE_DECL_GETOPT
# undef HAVE_DECL_GETOPT
#endif
#define HAVE_DECL_GETOPT 1

#include "gcc-plugin.h"

#define CHECKER_GCCPLUGINS_VERSION_FULL "0.1"
#define CHECKER_GCCPLUGINS_C_VERSION "Atlas gcc checker plugins version: " CHECKER_GCCPLUGINS_VERSION_FULL


// Declare prototypes for the checker initialization functions.
#define CHECKER(NAME) void init_##NAME##_checker (plugin_name_args* plugin_info);
#include "checkers.def"
#undef CHECKER


#endif // not CHECKERGCCPLUGINS_CHECKER_GCCPLUGINS_H
