/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "L1CaloFEXSim/eFEXDriver.h"
#include "L1CaloFEXSim/eFEXSysSim.h"
#include "L1CaloFEXSim/eFEXSim.h"
#include "L1CaloFEXSim/eFEXFPGA.h"
#include "L1CaloFEXSim/eFEXtauAlgo.h"
#include "L1CaloFEXSim/eFEXegAlgo.h"
#include "L1CaloFEXSim/eFEXNtupleWriter.h"

using namespace LVL1;

DECLARE_COMPONENT(eFEXDriver)
DECLARE_COMPONENT(eFEXSysSim)
DECLARE_COMPONENT(eFEXSim)
DECLARE_COMPONENT(eTowerBuilder)
DECLARE_COMPONENT(eSuperCellTowerMapper)
DECLARE_COMPONENT(eFEXFPGA)
DECLARE_COMPONENT(eFEXtauAlgo)
DECLARE_COMPONENT(eFEXegAlgo)
DECLARE_COMPONENT(eFEXNtupleWriter)
