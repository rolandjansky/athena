/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigBphysAuxContainer_v1.cxx 600124 2014-06-03 15:35:20Z krasznaa $

// Local include(s):
#include "xAODTrigBphys/versions/TrigBphysAuxContainer_v1.h"

namespace xAOD {

    TrigBphysAuxContainer_v1::TrigBphysAuxContainer_v1()
    : AuxContainerBase() {

        AUX_VARIABLE(roiId);
        AUX_VARIABLE(particleType);
        AUX_VARIABLE(level);
        AUX_VARIABLE(eta);
        AUX_VARIABLE(phi);
        AUX_VARIABLE(mass);
        AUX_VARIABLE(fitmass);
        AUX_VARIABLE(fitchi2);
        AUX_VARIABLE(fitndof);
        AUX_VARIABLE(fitx);
        AUX_VARIABLE(fity);
        AUX_VARIABLE(fitz);

        AUX_VARIABLE(secondaryDecayLink);
        AUX_VARIABLE(trackParticleLinks);

    } // TrigBphysAuxContainer_v1

} // namespace xAOD
