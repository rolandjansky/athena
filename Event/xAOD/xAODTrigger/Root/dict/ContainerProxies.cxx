/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 755776 2016-06-17 13:45:30Z krasznaa $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODTrigger/versions/TriggerMenuContainer_v1.h"
#include "xAODTrigger/versions/TriggerMenuJsonContainer_v1.h"

#include "xAODTrigger/versions/EmTauRoIContainer_v1.h"
#include "xAODTrigger/versions/EmTauRoIContainer_v2.h"
#include "xAODTrigger/versions/JetRoIContainer_v1.h"
#include "xAODTrigger/versions/JetRoIContainer_v2.h"
#include "xAODTrigger/versions/MuonRoIContainer_v1.h"
#include "xAODTrigger/versions/eFexEMRoIContainer_v1.h"

#include "xAODTrigger/versions/eFexTauRoIContainer_v1.h"
#include "xAODTrigger/versions/jFexSRJetRoIContainer_v1.h"
#include "xAODTrigger/versions/jFexLRJetRoIContainer_v1.h"
#include "xAODTrigger/versions/jFexTauRoIContainer_v1.h"
#include "xAODTrigger/versions/jFexSumETRoIContainer_v1.h"
#include "xAODTrigger/versions/jFexMETRoIContainer_v1.h"

#include "xAODTrigger/versions/gFexJetRoIContainer_v1.h"
#include "xAODTrigger/versions/gFexGlobalRoIContainer_v1.h"

#include "xAODTrigger/versions/TrigCompositeContainer_v1.h"
#include "xAODTrigger/versions/BunchConfContainer_v1.h"
#include "xAODTrigger/versions/TrigPassBitsContainer_v1.h"

#include "xAODTrigger/versions/L1TopoSimResultsContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, TriggerMenuContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TriggerMenuJsonContainer_v1 );

ADD_NS_DV_PROXY( xAOD, EmTauRoIContainer_v1 );
ADD_NS_DV_PROXY( xAOD, EmTauRoIContainer_v2 );
ADD_NS_DV_PROXY( xAOD, JetRoIContainer_v1 );
ADD_NS_DV_PROXY( xAOD, JetRoIContainer_v2 );
ADD_NS_DV_PROXY( xAOD, MuonRoIContainer_v1 );
ADD_NS_DV_PROXY( xAOD, eFexEMRoIContainer_v1 );
ADD_NS_DV_PROXY( xAOD, eFexTauRoIContainer_v1 );
ADD_NS_DV_PROXY( xAOD, jFexSRJetRoIContainer_v1);
ADD_NS_DV_PROXY( xAOD, jFexLRJetRoIContainer_v1);
ADD_NS_DV_PROXY( xAOD, jFexTauRoIContainer_v1);
ADD_NS_DV_PROXY( xAOD, jFexSumETRoIContainer_v1);
ADD_NS_DV_PROXY( xAOD, jFexMETRoIContainer_v1);
ADD_NS_DV_PROXY( xAOD, gFexJetRoIContainer_v1 );
ADD_NS_DV_PROXY( xAOD, gFexGlobalRoIContainer_v1 );

ADD_NS_DV_PROXY( xAOD, TrigCompositeContainer_v1 );
ADD_NS_DV_PROXY( xAOD, BunchConfContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TrigPassBitsContainer_v1 );

ADD_NS_DV_PROXY( xAOD, L1TopoSimResultsContainer_v1 );
