/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloClusterAuxContainer_v1.cxx 592214 2014-04-09 19:27:48Z krasznaa $

// Local include(s):
#include "xAODTrigCalo/versions/TrigCaloClusterAuxContainer_v1.h"

namespace xAOD {

   TrigCaloClusterAuxContainer_v1::TrigCaloClusterAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( rawEnergy );
      AUX_VARIABLE( rawEnergySample );
      AUX_VARIABLE( rawEt );
      AUX_VARIABLE( rawEta );
      AUX_VARIABLE( rawPhi );
      AUX_VARIABLE( RoIword );
      AUX_VARIABLE( nCells );
      AUX_VARIABLE( clusterQuality );

   }

} // namespace xAOD
