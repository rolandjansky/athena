/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file L1CaloD3PDMaker/L1CaloPPMFillerTool.cxx
 * @author John Morris <john.morris@cern.ch>
 * @date August 2010
 * @brief Block filler tool for L1Calo PPM information.
 */

#include "AthenaKernel/errorcheck.h"
#include "L1CaloPPMFillerTool.h"

#include <iostream>

namespace D3PD{

 /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
  */
  L1CaloPPMFillerTool::L1CaloPPMFillerTool (const std::string& type,
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
      m_dbPpmDeadChannels("/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels"),      
      m_isCalibration(false),
      m_minCaloCellEnergy(0.05),
      m_minADC(30),
      m_useRandom(false),
      m_minRandom(0.03),
      m_isMC(false),
      m_toolsMode(1),
      m_random(0)
  {
    declareProperty("TriggerTowerLocation", m_triggerTowerLocation);
    declareProperty("CaloCellsLocation", m_caloCellsLocation);
    declareProperty("LArDigitLocatrion", m_larDigitLocation);
    declareProperty("TileDigitsLocation",m_tileDigitsLocation);
    declareProperty("TileTTL1ContainerName",m_tileTTL1ContainerName);
    declareProperty("DbPpmDisabledTowers",m_dbPpmDisabledTowers);
    declareProperty("DbPpmDeadChannels",m_dbPpmDeadChannels);
    declareProperty("DoCalibration",m_isCalibration);
    declareProperty("MinCaloCellEnergy",m_minCaloCellEnergy);
    declareProperty("MinADC",m_minADC);
    declareProperty("UseRandom",m_useRandom);
    declareProperty("MinRandom",m_minRandom);
    declareProperty("IsMC",m_isMC);

    book().ignore(); // Avoid coverity warnings.
    m_em_larTowerEnergy = 0;
    m_had_larTowerEnergy = 0;
  }


  /**
   * @brief Standard Gaudi initialize method.
   */
  StatusCode L1CaloPPMFillerTool::initialize()
  {
    CHECK( m_tools.retrieve() );

    // No random number for calibration
    if (m_isCalibration)
      m_useRandom = false;

    // Random number generator
    if (m_useRandom){
      m_random = new TRandom3(0);
    }

    return StatusCode::SUCCESS;
  }


  /**
   * @brief Standard Gaudi finalize method.
   */
  StatusCode L1CaloPPMFillerTool::finalize()
  {
    delete m_random;
    m_random = 0;
    return StatusCode::SUCCESS;
  }


  StatusCode L1CaloPPMFillerTool::book()
  {
    CHECK( addVariable ("emCoolId", m_em_coolId) );
    CHECK( addVariable ("emTTCellsEnergy", m_em_ttCellsEnergy) );
    CHECK( addVariable ("emTTCellsEt", m_em_ttCellsEt) );
    CHECK( addVariable ("emTTCellsEnergyByLayer", m_em_ttCellsEnergyByLayer) );
    CHECK( addVariable ("emTTCellsEtByLayer", m_em_ttCellsEtByLayer) );
    CHECK( addVariable ("emTTCellsEnergyByReceiver", m_em_ttCellsEnergyByReceiver) );
    CHECK( addVariable ("emTTCellsEtByReceiver", m_em_ttCellsEtByReceiver) );
    CHECK( addVariable ("emPeak", m_em_Peak) );
    CHECK( addVariable ("emADCPeak", m_em_ADCPeak) );
    CHECK( addVariable ("emLUT", m_em_LUT) );
    CHECK( addVariable ("emADC", m_em_ADC) );
    CHECK( addVariable ("emEnergy", m_em_Energy) );
    CHECK( addVariable ("emBCIDvec", m_em_BCIDvec) );
    CHECK( addVariable ("emBCIDext", m_em_BCIDext) );
    CHECK( addVariable ("emError", m_em_Error) );
    CHECK( addVariable ("emBCID", m_em_BCID) );
    CHECK( addVariable ("emIsSaturated", m_em_IsSaturated) );
    CHECK( addVariable ("emBadCalo",m_em_badCalo) );
    CHECK( addVariable ("emCaloQuality",m_em_caloQuality) );
    if(m_isMC == false){
      CHECK( addVariable ("emDisabled",m_em_disabledTower) );
      CHECK( addVariable ("emDead",m_em_deadChannel) );
    }

    CHECK( addVariable ("hadCoolId", m_had_coolId) );
    CHECK( addVariable ("hadTTCellsEnergy", m_had_ttCellsEnergy) );
    CHECK( addVariable ("hadTTCellsEt", m_had_ttCellsEt) );
    CHECK( addVariable ("hadTileCellsEnergy", m_had_TileCellsEnergy) );
    CHECK( addVariable ("hadTTCellsEnergyByLayer", m_had_ttCellsEnergyByLayer) );
    CHECK( addVariable ("hadTTCellsEtByLayer", m_had_ttCellsEtByLayer) );
    CHECK( addVariable ("hadTTCellsEnergyByReceiver", m_had_ttCellsEnergyByReceiver) );
    CHECK( addVariable ("hadTTCellsEtByReceiver", m_had_ttCellsEtByReceiver) );
    CHECK( addVariable ("hadPeak", m_had_Peak) );
    CHECK( addVariable ("hadADCPeak", m_had_ADCPeak) );
    CHECK( addVariable ("hadLUT", m_had_LUT) );
    CHECK( addVariable ("hadADC", m_had_ADC) );
    CHECK( addVariable ("hadEnergy", m_had_Energy) );
    CHECK( addVariable ("hadBCIDvec", m_had_BCIDvec) );
    CHECK( addVariable ("hadBCIDext", m_had_BCIDext) );
    CHECK( addVariable ("hadError", m_had_Error) );
    CHECK( addVariable ("hadBCID", m_had_BCID) );
    CHECK( addVariable ("hadIsSaturated", m_had_IsSaturated) );
    CHECK( addVariable ("hadBadCalo",m_had_badCalo) );
    CHECK( addVariable ("hadCaloQuality",m_had_caloQuality) );
    if(m_isMC == false){
      CHECK( addVariable ("hadDisabled",m_had_disabledTower) );
      CHECK( addVariable ("hadDead",m_had_deadChannel) );
    }
    
    // dead feb corrected tower cell energy
    if(m_isCalibration) {
      CHECK( addVariable("emEtLArg", m_em_larTowerEnergy) );
      CHECK( addVariable("hadEtLArg", m_had_larTowerEnergy) );
    }

    return StatusCode::SUCCESS;
  }

  StatusCode L1CaloPPMFillerTool::fill()
  {
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

    const TileTTL1CellContainer* TTL1Cnt_cell = 0;
    sc = evtStore()->retrieve(TTL1Cnt_cell,m_tileTTL1ContainerName);
    if(sc.isFailure()){ATH_MSG_INFO("Failed to load TileTTL1");return sc;}
    this->preCacheTTL1Cell(TTL1Cnt_cell);

    const TriggerTowerCollection* ttCollection = 0;
    sc = evtStore()->retrieve(ttCollection,m_triggerTowerLocation);
    if(sc.isFailure()){ATH_MSG_INFO("Failed to load Trigger Towers");return sc;}

    const CondAttrListCollection* attrDisabled = 0;
    if(m_isMC == false){
      sc = evtStore()->retrieve(attrDisabled,m_dbPpmDisabledTowers);
      if(sc.isFailure()){return sc;}
    }

    const CondAttrListCollection* attrDead = 0;
    if(m_isMC == false){
      sc = evtStore()->retrieve(attrDead,m_dbPpmDeadChannels);
      if(sc.isFailure()){return sc;}
    }


    typedef TriggerTowerCollection::const_iterator Itr;

    // Should we save the trigger tower or not???
    // If this is calibration data, then yes - save the lot
    // Randomly save all trigger towers m_minRandom % of the time.
    //
    // If this is collisions data then only save if:
    // 1.) Any ADC value is greater than m_minADC  OR
    // 2.) The CaloCell energy is greater than m_minCaloCellEnergy

    bool globalSaveMe(false);
    // Test for calibration
    if(m_isCalibration == true){globalSaveMe = true;}

    // Random number save all
    if(m_useRandom == true){
      if(globalSaveMe == false){
        if(m_random->Rndm() < m_minRandom){
          globalSaveMe = true;
        }
      }
    }

    for(Itr i=ttCollection->begin();i!=ttCollection->end();++i){

      bool saveMe(false);
      if(globalSaveMe == true){saveMe = true;}

      // Test for EM Calo Energy
      if(saveMe == false){
        if(m_tools->emTTCellsEnergy((*i)) > m_minCaloCellEnergy){
          saveMe = true;
        }
      }

      // Test for Had Calo Energy
      if(saveMe == false){
        if(m_tools->hadTTCellsEnergy((*i)) > m_minCaloCellEnergy){
          saveMe = true;
        }
      }

      // Test for EM ADC values
      typedef std::vector<int>::iterator Itr_tt;
      if(saveMe == false){
        std::vector<int> adc = (*i)->emADC();
        for(Itr_tt j=adc.begin();j!=adc.end();++j){
          if( (*j) > m_minADC){
            saveMe = true;
            break;
          }
        }
      }

      // Test for Had ADC values
      if(saveMe == false){
        std::vector<int> adc = (*i)->hadADC();
        for(Itr_tt j=adc.begin();j!=adc.end();++j){
          if( (*j) > m_minADC){
            saveMe = true;
            break;
          }
        }
      }

      if(saveMe == true){

        // EM
        m_em_coolId->push_back(m_tools->emCoolChannelId(*i));
        m_em_ttCellsEnergy->push_back(m_tools->emTTCellsEnergy((*i)));
        m_em_ttCellsEt->push_back(m_tools->emTTCellsEt((*i)));
        m_em_ttCellsEnergyByLayer->push_back(m_tools->emTTCellsEnergyByLayer((*i)));
        m_em_ttCellsEtByLayer->push_back(m_tools->emTTCellsEtByLayer((*i)));
        m_em_ttCellsEnergyByReceiver->push_back(m_tools->emTTCellsEnergyByReceiver(*i,m_toolsMode));
        m_em_ttCellsEtByReceiver->push_back(m_tools->emTTCellsEtByReceiver(*i,m_toolsMode));
        m_em_Peak->push_back((*i)->emPeak());
        m_em_ADCPeak->push_back((*i)->emADCPeak());
        m_em_LUT->push_back((*i)->emLUT());
        m_em_ADC->push_back((*i)->emADC());
        m_em_Energy->push_back((*i)->emEnergy());
        m_em_BCIDvec->push_back((*i)->emBCIDvec());
        m_em_BCIDext->push_back((*i)->emBCIDext());
        m_em_Error->push_back((*i)->emError());
        m_em_BCID->push_back((*i)->emBCID());
        m_em_IsSaturated->push_back((*i)->isEMSaturated());

        if(m_isMC == false){
          const coral::AttributeList* emDbAttrDisabled = m_tools->emDbAttributes(*i,attrDisabled);
          m_em_disabledTower->push_back(m_tools->DisabledTower(emDbAttrDisabled));

          int emDead(0);
          const coral::AttributeList* emDbAttrDead = m_tools->emDbAttributes(*i,attrDead);
          if(emDbAttrDead != 0){
            emDead = m_tools->DeadChannel(emDbAttrDead);
          }
          m_em_deadChannel->push_back(emDead);
        }
        
        m_em_badCalo->push_back(m_tools->emBadCalo(*i));
        m_em_caloQuality->push_back(m_tools->emCaloQuality(*i));

        if(m_isCalibration){
          m_em_larTowerEnergy->push_back(m_tools->emLArTowerEnergy(*i));
        }


        // Had
        m_had_coolId->push_back(m_tools->hadCoolChannelId(*i));
        m_had_ttCellsEnergy->push_back(m_tools->hadTTCellsEnergy((*i)));
        m_had_ttCellsEt->push_back(m_tools->hadTTCellsEt((*i)));
        m_had_TileCellsEnergy->push_back(m_tools->tileCellEnergy((*i),m_idTTL1CellMap));
        m_had_ttCellsEnergyByLayer->push_back(m_tools->hadTTCellsEnergyByLayer((*i)));
        m_had_ttCellsEtByLayer->push_back(m_tools->hadTTCellsEtByLayer((*i)));
        m_had_ttCellsEnergyByReceiver->push_back(m_tools->hadTTCellsEnergyByReceiver(*i,m_toolsMode));
        m_had_ttCellsEtByReceiver->push_back(m_tools->hadTTCellsEtByReceiver(*i,m_toolsMode));
        m_had_Peak->push_back((*i)->hadPeak());
        m_had_ADCPeak->push_back((*i)->hadADCPeak());
        m_had_LUT->push_back((*i)->hadLUT());
        m_had_ADC->push_back((*i)->hadADC());
        m_had_Energy->push_back((*i)->hadEnergy());
        m_had_BCIDvec->push_back((*i)->hadBCIDvec());
        m_had_BCIDext->push_back((*i)->hadBCIDext());
        m_had_Error->push_back((*i)->hadError());
        m_had_BCID->push_back((*i)->hadBCID());
        m_had_IsSaturated->push_back((*i)->isHadSaturated());

        if(m_isMC == false){
          const coral::AttributeList* hadDbAttrDisabled = m_tools->hadDbAttributes(*i,attrDisabled);
          m_had_disabledTower->push_back(m_tools->DisabledTower(hadDbAttrDisabled));

          int hadDead(0);
          const coral::AttributeList* hadDbAttrDead = m_tools->hadDbAttributes(*i,attrDead);
          if(hadDbAttrDead != 0){
            hadDead = m_tools->DeadChannel(hadDbAttrDead);
          }
          m_had_deadChannel->push_back(hadDead);
        }
        
        m_had_badCalo->push_back(m_tools->hadBadCalo(*i,m_idTTL1CellMap));
        m_had_caloQuality->push_back(m_tools->hadCaloQuality(*i));

        if(m_isCalibration){
          m_had_larTowerEnergy->push_back(m_tools->hadLArTowerEnergy(*i));
        }

      } // End of saveMe statement
    } // End of loop over trigger towers

    return StatusCode::SUCCESS;
  }

  void L1CaloPPMFillerTool::preCacheTTL1Cell(const TileTTL1CellContainer* cont) {
    if(!cont) return; // should be checked earlier to give an error message

    TileTTL1CellContainer::const_iterator it(cont->begin());
    TileTTL1CellContainer::const_iterator itE(cont->end());

    m_idTTL1CellMap.clear();

    for(;it != itE; ++it) {
      m_idTTL1CellMap.insert(IdTTL1CellMapType::value_type((*it)->TTL1_ID(), *it));
    }
  }


}
