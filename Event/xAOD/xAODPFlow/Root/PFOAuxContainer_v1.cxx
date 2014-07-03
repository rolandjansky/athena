/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PFOAuxContainer_v1.cxx 595979 2014-05-08 09:37:35Z mhodgkin $

// Local include(s):
#include "xAODPFlow/versions/PFOAuxContainer_v1.h"

namespace xAOD {
  
  PFOAuxContainer_v1::PFOAuxContainer_v1()
    : AuxContainerBase() {
    
    AUX_VARIABLE(bdtPi0Score);
    AUX_VARIABLE(centerMag);
    AUX_VARIABLE(charge);
    AUX_VARIABLE(pt);
    AUX_VARIABLE(eta);
    AUX_VARIABLE(phi);
    AUX_VARIABLE(m);
    AUX_VARIABLE(ptEM);
    AUX_VARIABLE(etaEM);
    AUX_VARIABLE(phiEM);
    AUX_VARIABLE(mEM);
    AUX_VARIABLE(pfo_ClusterLinks);
    AUX_VARIABLE(pfo_TrackLinks);
    AUX_VARIABLE(pfo_TauShotLinks);
  }
  
} // namespace xAOD
