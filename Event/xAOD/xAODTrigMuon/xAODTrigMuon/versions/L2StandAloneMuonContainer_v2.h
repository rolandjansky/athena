// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2StandAloneMuonContainer_v2.h 631108 2014-11-26 07:47:24Z gwatts $
#ifndef XAODTRIGMUON_VERSIONS_L2STANDALONEMUONCONTAINER_V2_H
#define XAODTRIGMUON_VERSIONS_L2STANDALONEMUONCONTAINER_V2_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigMuon/versions/L2StandAloneMuon_v2.h"

// Declare IParticle as a base class of L2StandAloneMuon_v2:
DATAVECTOR_BASE( xAOD::L2StandAloneMuon_v2, xAOD::IParticle );

namespace xAOD {
   /// Define the trigger calorimeter cluster container as a simple DataVector
   typedef DataVector< L2StandAloneMuon_v2 > L2StandAloneMuonContainer_v2;
}

#endif // XAODTRIGMUON_VERSIONS_L2STANDALONEMUONCONTAINER_V2_H
