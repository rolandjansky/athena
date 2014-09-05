/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_PREPRAWDATAAUXCONTAINER_H
#define XAODTRACKING_PREPRAWDATAAUXCONTAINER_H
 
// Local include(s):
#include "xAODTracking/versions/PrepRawDataAuxContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current Muon auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::PrepRawDataContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef PrepRawDataAuxContainer_v1 PrepRawDataAuxContainer;
}
 
#endif // XAODTRACKING_PREPRAWDATAAUXCONTAINER_H
