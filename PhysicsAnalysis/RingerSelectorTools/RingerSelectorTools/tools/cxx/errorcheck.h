/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_ERRORCHECK_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_ERRORCHECK_H

#if !defined(RINGER_STANDALONE) && !defined(XAOD_STANDALONE)
# include "AthenaKernel/errorcheck.h"
#else
# ifndef XAOD_STANDALONE
#  include <TError.h> // Include ::Error from root
# endif
# define PLACE_INFO __FILE__
# define CHECK( ARG )                                  \
  do {                                                  \
      const bool result = ARG;                          \
    if( ! result ) {                                    \
      ::Error( PLACE_INFO, "Failed to execute: \"%s\"", \
  #ARG );                                               \
        return 1;                                       \
    }                                                   \
   } while( false )
#endif

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_ERRORCHECK_H
