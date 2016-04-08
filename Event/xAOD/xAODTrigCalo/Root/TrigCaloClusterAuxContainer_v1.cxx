/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloClusterAuxContainer_v1.cxx 639154 2015-01-13 14:35:02Z wigleswt $

// Local include(s):
#include "xAODTrigCalo/versions/TrigCaloClusterAuxContainer_v1.h"

namespace xAOD {

   TrigCaloClusterAuxContainer_v1::TrigCaloClusterAuxContainer_v1()
      : AuxContainerBase() {

     /// Include the TrigCaloCluster variables:
#define TRIGCALOCLUSTER_AUX_VARIABLE( TYPE, NAME ) AUX_VARIABLE( NAME )
#include "xAODTrigCalo/versions/TrigCaloClusterAuxVariables_v1.def"
#undef TRIGCALOCLUSTER_AUX_VARIABLE
     
   }

} // namespace xAOD
