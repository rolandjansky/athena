/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file L1CaloD3PDMaker/L1CaloPPMdbMCFillerTool.cxx
 * @author John Morris <john.morris@cern.ch>
 * @brief Block filler tool for L1Calo PPM Database information.
 */

#include "AthenaKernel/errorcheck.h"
#include "L1CaloPPMdbMCFillerTool.h"

#include <iostream>
using std::cout;
using std::endl;

namespace D3PD{

 /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
  */
  L1CaloPPMdbMCFillerTool::L1CaloPPMdbMCFillerTool (const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent)
  : BlockFillerTool<void> (type, name, parent),
  m_tools("LVL1::L1CaloOfflineTriggerTowerTools/L1CaloOfflineTriggerTowerTools"), 
  m_triggerTowerLocation("TriggerTowers"),
  m_caloCellsLocation("AllCalo"),
  m_larDigitLocation("FREE"),
  m_tileDigitsLocation("TileDigitsCnt"),
  m_firstEvent(true)
  {
  declareProperty("TriggerTowerLocation", m_triggerTowerLocation);
  declareProperty("CaloCellsLocation", m_caloCellsLocation);
  declareProperty("LArDigitLocatrion", m_larDigitLocation);
  declareProperty("TileDigitsLocation",m_tileDigitsLocation);

  book().ignore(); // Avoid coverity warnings.
  }


  /**
   * @brief Standard Gaudi initialize method.
   */
  StatusCode L1CaloPPMdbMCFillerTool::initialize()
  {
    CHECK( m_tools.retrieve() );
    m_firstEvent = true;
    return StatusCode::SUCCESS;
  }


  StatusCode L1CaloPPMdbMCFillerTool::book()
  {
    CHECK( addVariable ("eta", m_eta) );
    CHECK( addVariable ("l1caloPhi", m_l1caloPhi) );
    CHECK( addVariable ("atlasPhi", m_atlasPhi) );

    CHECK( addVariable ("IsTile",m_isTile) );
    CHECK( addVariable ("NCaloCells",m_nCells) );
    CHECK( addVariable ("CellsLayerNames",m_cellsLayerNames) );
    CHECK( addVariable ("NCaloCellsByLayer",m_nCellsByLayer) );    

    CHECK( addVariable ("CoolId",m_coolId) );    


    return StatusCode::SUCCESS;
  }

  StatusCode L1CaloPPMdbMCFillerTool::fill(){
    if(m_firstEvent == true){

      StatusCode sc;
      
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
      
      const TriggerTowerCollection* ttCollection = 0;
      sc = evtStore()->retrieve(ttCollection,m_triggerTowerLocation);
      if(sc.isFailure()){ATH_MSG_INFO("Failed to load Trigger Towers");return sc;}

      int notTile(0);

      typedef TriggerTowerCollection::const_iterator Itr;
      typedef std::vector<const coral::AttributeList*>::const_iterator Itr_Rx;
      for(Itr i=ttCollection->begin();i!=ttCollection->end();++i){
        // EM
        m_coolId->push_back(m_tools->emCoolChannelId(*i));
        m_eta->push_back((*i)->eta());
        m_l1caloPhi->push_back((*i)->phi());
        m_atlasPhi->push_back((this->AtlasPhi((*i)->phi())));
        
        m_isTile->push_back(notTile);
        m_nCells->push_back(m_tools->emNCells(*i));
        m_cellsLayerNames->push_back(m_tools->emTTCellsLayerNames(*i));
        m_nCellsByLayer->push_back(m_tools->emNCellsByLayer(*i));        
        

        // Had
        m_coolId->push_back(m_tools->hadCoolChannelId(*i));
        m_eta->push_back((*i)->eta());
        m_l1caloPhi->push_back((*i)->phi());
        m_atlasPhi->push_back((this->AtlasPhi((*i)->phi())));         
        
        m_isTile->push_back(m_tools->hadIsTile(*i));
        m_nCells->push_back(m_tools->hadNCells(*i));
        m_cellsLayerNames->push_back(m_tools->hadTTCellsLayerNames(*i));
        m_nCellsByLayer->push_back(m_tools->hadNCellsByLayer(*i));

      }
      m_firstEvent = false;
    }    
    return StatusCode::SUCCESS;
  }

  float L1CaloPPMdbMCFillerTool::AtlasPhi(const float& phi){
    float pi(3.14159265);
    if(phi >= 0.0 && phi < pi ){
      return phi;
    }
    if(phi >= pi){
      return phi - (2.0 * pi);
    }
    return phi;
  }  

}
