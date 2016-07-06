// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODROOTACCESS_TOOLS_RETURNCHECK_H
#define XAODROOTACCESS_TOOLS_RETURNCHECK_H

// ROOT include(s):
#include <TError.h>

// Local include(s):
#include "xAODRootAccess/tools/TReturnCode.h"
#include "xAODRootAccess/tools/Message.h"

/// Helper macro for checking return codes in a compact form in the code
///
/// This is pretty much a rip-off of the (ATH_)CHECK macros of the offline
/// code. It is used in the package in functions that return a TReturnCode,
/// and themselves call functions returning TReturnCode.
///
/// @param CONTEXT A context string to print an error message on failure
/// @param EXP The expression to execute in a checked manner
///
#define RETURN_CHECK( CONTEXT, EXP )                                 \
   do {                                                              \
      const auto result = EXP;                                       \
      if( ! result.isSuccess() ) {                                   \
         ::Error( CONTEXT, XAOD_MESSAGE( "Failed to execute: %s" ),  \
                  #EXP );                                            \
         return result;                                              \
      }                                                              \
   } while( false )

#endif // XAODROOTACCESS_TOOLS_RETURNCHECK_H
