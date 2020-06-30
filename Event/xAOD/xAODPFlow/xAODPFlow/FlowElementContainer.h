//  this file is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPFLOW_FlowElementCONTAINER_H
#define XAODPFLOW_FlowElementCONTAINER_H

// Local include(s):
#include "xAODPFlow/FlowElement.h"
#include "xAODPFlow/versions/FlowElementContainer_v1.h"

namespace xAOD {
   /// Definition of the current "pfo container version"
   typedef FlowElementContainer_v1 FlowElementContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::FlowElementContainer , 1158384067 , 1 )

#endif // XAODPFLOW_FlowElementCONTAINER_H
