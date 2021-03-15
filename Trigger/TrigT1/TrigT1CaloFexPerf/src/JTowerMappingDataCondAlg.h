/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_JTOWERMAPPINGDATACONDALG_H
#define TRIGT1CALOFEXPERF_JTOWERMAPPINGDATACONDALG_H

#include "JGTowerMappingDataCondAlgBase.h"
#include "CaloIdentifier/JTower_ID.h"

namespace LVL1
{
  class JTowerMappingDataCondAlg : public JGTowerMappingDataCondAlgBase
  {
  public:
    JTowerMappingDataCondAlg(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~JTowerMappingDataCondAlg() override;

    virtual StatusCode initialize() override;

  private:
    Gaudi::Property<std::string> m_towerAreasFile{
        this, "TowerAreasFile", "Run3L1CaloSimulation/Noise/jTowerCorrection.20210209.r11881.root"};
    const JTower_ID *m_jtowerID{nullptr};
    virtual const JGTowerBase_ID *getJGTowerID() const override { return m_jtowerID; }
    virtual StatusCode buildForwardMapping(
        JGTowerMappingData &data,
        const CaloSuperCellDetDescrManager *mgr) const override;
    virtual StatusCode loadTowerAreas(JGTowerMappingData &data) const override;
    std::vector<float> m_towerAreas;
  }; //> end class JTowerMappingDatsaCondAlg
} // namespace LVL1
#endif