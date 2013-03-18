/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file L1CaloD3PDMaker/L1CaloProblemChannelFillerTool.cxx
 * @author John Morris <john.morris@cern.ch>
 * @date August 2010
 * @brief Block filler tool for L1Calo Problem Channel information.
 */

#include "AthenaKernel/errorcheck.h"
#include "L1CaloProblemChannelFillerTool.h"

#include <iostream>

namespace D3PD{

 /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
  */
  L1CaloProblemChannelFillerTool::L1CaloProblemChannelFillerTool (const std::string& type,
                                                                  const std::string& name,
                                                                  const IInterface* parent)
    : BlockFillerTool<void> (type, name, parent),
      m_tools("LVL1::L1CaloOfflineTriggerTowerTools/L1CaloOfflineTriggerTowerTools"),  
      m_triggerTowerLocation("TriggerTowers"),
      m_caloCellsLocation("AllCalo"),
      m_larDigitLocation("FREE"),
      m_tileDigitsLocation("TileDigitsCnt"),
      m_tileTTL1ContainerName("TileCellTTL1Container"),
      m_dbPpmDisabledTowers("/TRIGGER/L1Calo/V1/Conditions/DisabledTowers"),     
      m_currentLumiBlock(-999)    
  {
    declareProperty("TriggerTowerLocation", m_triggerTowerLocation);
    declareProperty("CaloCellsLocation", m_caloCellsLocation);
    declareProperty("LArDigitLocatrion", m_larDigitLocation);
    declareProperty("TileDigitsLocation",m_tileDigitsLocation);
    declareProperty("TileTTL1ContainerName",m_tileTTL1ContainerName);
    declareProperty("DbPpmDisabledTowers",m_dbPpmDisabledTowers);

    book().ignore(); // Avoid coverity warnings.
  }


  /**
   * @brief Standard Gaudi initialize method.
   */
  StatusCode L1CaloProblemChannelFillerTool::initialize()
  {
    CHECK( m_tools.retrieve() );
    return StatusCode::SUCCESS;
  }


  StatusCode L1CaloProblemChannelFillerTool::book()
  {
    CHECK( addVariable ("RunNumber", m_runNumber) );
    CHECK( addVariable ("LumiBlock", m_lumiBlock) );
    CHECK( addVariable ("emBadCalo", m_emBadCalo) );
    CHECK( addVariable ("emDisabled", m_emDisabled) );
    CHECK( addVariable ("hadBadCalo", m_hadBadCalo) );
    CHECK( addVariable ("hadDisabled", m_hadDisabled) );

    return StatusCode::SUCCESS;
  }

  StatusCode L1CaloProblemChannelFillerTool::fill()
  {
    StatusCode sc;

    sc = evtStore()->retrieve(m_eventInfo);
    if(sc.isFailure()){return sc;}

    // Only run on new Lumi Blocks
    int lumiBlock = m_eventInfo->event_ID()->lumi_block();
    if(m_currentLumiBlock != lumiBlock){
      m_currentLumiBlock = lumiBlock;

      *m_runNumber = m_eventInfo->event_ID()->run_number();
      *m_lumiBlock = m_currentLumiBlock;

      const CaloCellContainer* cellContainer = 0;
      sc = evtStore()->retrieve(cellContainer,m_caloCellsLocation);
      if(sc.isFailure()){ATH_MSG_INFO("Failed to load Calo Cells");return sc;}
      m_tools->caloCells(cellContainer);
      
      if(evtStore()->contains<LArDigitContainer>(m_larDigitLocation)) {
        const LArDigitContainer* larDigitContainer = 0;
        sc = evtStore()->retrieve(larDigitContainer,m_larDigitLocation);
        if(sc.isFailure()){ATH_MSG_INFO("Failed to load LAr Digits");return sc;}
        m_tools->larDigits(larDigitContainer);
      }
      
      if(evtStore()->contains<TileDigitsContainer>(m_tileDigitsLocation)) {
        const TileDigitsContainer* tileDigitsContainer = 0;
        sc = evtStore()->retrieve(tileDigitsContainer,m_tileDigitsLocation);
        if(sc.isFailure()){ATH_MSG_INFO("Failed to load Tile Digits");return sc;}
        m_tools->tileDigits(tileDigitsContainer);
      }
      
      const TileTTL1CellContainer* TTL1Cnt_cell = 0;
      sc = evtStore()->retrieve(TTL1Cnt_cell,m_tileTTL1ContainerName);
      if(sc.isFailure()){ATH_MSG_INFO("Failed to load TileTTL1");return sc;}
      this->preCacheTTL1Cell(TTL1Cnt_cell);
      
      const TriggerTowerCollection* ttCollection = 0;
      sc = evtStore()->retrieve(ttCollection,m_triggerTowerLocation);
      if(sc.isFailure()){ATH_MSG_INFO("Failed to load Trigger Towers");return sc;}

      const CondAttrListCollection* attrDisabled = 0;
      sc = evtStore()->retrieve(attrDisabled,m_dbPpmDisabledTowers);
      if(sc.isFailure()){return sc;}


      typedef TriggerTowerCollection::const_iterator Itr;

      for(Itr i=ttCollection->begin();i!=ttCollection->end();++i){

        unsigned int emCoolId = m_tools->emCoolChannelId(*i);
        unsigned int hadCoolId = m_tools->hadCoolChannelId(*i);

        const coral::AttributeList* emDbDisabled = m_tools->emDbAttributes(*i,attrDisabled);
        const coral::AttributeList* hadDbDisabled = m_tools->hadDbAttributes(*i,attrDisabled);

        // Dead Channels
        unsigned int emDisabled = m_tools->DisabledTower(emDbDisabled);
        unsigned int hadDisabled = m_tools->DisabledTower(hadDbDisabled);

        if(emDisabled != 0){
          m_emDisabled->push_back(emCoolId);
        }
        if(hadDisabled != 0){
          m_hadDisabled->push_back(hadCoolId);
        }

        // Channels with Bad Calo Cells
        int emBadCalo = m_tools->emBadCalo(*i);
        int hadBadCalo = m_tools->hadBadCalo(*i,m_idTTL1CellMap);

        if(emBadCalo != 0){
          m_emBadCalo->push_back(emCoolId);
        }
        if(hadBadCalo != 0){
          m_hadBadCalo->push_back(hadCoolId);
        }

      }
    }

    return StatusCode::SUCCESS;
  }

  void L1CaloProblemChannelFillerTool::preCacheTTL1Cell(const TileTTL1CellContainer* cont) {
    if(!cont) return; // should be checked earlier to give an error message

    TileTTL1CellContainer::const_iterator it(cont->begin());
    TileTTL1CellContainer::const_iterator itE(cont->end());

    m_idTTL1CellMap.clear();

    for(;it != itE; ++it) {
      m_idTTL1CellMap.insert(IdTTL1CellMapType::value_type((*it)->TTL1_ID(), *it));
    }
  }  
  

}


    
