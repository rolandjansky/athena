/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// L2CombinedMuonContainer.h giagu
#ifndef XAODTRIGMUON_L2COMBINEDMUONCONTAINER_H
#define XAODTRIGMUON_L2COMBINEDMUONCONTAINER_H

// Local include(s):
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODTrigMuon/versions/L2CombinedMuonContainer_v1.h"

namespace xAOD {
  /// Define the latest version of the muon CB container
  typedef L2CombinedMuonContainer_v1 L2CombinedMuonContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::L2CombinedMuonContainer, 1201561309, 1 )

#endif // XAODTRIGMUON_L2COMBINEDMUONCONTAINER_H
