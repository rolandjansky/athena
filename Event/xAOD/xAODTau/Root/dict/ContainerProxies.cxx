/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 740338 2016-04-14 16:03:58Z griffith $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODTau/versions/TauJetContainer_v1.h"
#include "xAODTau/versions/TauJetContainer_v2.h"
#include "xAODTau/versions/TauJetContainer_v3.h"
#include "xAODTau/versions/DiTauJetContainer_v1.h"
#include "xAODTau/versions/TauTrackContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, TauJetContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TauJetContainer_v2);
ADD_NS_DV_PROXY( xAOD, TauJetContainer_v3);
ADD_NS_DV_PROXY( xAOD, DiTauJetContainer_v1);
ADD_NS_DV_PROXY( xAOD, TauTrackContainer_v1);
