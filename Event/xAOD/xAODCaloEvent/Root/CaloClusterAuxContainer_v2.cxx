/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterAuxContainer_v2.cxx 628097 2014-11-13 09:24:54Z krasznaa $

// Local include(s):
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v2.h"

namespace xAOD {

   CaloClusterAuxContainer_v2::CaloClusterAuxContainer_v2()
      : AuxContainerBase() {

     AUX_VARIABLE( time );
     AUX_VARIABLE( clusterSize );
     AUX_VARIABLE( eta0 );
     AUX_VARIABLE( phi0 );
     AUX_VARIABLE( rawE );
     AUX_VARIABLE( rawEta );
     AUX_VARIABLE( rawPhi );
     AUX_VARIABLE( rawM );
     AUX_VARIABLE( altE );
     AUX_VARIABLE( altEta );
     AUX_VARIABLE( altPhi );
     AUX_VARIABLE( altM );
     AUX_VARIABLE( calE );
     AUX_VARIABLE( calEta );
     AUX_VARIABLE( calPhi );
     AUX_VARIABLE( calM );

     // Energies in various samplings
     AUX_VARIABLE ( e_sampl );
   }

} // namespace xAOD
