/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigMissingET/versions/TrigMissingETAuxContainer_v1.h"

namespace xAOD {

  TrigMissingETAuxContainer_v1::TrigMissingETAuxContainer_v1 ()
    : AuxContainerBase() {

    AUX_VARIABLE ( ex );
    AUX_VARIABLE ( ey );
    AUX_VARIABLE ( ez );
    AUX_VARIABLE ( sumEt );
    AUX_VARIABLE ( sumE );
    AUX_VARIABLE ( flag );
    AUX_VARIABLE ( roiWord );
    
    AUX_VARIABLE ( nameOfComponent );
    AUX_VARIABLE ( exComponent );
    AUX_VARIABLE ( eyComponent );
  }

}
