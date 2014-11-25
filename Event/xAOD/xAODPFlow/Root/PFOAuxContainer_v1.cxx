/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PFOAuxContainer_v1.cxx 624825 2014-10-29 11:29:33Z mhodgkin $

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
    AUX_VARIABLE(mEM);
    AUX_VARIABLE(pfo_ClusterLinks);
    AUX_VARIABLE(pfo_TrackLinks);
    AUX_VARIABLE(pfo_TauShotLinks);
  }
  
} // namespace xAOD
