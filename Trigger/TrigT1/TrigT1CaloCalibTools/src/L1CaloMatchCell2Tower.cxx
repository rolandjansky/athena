/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrigT1CaloCalibTools/src/L1CaloMatchCell2Tower.cxx
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

#include "TrigT1CaloCalibTools/L1CaloMatchCell2Tower.h"
#include "TileConditions/TileCablingService.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "Identifier/Identifier.h"
#include "StoreGate/ReadCondHandle.h"


using namespace LVL1;


StatusCode L1CaloMatchCell2Tower::initialize()
{
  ATH_CHECK( detStore()->retrieve (m_caloMgr) );
  ATH_CHECK( detStore()->retrieve (m_tileID, "TileID") );
  ATH_CHECK( detStore()->retrieve (m_lvl1Helper, "CaloLVL1_ID") );
  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_ttSvc.retrieve() );
  m_tileCablingService = TileCablingService::getInstance();
  return StatusCode::SUCCESS;
}
                                             

void L1CaloMatchCell2Tower::matchCell2Tower (const CaloCell* caloCell,
                                             Identifier& ttId1,
                                             Identifier& ttId2) const
{
  SG::ReadCondHandle<LArOnOffIdMapping> cabling (m_cablingKey);
  matchCell2Tower (**cabling, caloCell, ttId1, ttId2);
}


void L1CaloMatchCell2Tower::matchCell2Tower(const LArOnOffIdMapping& cabling,
                                            const CaloCell* caloCell,
                                            Identifier& ttId1,
                                            Identifier& ttId2) const
{
  const Identifier invalidId(0);
  ttId1 = invalidId;
  ttId2 = invalidId;
  const Identifier cellId(caloCell->ID());

  const CaloCell_ID* caloCellHelper = m_caloMgr->getCaloCell_ID();

  // Tile cells
  if (caloCellHelper->is_tile(cellId)) {

    int section = caloCellHelper->section(cellId);
    int sample  = caloCellHelper->sample(cellId);

    // gap detector - D4 & C10 cells
    // both pmt of D4 belong to the same TT
    if(section==3 && (sample==1 || sample==2) ) {
      ttId1 = m_tileCablingService->cell2tt_id(cellId);

      // barrel & ext. barrel - D cells
    } else if( (section==1 || section==2) && sample==2) {

      // special treatment of D-cells
      // each pmt is affected to a different trigger tower
      // each D-CaloCell is then added to 2 TT. This must ne taken into account when computing the TT energy
      // from the CaloCells by dividing the D-cell energy by 2.
      ttId1 = m_tileCablingService->pmt2tt_id(m_tileID->pmt_id(cellId, 0));
      ttId2 = m_tileCablingService->pmt2tt_id(m_tileID->pmt_id(cellId, 1));

      // barrel & ext. barrel - A & BC cells
    } else if( (section==1 || section==2) && (sample==0 || sample==1) ) {
      ttId1 = m_tileCablingService->cell2tt_id(cellId);
    }

    //LArg cells
  } else {
    //whichTTID ttId returns a layer_id, not a tower_id !
    const Identifier layerId(m_ttSvc->whichTTID(cellId));

    // We are not calling here m_ttSvc->is_in_lvl1(cellId) but use directly its code for a performance reason
    // By doing so we save a redundant and costly call to m_ttSvc->whichTTID()
    bool lvl1(true);
    if(caloCellHelper->is_em_barrel(cellId)) {
      int samp=caloCellHelper->sampling(cellId);
      if(samp==0) {
        int eta=caloCellHelper->eta(cellId);
        if(eta==60) {
          lvl1 = false ;
        }
      }
    } else {
      if(m_lvl1Helper->is_hec(layerId)) {
        int layer(m_lvl1Helper->layer(layerId));
        if(layer == 3){
          lvl1 = false ;
        }
      }
    }

    if(lvl1) {
      // check if the channel is connected
      HWIdentifier channelID = cabling.createSignalChannelID(cellId);
      if(cabling.isOnlineConnected(channelID)) {

        // As we don't want the layer information embedded in the identifier, we recreate a tower Id
        ttId1 = m_lvl1Helper->tower_id( m_lvl1Helper->pos_neg_z(layerId), m_lvl1Helper->sampling(layerId), m_lvl1Helper->region(layerId), m_lvl1Helper->eta(layerId), m_lvl1Helper->phi(layerId));
      }
    }
  }
  
}
