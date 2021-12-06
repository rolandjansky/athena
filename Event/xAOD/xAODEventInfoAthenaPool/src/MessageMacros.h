// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
//
// Dear emacs, this is -*- c++ -*-
//
#ifndef XAODEVENTINFOATHENAPOOL_MESSAGEMACROS_H
#define XAODEVENTINFOATHENAPOOL_MESSAGEMACROS_H

// Gaudi include(s).
#include "GaudiKernel/MsgStream.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

#endif // XAODEVENTINFOATHENAPOOL_MESSAGEMACROS_H
