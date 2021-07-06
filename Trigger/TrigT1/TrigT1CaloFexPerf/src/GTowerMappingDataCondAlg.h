/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_GTOWERMAPPINGDATACONDALG_H
#define TRIGT1CALOFEXPERF_GTOWERMAPPINGDATACONDALG_H

#include "JGTowerMappingDataCondAlgBase.h"
#include "CaloIdentifier/GTower_ID.h"

namespace LVL1
{
  class GTowerMappingDataCondAlg : public JGTowerMappingDataCondAlgBase
  {
  public:
    GTowerMappingDataCondAlg(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~GTowerMappingDataCondAlg() override;

    virtual StatusCode initialize() override;

  private:
    const GTower_ID *m_gtowerID{nullptr};
    virtual const JGTowerBase_ID *getJGTowerID() const override { return m_gtowerID; }
    virtual StatusCode buildForwardMapping(
        JGTowerMappingData &data,
        const CaloSuperCellDetDescrManager *mgr) const override;
    virtual StatusCode loadTowerAreas(JGTowerMappingData &data) const override;
  }; //> end class GTowerMappingDataCondAlg
} // namespace LVL1
#endif