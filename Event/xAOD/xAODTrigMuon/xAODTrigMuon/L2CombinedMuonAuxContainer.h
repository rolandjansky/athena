/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// L2CombinedMuonAuxContainer.h giagu
#ifndef XAODTRIGMUON_L2COMBINEDMUONAUXCONTAINER_H
#define XAODTRIGMUON_L2COMBINEDMUONAUXCONTAINER_H

// Local include(s):
#include "xAODTrigMuon/versions/L2CombinedMuonAuxContainer_v1.h"

namespace xAOD {
  /// Define the latest version of the muon CB auxiliary container
  typedef L2CombinedMuonAuxContainer_v1 L2CombinedMuonAuxContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::L2CombinedMuonAuxContainer, 1201627828 , 1 )

#endif // XAODTRIGMUON_L2COMBINEDMUONAUXCONTAINER_H
