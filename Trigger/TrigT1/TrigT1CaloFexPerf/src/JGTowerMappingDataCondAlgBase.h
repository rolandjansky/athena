/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_JGTOWERMAPPINGDATACONDALGBASE_H
#define TRIGT1CALOFEXPERF_JGTOWERMAPPINGDATACONDALGBASE_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "TrigT1CaloFexPerf/JGTowerMappingData.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/JGTowerBase_ID.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ICondSvc.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

namespace LVL1
{
  class JGTowerMappingDataCondAlgBase : public AthReentrantAlgorithm
  {
  public:
    JGTowerMappingDataCondAlgBase(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~JGTowerMappingDataCondAlgBase() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext &ctx) const final;

  private:
    ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
    SG::WriteCondHandleKey<JGTowerMappingData> m_outputKey{
        this, "MappingData", ""};
    Gaudi::Property<bool> m_mapTileCells{this, "MapTileCells", false};

  protected:
    virtual const JGTowerBase_ID *getJGTowerID() const = 0;
    virtual StatusCode buildForwardMapping(
        JGTowerMappingData &data,
        const CaloSuperCellDetDescrManager *mgr) const = 0;
    virtual StatusCode loadTowerAreas(JGTowerMappingData &data) const = 0;
    const CaloCell_SuperCell_ID *m_scid{nullptr};
    const CaloCell_ID *m_ccid{nullptr};
  }; //> end class AthReentrantAlgorithm
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_JGTOWERMAPPINGDATACONDALGBASE_H