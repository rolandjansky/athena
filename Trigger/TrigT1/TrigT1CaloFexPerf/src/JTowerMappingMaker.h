/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_JTOWERMAPPINGMAKER_H
#define TRIGT1CALOFEXPERF_JTOWERMAPPINGMAKER_H

#include "JGTowerMappingMakerBase.h"
#include "CaloIdentifier/JTower_ID.h"

namespace LVL1
{
  class JTowerMappingMaker : public JGTowerMappingMakerBase
  {
  public:
    JTowerMappingMaker(const std::string &type, const std::string &name, const IInterface *parent);
    virtual ~JTowerMappingMaker() override;

    virtual StatusCode initialize() override;

  private:
    virtual StatusCode buildForwardMapping() override;
    virtual StatusCode loadTowerAreas() override;
    const JTower_ID *m_jTowerId{nullptr};

    std::string m_towerAreasFile;

  }; //> end class JTowerMappingMaker
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_JTOWERMAPPINGMAKER_H