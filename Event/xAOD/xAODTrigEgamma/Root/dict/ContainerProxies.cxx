/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 602825 2014-06-20 07:29:53Z krasznaa $
	
// EDM include(s):
#include "xAODCore/AddDVProxy.h"
	
// Local include(s):
#include "xAODTrigEgamma/versions/TrigElectronContainer_v1.h"
#include "xAODTrigEgamma/versions/TrigPhotonContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, TrigElectronContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TrigPhotonContainer_v1 );
