/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigMinBias/versions/TrigTrackCountsAuxContainer_v1.h"

namespace xAOD {

  TrigTrackCountsAuxContainer_v1::TrigTrackCountsAuxContainer_v1 ()
    : AuxContainerBase() {

    AUX_VARIABLE ( z0_pt );
    AUX_VARIABLE ( eta_phi );
    
    AUX_VARIABLE ( z0Bins );
    AUX_VARIABLE ( z0Min );
    AUX_VARIABLE ( z0Max );
    
    AUX_VARIABLE ( ptBins );
    AUX_VARIABLE ( ptMin );
    AUX_VARIABLE ( ptMax );
    
    AUX_VARIABLE ( etaBins );
    AUX_VARIABLE ( etaMin );
    AUX_VARIABLE ( etaMax );
    
    AUX_VARIABLE ( phiBins );
    AUX_VARIABLE ( phiMin );
    AUX_VARIABLE ( phiMax );

  }

}
