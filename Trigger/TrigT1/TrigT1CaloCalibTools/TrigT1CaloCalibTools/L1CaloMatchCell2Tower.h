// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrigT1CaloCalibTools/L1CaloMatchCell2Tower.h
 * @author scott snyder <snyder@bnl.gov>, from existing code.
 * @date Oct, 2021
 * @brief Map from calo cells to trigger towers.
 *
 * Factored out from L1CaloCells2TriggerTowers:
 *  AlgTool that implement several methods
 *  used to produced the On/Off and On/Attr
 *  trigger tower Id maps.
 *
 *   @author Damien Prieur <damien.prieur@cern.ch>
 *   @author John Morris <john.morris@cern.ch>
 */


#ifndef TRIGT1CALOCALIBTOOLS_L1CALOMATCHCELL2TOWER_H
#define TRIGT1CALOCALIBTOOLS_L1CALOMATCHCELL2TOWER_H


#include "TrigT1CaloCalibToolInterfaces/IL1CaloMatchCell2Tower.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
class CaloIdManager;
class TileID;
class CaloLVL1_ID;
class TileCablingService;


namespace LVL1 {


class L1CaloMatchCell2Tower
  : public extends<AthAlgTool, IL1CaloMatchCell2Tower>
{
public:
  using base_class::base_class;

  virtual StatusCode initialize() override;

  virtual void matchCell2Tower (const CaloCell* caloCell,
                                Identifier& ttId1,
                                Identifier& ttId2) const override;
  virtual void matchCell2Tower (const LArOnOffIdMapping& cabling,
                                const CaloCell* caloCell,
                                Identifier& ttId1,
                                Identifier& ttId2) const override;


private:
  const CaloIdManager* m_caloMgr = nullptr;
  const TileID* m_tileID = nullptr;
  const CaloLVL1_ID* m_lvl1Helper = nullptr;
  const TileCablingService* m_tileCablingService = nullptr;

  ToolHandle<CaloTriggerTowerService> m_ttSvc
    { this, "CaloTriggerTowerService", "CaloTriggerTowerService", "CaloTriggerTowerService" };

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey
  { this, "CablingKey", "LArOnOffIdMap", "SG Key of LArOnOffIdMapping object" };
};


} // namespace LVL1


#endif // not TRIGT1CALOCALIBTOOLS_L1CALOMATCHCELL2TOWER_H
