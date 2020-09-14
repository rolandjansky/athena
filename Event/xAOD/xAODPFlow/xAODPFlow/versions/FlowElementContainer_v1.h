//  this file is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef XAODPFLOW_VERSIONS_FlowElementCONTAINER_V1_H
#define XAODPFLOW_VERSIONS_FlowElementCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODPFlow/versions/FlowElement_v1.h"
// initialise the base class of the FlowElementContainer as an IParticle
//DATAVECTOR_BASE(xAOD::FlowElement_v1, xAOD::IParticle);
namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::FlowElement_v1 > FlowElementContainer_v1;
}

#endif // XAODPFLOW_VERSIONS_FlowElementCONTAINER_V1_H
