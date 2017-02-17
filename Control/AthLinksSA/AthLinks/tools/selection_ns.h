// Dear emacs, this is -*- c++ -*-
// $Id: selection_ns.h 590863 2014-04-02 13:41:29Z krasznaa $

/**
 * @file  AthLinks/tools/selection_ns.h
 * @author scott snyder
 * @date Nov 2008
 * @brief Root changed the name of the selection namespace in 5.19
 *        and 5.99. Define macros to work around.
 */

#ifndef ATHLINKS_TOOLS_SELECTION_NS
#define ATHLINKS_TOOLS_SELECTION_NS

// ROOT include(s):
#include "RVersion.h"

// Include the correct "selection header":
#if ROOT_VERSION_CODE < ROOT_VERSION( 5, 99, 00 )
#   include <Reflex/Builder/DictSelection.h>
#else
#   include <RootMetaSelection.h>
#endif // ROOT 5

#if ROOT_VERSION_CODE < ROOT_VERSION( 5, 19, 0 )

//
// Definitions for *really* old ROOT versions:
//

#ifndef ROOT_SELECTION_NS
#   define ROOT_SELECTION_NS ROOT::Reflex::Selection
#endif

#ifndef ENTER_ROOT_SELECTION_NS
#   define ENTER_ROOT_SELECTION_NS                             \
   namespace ROOT { namespace Reflex { namespace Selection {
#endif

#ifndef EXIT_ROOT_SELECTION_NS
#   define EXIT_ROOT_SELECTION_NS }}}
#endif

#elif ROOT_VERSION_CODE < ROOT_VERSION( 5, 99, 0 )

//
// Definitions for ROOT 5:
//

#ifndef ROOT_SELECTION_NS
#   define ROOT_SELECTION_NS Reflex::Selection
#endif

#ifndef ENTER_ROOT_SELECTION_NS
#   define ENTER_ROOT_SELECTION_NS              \
   namespace Reflex { namespace Selection {
#endif

#ifndef EXIT_ROOT_SELECTION_NS
#   define EXIT_ROOT_SELECTION_NS }}
#endif

#else

//
// Definitions for ROOT 6:
//

#ifndef ROOT_SELECTION_NS
#   define ROOT_SELECTION_NS ROOT::Meta::Selection
#endif

#ifndef ENTER_ROOT_SELECTION_NS
#   define ENTER_ROOT_SELECTION_NS                          \
   namespace ROOT { namespace Meta { namespace Selection {
#endif

#ifndef EXIT_ROOT_SELECTION_NS
#   define EXIT_ROOT_SELECTION_NS }}}
#endif

#endif // ROOT_VERSION

#endif // not ATHLINKS_TOOLS_SELECTION_NS
