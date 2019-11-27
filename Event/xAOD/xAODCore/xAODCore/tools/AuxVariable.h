// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODCORE_TOOLS_AUXVARIABLE_H
#define XAODCORE_TOOLS_AUXVARIABLE_H

// EDM include(s).
#include "AthContainers/AuxTypeRegistry.h"

// System include(s).
#include <boost/preprocessor/facilities/overload.hpp>

/// Convenience macro for declaring an auxiliary variable
///
/// Should be used in the constructor of the derived class, like:
///   <code>
///      AUX_VARIABLE( RunNumber );
///   </code>
///
/// Can take a optional additional argument giving SG::AuxTypeRegistry::Flags
/// to use for this variable.
///
#define AUX_VARIABLE(...)                                         \
   BOOST_PP_OVERLOAD( AUX_VARIABLE_, __VA_ARGS__ )(__VA_ARGS__)
#define AUX_VARIABLE_1( VAR )                                           \
   do {                                                                 \
      static const auxid_t auxid = getAuxID( #VAR, VAR );               \
      regAuxVar( auxid, #VAR, VAR );                                    \
   } while( false )
#define AUX_VARIABLE_2( VAR, FLAGS )            \
   do {                                         \
      static const auxid_t auxid =              \
         getAuxID( #VAR, VAR, FLAGS );          \
      regAuxVar( auxid, #VAR, VAR );            \
   } while( false )

#endif // XAODCORE_TOOLS_AUXVARIABLE_H
