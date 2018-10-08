/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterAuxContainer_v2.cxx 628097 2014-11-13 09:24:54Z krasznaa $

// Local include(s):
#include "xAODTrigCalo/versions/CaloClusterTrigAuxContainer_v1.h"

namespace xAOD {

   CaloClusterTrigAuxContainer_v1::CaloClusterTrigAuxContainer_v1()
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
     AUX_VARIABLE ( eta_sampl );
     AUX_VARIABLE ( phi_sampl );

     AUX_VARIABLE ( ETACALOFRAME );
     AUX_VARIABLE ( ETA1CALOFRAME );
     AUX_VARIABLE ( ETA2CALOFRAME );
     
     AUX_VARIABLE ( PHICALOFRAME );
     AUX_VARIABLE ( PHI1CALOFRAME );
     AUX_VARIABLE ( PHI2CALOFRAME );     

     AUX_VARIABLE ( CENTER_LAMBDA );
     AUX_VARIABLE ( FIRST_ENG_DENS );
     AUX_VARIABLE ( EM_PROBABILITY );
     AUX_VARIABLE ( SECOND_LAMBDA );
     AUX_VARIABLE ( SECOND_R );  
   }

} // namespace xAOD
