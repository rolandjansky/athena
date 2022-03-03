/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "L1CaloFEXSim/eFEXDriver.h"
#include "L1CaloFEXSim/eFEXSysSim.h"
#include "L1CaloFEXSim/eFEXFillEDM.h"
#include "L1CaloFEXSim/eFEXFormTOBs.h"
#include "L1CaloFEXSim/eFEXSim.h"
#include "L1CaloFEXSim/eFEXFPGA.h"
#include "L1CaloFEXSim/eFEXtauAlgo.h"
#include "L1CaloFEXSim/eFEXegAlgo.h"
#include "L1CaloFEXSim/eFEXNtupleWriter.h"
#include "L1CaloFEXSim/jFEXDriver.h"
#include "L1CaloFEXSim/jFEXSysSim.h"
#include "L1CaloFEXSim/jFEXSim.h"
#include "L1CaloFEXSim/jFEXFPGA.h"
#include "L1CaloFEXSim/jFEXSmallRJetAlgo.h"
#include "L1CaloFEXSim/jFEXtauAlgo.h"
#include "L1CaloFEXSim/jFEXsumETAlgo.h"
#include "L1CaloFEXSim/jFEXmetAlgo.h"
#include "L1CaloFEXSim/jFEXLargeRJetAlgo.h"
#include "L1CaloFEXSim/jFEXForwardJetsAlgo.h"
#include "L1CaloFEXSim/jFEXForwardElecAlgo.h"
#include "L1CaloFEXSim/jFEXNtupleWriter.h"
#include "L1CaloFEXSim/eFEXFPGATowerIdProvider.h"
#include "L1CaloFEXSim/eFEXSuperCellTowerIdProvider.h"
#include "L1CaloFEXSim/eFakeTower.h"
#include "L1CaloFEXSim/jFEXPileupAndNoise.h"

#include "L1CaloFEXSim/gFEXDriver.h"
#include "L1CaloFEXSim/gFEXSysSim.h"
#include "L1CaloFEXSim/gFEXSim.h"
#include "L1CaloFEXSim/gFEXFPGA.h"
#include "L1CaloFEXSim/gFEXJetAlgo.h"
#include "L1CaloFEXSim/gFEXJwoJAlgo.h"
#include "L1CaloFEXSim/gFEXNtupleWriter.h"



using namespace LVL1;

DECLARE_COMPONENT(eFEXDriver)
DECLARE_COMPONENT(eFEXSysSim)
DECLARE_COMPONENT(eFEXFillEDM)
DECLARE_COMPONENT(eFEXFormTOBs)
DECLARE_COMPONENT(eFEXSim)
DECLARE_COMPONENT(eTowerBuilder)
DECLARE_COMPONENT(eSuperCellTowerMapper)
DECLARE_COMPONENT(eFEXFPGA)
DECLARE_COMPONENT(eFEXtauAlgo)
DECLARE_COMPONENT(eFEXegAlgo)
DECLARE_COMPONENT(eFEXNtupleWriter)
DECLARE_COMPONENT(eFEXFPGATowerIdProvider)
DECLARE_COMPONENT(eFEXSuperCellTowerIdProvider)
DECLARE_COMPONENT(eFakeTower)

DECLARE_COMPONENT(jFEXDriver)
DECLARE_COMPONENT(jFEXSysSim)
DECLARE_COMPONENT(jFEXSim)
DECLARE_COMPONENT(jTowerBuilder)
DECLARE_COMPONENT(jSuperCellTowerMapper)
DECLARE_COMPONENT(jFEXFPGA)
DECLARE_COMPONENT(jFEXSmallRJetAlgo)
DECLARE_COMPONENT(jFEXtauAlgo)
DECLARE_COMPONENT(jFEXPileupAndNoise)
DECLARE_COMPONENT(jFEXsumETAlgo)
DECLARE_COMPONENT(jFEXmetAlgo)
DECLARE_COMPONENT(jFEXLargeRJetAlgo)
DECLARE_COMPONENT(jFEXForwardJetsAlgo)
DECLARE_COMPONENT(jFEXForwardElecAlgo)
DECLARE_COMPONENT(jFEXNtupleWriter)

DECLARE_COMPONENT(gFEXDriver)
DECLARE_COMPONENT(gFEXSysSim)
DECLARE_COMPONENT(gFEXSim)
DECLARE_COMPONENT(gTowerBuilder)
DECLARE_COMPONENT(gSuperCellTowerMapper)
DECLARE_COMPONENT(gFEXFPGA)
DECLARE_COMPONENT(gFEXJetAlgo)
DECLARE_COMPONENT(gFEXJwoJAlgo)
DECLARE_COMPONENT(gFEXNtupleWriter)

