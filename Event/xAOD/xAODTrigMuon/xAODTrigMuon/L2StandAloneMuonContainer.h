// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2StandAloneMuonContainer.h 700475 2015-10-14 08:56:00Z mishitsu $
#ifndef XAODTRIGMUON_L2STANDALONEMUONCONTAINER_H
#define XAODTRIGMUON_L2STANDALONEMUONCONTAINER_H

// Local include(s):
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonContainer_v2.h"

namespace xAOD {
  /// Define the latest version of the muon SA container
  typedef L2StandAloneMuonContainer_v2 L2StandAloneMuonContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::L2StandAloneMuonContainer, 1307619630, 1 )

#endif // XAODTRIGMUON_L2STANDALONEMUONCONTAINER_H
