/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_JGTOWERBUILDER_H
#define TRIGT1CALOFEXPERF_JGTOWERBUILDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "IJGTowerMappingMaker.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

namespace LVL1
{
  class JGTowerBuilder : public AthAlgorithm
  {
  public:
    JGTowerBuilder(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~JGTowerBuilder() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

  private:
    SG::ReadHandleKey<CaloCellContainer> m_cellsKey;
    SG::ReadHandleKey<CaloCellContainer> m_superCellsKey;
    SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_triggerTowersKey;
    SG::WriteHandleKey<xAOD::JGTowerContainer> m_towersKey;
    ToolHandle<IJGTowerMappingMaker> m_mappingMaker;
    bool m_useAllCalo;
    bool m_useAllRun2TT;
    bool m_emulateSCTiming;
    bool m_useSCQuality;
    uint16_t m_scQuality;
    float m_minSCETp;
    float m_maxSCETm;
    float m_minTowerEt;

    // Internals
    const CaloCell_SuperCell_ID *m_scid;
  }; //> end class JGTowerBuilder
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_JGTOWERBUILDER_H