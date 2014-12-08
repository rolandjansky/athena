/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// L2IsoMuonAuxContainer.h giagu
#ifndef XAODTRIGMUON_L2ISOMUONAUXCONTAINER_H
#define XAODTRIGMUON_L2ISOMUONAUXCONTAINER_H

// Local include(s):
#include "xAODTrigMuon/versions/L2IsoMuonAuxContainer_v1.h"

namespace xAOD {
  /// Define the latest version of the muon Iso auxiliary container
  typedef L2IsoMuonAuxContainer_v1 L2IsoMuonAuxContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::L2IsoMuonAuxContainer, 1128059587, 1 )

#endif // XAODTRIGMUON_L2ISOMUONAUXCONTAINER_H
