/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 693681 2015-09-08 12:18:40Z amazurov $

// xAOD include(s):
#include "xAODCore/AddDVProxy.h"
       
// Local include(s):

// Run 1
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

// Run 2
#include "xAODTrigL1Calo/versions/TriggerTowerContainer_v2.h"
#include "xAODTrigL1Calo/versions/RODHeaderContainer_v2.h"

#include "xAODTrigL1Calo/versions/CMXCPHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXCPTobContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXJetHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXJetTobContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXEtSumsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXRoIContainer_v1.h"

#include "xAODTrigL1Calo/versions/CPMTowerContainer_v2.h"
#include "xAODTrigL1Calo/versions/CPMTobRoIContainer_v1.h"

#include "xAODTrigL1Calo/versions/JEMEtSumsContainer_v2.h"
#include "xAODTrigL1Calo/versions/JEMTobRoIContainer_v1.h"

#include "xAODTrigL1Calo/versions/JetElementContainer_v2.h"

#include "xAODTrigL1Calo/versions/L1TopoRawDataContainer_v1.h"

// Run 3
#include "xAODTrigL1Calo/versions/JGTowerContainer_v1.h"
#include "xAODTrigL1Calo/versions/GBlockContainer_v1.h"

// Set up the collection proxies:
// Run 1
ADD_NS_DV_PROXY( xAOD , JEMHitsContainer_v1 );
ADD_NS_DV_PROXY( xAOD , JEMEtSumsContainer_v1 );
ADD_NS_DV_PROXY( xAOD , JEMRoIContainer_v1 );

ADD_NS_DV_PROXY( xAOD , CPMHitsContainer_v1 );
ADD_NS_DV_PROXY( xAOD , CPMTowerContainer_v1 );
ADD_NS_DV_PROXY( xAOD , CPMRoIContainer_v1 );

ADD_NS_DV_PROXY( xAOD , CMMCPHitsContainer_v1 );
ADD_NS_DV_PROXY( xAOD , CMMEtSumsContainer_v1 );
ADD_NS_DV_PROXY( xAOD , CMMJetHitsContainer_v1 );

ADD_NS_DV_PROXY( xAOD , JetElementContainer_v1 );
ADD_NS_DV_PROXY( xAOD , RODHeaderContainer_v1 );
ADD_NS_DV_PROXY( xAOD , TriggerTowerContainer_v1 );

// Run 2
ADD_NS_DV_PROXY( xAOD , TriggerTowerContainer_v2 );
ADD_NS_DV_PROXY( xAOD , RODHeaderContainer_v2 );

ADD_NS_DV_PROXY( xAOD , CMXCPHitsContainer_v1 );
ADD_NS_DV_PROXY( xAOD , CMXCPTobContainer_v1 );
ADD_NS_DV_PROXY( xAOD , CMXJetHitsContainer_v1 );
ADD_NS_DV_PROXY( xAOD , CMXJetTobContainer_v1 );
ADD_NS_DV_PROXY( xAOD , CMXEtSumsContainer_v1 );
ADD_NS_DV_PROXY( xAOD , CMXRoIContainer_v1 );

ADD_NS_DV_PROXY( xAOD , CPMTowerContainer_v2 );
ADD_NS_DV_PROXY( xAOD , CPMTobRoIContainer_v1 );

ADD_NS_DV_PROXY( xAOD , JEMEtSumsContainer_v2 );
ADD_NS_DV_PROXY( xAOD , JEMTobRoIContainer_v1 );
ADD_NS_DV_PROXY( xAOD , JetElementContainer_v2 );

ADD_NS_DV_PROXY( xAOD , L1TopoRawDataContainer_v1 );

// Run 3
ADD_NS_DV_PROXY( xAOD , JGTowerContainer_v1 );
ADD_NS_DV_PROXY( xAOD, GBlockContainer_v1);
