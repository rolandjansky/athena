/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMClusterAuxContainer_v2.cxx 592326 2014-04-10 10:55:31Z krasznaa $

// Local include(s):
#include "xAODTrigCalo/versions/TrigEMClusterAuxContainer_v2.h"

namespace xAOD {

   TrigEMClusterAuxContainer_v2::TrigEMClusterAuxContainer_v2()
      : AuxContainerBase() {

     /// Include the TrigCaloCluster variables:
#define TRIGCALOCLUSTER_AUX_VARIABLE( TYPE, NAME ) AUX_VARIABLE( NAME )
#include "xAODTrigCalo/versions/TrigCaloClusterAuxVariables_v1.def"
#undef TRIGCALOCLUSTER_AUX_VARIABLE
     
      AUX_VARIABLE( energy );
      AUX_VARIABLE( energySample );
      AUX_VARIABLE( et );
      AUX_VARIABLE( eta );
      AUX_VARIABLE( phi );
      AUX_VARIABLE( e237 );
      AUX_VARIABLE( e277 );
      AUX_VARIABLE( fracs1 );
      AUX_VARIABLE( weta2 );
      AUX_VARIABLE( ehad1 );
      AUX_VARIABLE( eta1 );
      AUX_VARIABLE( emaxs1 );
      AUX_VARIABLE( e2tsts1 );
      AUX_VARIABLE( e233 );
      AUX_VARIABLE( wstot );

   }

} // namespace xAOD
