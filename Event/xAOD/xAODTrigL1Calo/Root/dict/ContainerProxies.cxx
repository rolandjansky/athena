/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 608592 2014-07-25 13:16:23Z morrisj $

// xAOD include(s):
#include "xAODCore/AddDVProxy.h"
       
// Local include(s):
#include "xAODTrigL1Calo/versions/JEMHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/JEMEtSumsContainer_v1.h"
#include "xAODTrigL1Calo/versions/JEMRoIContainer_v1.h"

#include "xAODTrigL1Calo/versions/CPMHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CPMTowerContainer_v1.h"
#include "xAODTrigL1Calo/versions/CPMRoIContainer_v1.h"

#include "xAODTrigL1Calo/versions/CMMCPHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMMEtSumsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMMJetHitsContainer_v1.h"

#include "xAODTrigL1Calo/versions/JetElementContainer_v1.h"
#include "xAODTrigL1Calo/versions/RODHeaderContainer_v1.h"
#include "xAODTrigL1Calo/versions/TriggerTowerContainer_v1.h"
#include "xAODTrigL1Calo/versions/TriggerTowerContainer_v2.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, JEMHitsContainer_v1 );
ADD_NS_DV_PROXY( xAOD, JEMEtSumsContainer_v1 );
ADD_NS_DV_PROXY( xAOD, JEMRoIContainer_v1 );

ADD_NS_DV_PROXY( xAOD, CPMHitsContainer_v1 );
ADD_NS_DV_PROXY( xAOD, CPMTowerContainer_v1 );
ADD_NS_DV_PROXY( xAOD, CPMRoIContainer_v1 );

ADD_NS_DV_PROXY( xAOD, CMMCPHitsContainer_v1 );
ADD_NS_DV_PROXY( xAOD, CMMEtSumsContainer_v1 );
ADD_NS_DV_PROXY( xAOD, CMMJetHitsContainer_v1 );

ADD_NS_DV_PROXY( xAOD, JetElementContainer_v1 );
ADD_NS_DV_PROXY( xAOD, RODHeaderContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TriggerTowerContainer_v1 );
ADD_NS_DV_PROXY( xAOD, TriggerTowerContainer_v2 );
