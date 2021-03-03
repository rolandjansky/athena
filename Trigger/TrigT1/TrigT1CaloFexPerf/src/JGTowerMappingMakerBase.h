/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_JGTOWERMAPPINGMAKERBASE_H
#define TRIGT1CALOFEXPERF_JGTOWERMAPPINGMAKERBASE_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "IJGTowerMappingMaker.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/JGTowerBase_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

namespace LVL1
{
  class JGTowerMappingMakerBase : virtual public IJGTowerMappingMaker, public AthAlgTool
  {
  public:
    JGTowerMappingMakerBase(const std::string &type, const std::string &name, const IInterface *parent);
    virtual ~JGTowerMappingMakerBase() override;

    virtual StatusCode initialize() override;
    virtual const std::vector<JGTowerHelper> &getMapping() const override;

  protected:
    bool m_mapTileCells;

    std::vector<JGTowerHelper> m_mapping;
    StatusCode buildMapping(const JGTowerBase_ID *jgTowerID);
    virtual StatusCode buildForwardMapping() = 0;
    virtual StatusCode loadTowerAreas() = 0;
    const DataHandle<CaloSuperCellDetDescrManager> m_scDetMgr;
    const CaloCell_SuperCell_ID *m_scid{nullptr};
    const CaloCell_ID *m_ccid{nullptr};
  }; //> end class JGTowerMappingMakerBase
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_JGTOWERMAPPINGMAKERBASE_H