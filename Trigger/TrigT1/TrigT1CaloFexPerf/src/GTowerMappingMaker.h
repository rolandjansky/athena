/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_GTOWERMAPPINGMAKER_H
#define TRIGT1CALOFEXPERF_GTOWERMAPPINGMAKER_H

#include "JGTowerMappingMakerBase.h"
#include "CaloIdentifier/GTower_ID.h"

namespace LVL1
{
  class GTowerMappingMaker : public JGTowerMappingMakerBase
  {
  public:
    GTowerMappingMaker(const std::string &type, const std::string &name, const IInterface *parent);
    virtual ~GTowerMappingMaker() override;

    virtual StatusCode initialize() override;

  private:
    virtual StatusCode buildForwardMapping() override;
    virtual StatusCode loadTowerAreas() override;
    const GTower_ID *m_gTowerId{nullptr};

  }; //> end class JGTowerMappingMaker
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_GTOWERMAPPINGMAKER_H