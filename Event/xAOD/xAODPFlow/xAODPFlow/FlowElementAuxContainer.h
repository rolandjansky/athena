// this file is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODPFLOW_FLOWELEMENTAUXCONTAINER_H
#define XAODPFLOW_FLOWELEMENTAUXCONTAINER_H

// Local include(s):
#include "xAODPFlow/versions/FlowElementAuxContainer_v1.h"

namespace xAOD {
   /// Definition of the current FlowElementAuxContainer_v1 auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::FlowElementContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef FlowElementAuxContainer_v1 FlowElementAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::FlowElementAuxContainer , 1078624534 , 1 )

#endif // XAODPFLOW_FLOWELEMENTAUXCONTAINER_H
