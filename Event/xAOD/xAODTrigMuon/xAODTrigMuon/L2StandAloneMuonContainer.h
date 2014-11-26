// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2StandAloneMuonContainer.h 631391 2014-11-26 21:43:36Z gwatts $
#ifndef XAODTRIGMUON_L2STANDALONEMUONCONTAINER_H
#define XAODTRIGMUON_L2STANDALONEMUONCONTAINER_H

// Local include(s):
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonContainer_v1.h"

namespace xAOD {
  /// Define the latest version of the muon SA container
  typedef L2StandAloneMuonContainer_v1 L2StandAloneMuonContainer;
}

#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::L2StandAloneMuonContainer, 1307619630, 1 )

#endif // XAODTRIGMUON_L2STANDALONEMUONCONTAINER_H
