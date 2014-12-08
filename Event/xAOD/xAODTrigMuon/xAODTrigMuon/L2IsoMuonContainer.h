/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// L2IsoMuonContainer.h giagu
#ifndef XAODTRIGMUON_L2ISOMUONCONTAINER_H
#define XAODTRIGMUON_L2ISOMUONCONTAINER_H

// Local include(s):
#include "xAODTrigMuon/L2IsoMuon.h"
#include "xAODTrigMuon/versions/L2IsoMuonContainer_v1.h"

namespace xAOD {
  /// Define the latest version of the muon Iso container
  typedef L2IsoMuonContainer_v1 L2IsoMuonContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::L2IsoMuonContainer, 1103714444, 1 )

#endif // XAODTRIGMUON_L2ISOMUONCONTAINER_H
