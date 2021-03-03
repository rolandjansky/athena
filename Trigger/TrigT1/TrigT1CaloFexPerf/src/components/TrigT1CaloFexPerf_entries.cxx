/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */
 
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../JGTowerMaker.h"
#include "../METNoiseCutPerfFex.h"
#include "../JGTowerBuilder.h"
#include "../JTowerMappingMaker.h"
#include "../GTowerMappingMaker.h"
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

#include "../SCEmulation.h"

DECLARE_ALGORITHM_FACTORY(LVL1::SCEmulation)

DECLARE_ALGORITHM_FACTORY(LVL1::JGTowerMaker)
DECLARE_ALGORITHM_FACTORY(LVL1::METNoiseCutPerfFex)
DECLARE_ALGORITHM_FACTORY(LVL1::JGTowerBuilder)
DECLARE_ALGORITHM_FACTORY(LVL1::JTowerRhoSubtractionAlg)
DECLARE_ALGORITHM_FACTORY(LVL1::GTowersFromGCaloTowers)
DECLARE_ALGORITHM_FACTORY(LVL1::GTowerRhoSubtractionAlg)
DECLARE_ALGORITHM_FACTORY(LVL1::JGTowerNoiseAlg)
DECLARE_ALGORITHM_FACTORY(LVL1::GBlockBuilder)
DECLARE_ALGORITHM_FACTORY(LVL1::METJWoJPerfFex)
DECLARE_TOOL_FACTORY(LVL1::JTowerMappingMaker)
DECLARE_TOOL_FACTORY(LVL1::GTowerMappingMaker)
// eFEX
DECLARE_ALGORITHM_FACTORY(LVL1::EFexEMAlgorithm)
DECLARE_ALGORITHM_FACTORY(LVL1::EFexTauAlgorithm)
DECLARE_TOOL_FACTORY(LVL1::EFexEMEnergyWeightedClusterTool)
DECLARE_TOOL_FACTORY(LVL1::EFexEMClusterTool)

DECLARE_FACTORY_ENTRIES(TrigT1CaloFexPerf)
{
  DECLARE_ALGORITHM(LVL1::JGTowerMaker)
  DECLARE_ALGORITHM(LVL1::METNoiseCutPerfFex)
  DECLARE_ALGORITHM(LVL1::JGTowerBuilder)
  DECLARE_ALGORITHM(LVL1::JTowerRhoSubtractionAlg)
  DECLARE_ALGORITHM(LVL1::GTowersFromGCaloTowers)
  DECLARE_ALGORITHM(LVL1::GTowerRhoSubtractionAlg)
  DECLARE_ALGORITHM(LVL1::JGTowerNoiseAlg)
  DECLARE_ALGORITHM(LVL1::GBlockBuilder)
  DECLARE_ALGORITHM(LVL1::METJWoJPerfFex)
  DECLARE_TOOL(LVL1::JTowerMappingMaker)
  DECLARE_TOOL(LVL1::GTowerMappingMaker)
  // eFEX
  DECLARE_ALGORITHM(LVL1::EFexEMAlgorithm);
  DECLARE_ALGORITHM(LVL1::EFexTauAlgorithm);
  DECLARE_TOOL(LVL1::EFexEMEnergyWeightedClusterTool)
  DECLARE_TOOL(LVL1::EFexEMClusterTool)
  DECLARE_ALGORITHM(LVL1::SCEmulation)
}
