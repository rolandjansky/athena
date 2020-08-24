// Dear emacs, this is -*- c++ -*-
/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

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

#include <RootMetaSelection.h>

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

#endif // not ATHLINKS_TOOLS_SELECTION_NS
