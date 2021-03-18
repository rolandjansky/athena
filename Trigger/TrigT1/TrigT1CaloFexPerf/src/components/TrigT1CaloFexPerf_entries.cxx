/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "../METNoiseCutPerfFex.h"
#include "../JGTowerBuilder.h"
#include "../JTowerRhoSubtractionAlg.h"
#include "../GTowersFromGCaloTowers.h"
#include "../GTowerRhoSubtractionAlg.h"
#include "../JGTowerNoiseAlg.h"
#include "../GBlockBuilder.h"
#include "../METJWoJPerfFex.h"
// eFEX
#include "../EFexEMAlgorithm.h"
#include "../EFexTauAlgorithm.h"
#include "../EFexEMEnergyWeightedClusterTool.h"
#include "../EFexEMClusterTool.h"
#include "../JTowerMappingDataCondAlg.h"
#include "../GTowerMappingDataCondAlg.h"

DECLARE_COMPONENT(LVL1::METNoiseCutPerfFex)
DECLARE_COMPONENT(LVL1::JGTowerBuilder)
DECLARE_COMPONENT(LVL1::JTowerRhoSubtractionAlg)
DECLARE_COMPONENT(LVL1::GTowersFromGCaloTowers)
DECLARE_COMPONENT(LVL1::GTowerRhoSubtractionAlg)
DECLARE_COMPONENT(LVL1::JGTowerNoiseAlg)
DECLARE_COMPONENT(LVL1::GBlockBuilder)
DECLARE_COMPONENT(LVL1::METJWoJPerfFex)
DECLARE_COMPONENT(LVL1::JTowerMappingDataCondAlg)
DECLARE_COMPONENT(LVL1::GTowerMappingDataCondAlg)
// eFEX
DECLARE_COMPONENT(LVL1::EFexEMAlgorithm)
DECLARE_COMPONENT(LVL1::EFexTauAlgorithm)
DECLARE_COMPONENT(LVL1::EFexEMEnergyWeightedClusterTool)
DECLARE_COMPONENT(LVL1::EFexEMClusterTool)