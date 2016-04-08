// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: setupJetKeyDescriptor.h 588111 2014-03-18 09:02:18Z krasznaa $
#ifndef TRIGJETUTILS_SETUPJETKEYDESCRIPTOR_H
#define TRIGJETUTILS_SETUPJETKEYDESCRIPTOR_H

// Gaudi/Athena include(s):
#include "GaudiKernel/StatusCode.h"

// Forward declaration(s):
class StoreGateSvc;
class MsgStream;

namespace TrigJetUtils {

   /// The "old" JetAlgorithm::setupJetKeyDescriptor function
   StatusCode setupJetKeyDescriptor( StoreGateSvc* sg, MsgStream& log );

} // namespace TrigJetUtils

#endif // TRIGJETUTILS_SETUPJETKEYDESCRIPTOR_H
