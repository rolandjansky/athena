/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigBphysAuxContainer_v1.cxx 696633 2015-09-24 14:59:50Z jwalder $

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
        AUX_VARIABLE(pt);
        AUX_VARIABLE(mass);
        AUX_VARIABLE(fitmass);
        AUX_VARIABLE(fitchi2);
        AUX_VARIABLE(fitndof);
        AUX_VARIABLE(fitx);
        AUX_VARIABLE(fity);
        AUX_VARIABLE(fitz);
        
        AUX_VARIABLE(fitmassError);
        AUX_VARIABLE(lxy     );
        AUX_VARIABLE(lxyError);
        AUX_VARIABLE(tau     );
        AUX_VARIABLE(tauError);

        
        
        AUX_VARIABLE(secondaryDecayLink);
        AUX_VARIABLE(trackParticleLinks);

        AUX_VARIABLE(vecRoiIds);
        AUX_VARIABLE(lowerChainLink);
        AUX_VARIABLE(particleLinks);


    } // TrigBphysAuxContainer_v1

} // namespace xAOD
