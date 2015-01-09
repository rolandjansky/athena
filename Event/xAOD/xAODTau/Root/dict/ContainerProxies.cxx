/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 631748 2014-11-28 09:58:13Z janus $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODTau/versions/TauJetContainer_v1.h"
#include "xAODTau/versions/TauJetContainer_v2.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, TauJetContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TauJetContainer_v2);
