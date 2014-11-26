// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2StandAloneMuonAuxContainer.h 631391 2014-11-26 21:43:36Z gwatts $
#ifndef XAODTRIGMUON_L2STANDALONEMUONAUXCONTAINER_H
#define XAODTRIGMUON_L2STANDALONEMUONAUXCONTAINER_H

// Local include(s):
#include "xAODTrigMuon/versions/L2StandAloneMuonAuxContainer_v1.h"

namespace xAOD {
  /// Define the latest version of the muon SA auxiliary container
  typedef L2StandAloneMuonAuxContainer_v1 L2StandAloneMuonAuxContainer;
}

#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::L2StandAloneMuonAuxContainer, 1207804085, 1 )

#endif // XAODTRIGMUON_L2STANDALONEMUONAUXCONTAINER_H
