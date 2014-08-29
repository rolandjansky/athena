// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthVertexContainerFwd.h 574253 2013-12-06 15:21:49Z krasznaa $
#ifndef XAODTRUTH_TRUTHVERTEXCONTAINERFWD_H
#define XAODTRUTH_TRUTHVERTEXCONTAINERFWD_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTruth/TruthVertexFwd.h"

namespace xAOD {
   /// Provide a forward declaration for TruthVertexContainer
   typedef DataVector< TruthVertex > TruthVertexContainer;
}

#endif // XAODTRUTH_TRUTHVERTEXCONTAINERFWD_H
