// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2StandAloneMuonContainer_v1.h 592605 2014-04-11 14:24:04Z krasznaa $
#ifndef XAODTRIGMUON_VERSIONS_L2STANDALONEMUONCONTAINER_V1_H
#define XAODTRIGMUON_VERSIONS_L2STANDALONEMUONCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigMuon/versions/L2StandAloneMuon_v1.h"

// Declare IParticle as a base class of L2StandAloneMuon_v1:
DATAVECTOR_BASE( xAOD::L2StandAloneMuon_v1, xAOD::IParticle );

namespace xAOD {
   /// Define the trigger calorimeter cluster container as a simple DataVector
   typedef DataVector< L2StandAloneMuon_v1 > L2StandAloneMuonContainer_v1;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::L2StandAloneMuonContainer_v1, 1307619630, 1 )
#endif // not XAOD_STANDALONE

#endif // XAODTRIGMUON_VERSIONS_L2STANDALONEMUONCONTAINER_V1_H
