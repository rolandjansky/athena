/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_JJETBUILDER_H
#define TRIGT1CALOFEXPERF_JJETBUILDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "IJGTowerMappingMaker.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "JJetSeedHelper.h"
#include <vector>

namespace LVL1
{
  class JJetBuilder : public AthAlgorithm
  {
  public:
    JJetBuilder(const std::string &name, ISvcLocator *pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual float passNoiseCutET(const xAOD::JGTower *tower); 

  private:
    SG::ReadHandleKey<xAOD::JGTowerContainer> m_jtowersKey;
    ToolHandle<IJGTowerMappingMaker> m_mappingMaker;
    SG::WriteHandleKey<xAOD::JetRoIContainer> m_outputSmallJetContainerKey;
    SG::WriteHandleKey<xAOD::JetRoIContainer> m_outputLargeJetContainerKey;
    SG::WriteHandleKey<xAOD::JetRoIContainer> m_outputSeedJetContainerKey;
    // Internals
    std::vector<JJetSeedHelper> m_seedHelpers;
    
    std::string m_noiseName;

    float m_jRoundJetMinET; 
    float m_jRoundJetSeedMinET; 
    
    std::unique_ptr<SG::AuxElement::ConstAccessor<float>> m_noiseAcc;
  }; //> end class JJetBuilder
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_JJETBUILDER_H
