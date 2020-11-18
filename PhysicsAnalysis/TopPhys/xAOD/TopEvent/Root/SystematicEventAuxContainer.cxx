//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "TopEvent/SystematicEventAuxContainer.h"

// EDM includes(s).
#include "xAODCore/AuxStoreAccessorMacros.h"

namespace xAOD {

   SystematicEventAuxContainer::SystematicEventAuxContainer()
   : AuxContainerBase() {

      AUX_VARIABLE(hashValue);
      AUX_VARIABLE(ttreeIndex);
      AUX_VARIABLE(isLooseEvent);

      AUX_VARIABLE(goodPhotons);
      AUX_VARIABLE(goodElectrons);
      AUX_VARIABLE(goodFwdElectrons);
      AUX_VARIABLE(goodMuons);
      AUX_VARIABLE(goodSoftMuons);
      AUX_VARIABLE(goodTaus);
      AUX_VARIABLE(goodJets);
      AUX_VARIABLE(goodLargeRJets);
      AUX_VARIABLE(goodTrackJets);
      AUX_VARIABLE(goodTracks);
   }

} // namespace xAOD
