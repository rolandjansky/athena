/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file L1CaloD3PDMaker/L1CaloPPMdbDataFillerTool.cxx
 * @author John Morris <john.morris@cern.ch>
 * @brief Block filler tool for L1Calo PPM Database information.
 */

#include "AthenaKernel/errorcheck.h"
#include "L1CaloPPMdbDataFillerTool.h"

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
  L1CaloPPMdbDataFillerTool::L1CaloPPMdbDataFillerTool (const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent)
  : BlockFillerTool<void> (type, name, parent),
  m_tools("LVL1::L1CaloOfflineTriggerTowerTools/L1CaloOfflineTriggerTowerTools"),
  m_LArHVCorrTool("LArHVCorrTool"),  
  m_triggerTowerLocation("TriggerTowers"),
  m_caloCellsLocation("AllCalo"),
  m_larDigitLocation("FREE"),
  m_tileDigitsLocation("TileDigitsCnt"),
  m_dbPpmChanCalib("/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib"),
  m_dbRxGains("/TRIGGER/Receivers/Conditions/VgaDac"),
  m_dbRxStatus("/TRIGGER/Receivers/Factors/CalibGains"),
  m_firstEvent(true)
  {
  declareProperty("TriggerTowerLocation", m_triggerTowerLocation);
  declareProperty("CaloCellsLocation", m_caloCellsLocation);
  declareProperty("LArDigitLocatrion", m_larDigitLocation);
  declareProperty("TileDigitsLocation",m_tileDigitsLocation);
  declareProperty("DbPpmChanCalib",m_dbPpmChanCalib);
  declareProperty("DbRxGains",m_dbRxGains);
  declareProperty("DbRxStatus",m_dbRxStatus);
  declareProperty("LArHVCorrTool",m_LArHVCorrTool);

  book().ignore(); // Avoid coverity warnings.
  }


  /**
   * @brief Standard Gaudi initialize method.
   */
  StatusCode L1CaloPPMdbDataFillerTool::initialize()
  {
    CHECK( m_tools.retrieve() );
    CHECK( m_LArHVCorrTool.retrieve() );
    m_tools->LArHV (m_LArHVCorrTool);
    m_firstEvent = true;
    return StatusCode::SUCCESS;
  }


  StatusCode L1CaloPPMdbDataFillerTool::book()
  {
    CHECK( addVariable ("eta", m_eta) );
    CHECK( addVariable ("l1caloPhi", m_l1caloPhi) );
    CHECK( addVariable ("atlasPhi", m_atlasPhi) );

    CHECK( addVariable ("IsTile",m_isTile) );
    CHECK( addVariable ("NCaloCells",m_nCells) );
    CHECK( addVariable ("CellsLayerNames",m_cellsLayerNames) );
    CHECK( addVariable ("NCaloCellsByLayer",m_nCellsByLayer) );

    CHECK( addVariable ("CoolId",m_coolId) );
    CHECK( addVariable ("DacOffset",m_DacOffset) );
    CHECK( addVariable ("DacSlope",m_DacSlope) );
    CHECK( addVariable ("PedMean",m_PedMean) );
    CHECK( addVariable ("PedValue",m_PedValue) );
    CHECK( addVariable ("FullDelayData",m_FullDelayData) );
    CHECK( addVariable ("SatBcidThreshLow",m_SatBcidThreshLow) );
    CHECK( addVariable ("SatBcidThreshHigh",m_SatBcidThreshHigh) );
    CHECK( addVariable ("SatBcidLevel",m_SatBcidLevel) );
    CHECK( addVariable ("BcidEnergyRangeLow",m_BcidEnergyRangeLow) );
    CHECK( addVariable ("BcidEnergyRangeHigh",m_BcidEnergyRangeHigh) );
    CHECK( addVariable ("FirStartBit",m_FirStartBit) );
    CHECK( addVariable ("FirCoeff1",m_FirCoeff1) );
    CHECK( addVariable ("FirCoeff2",m_FirCoeff2) );
    CHECK( addVariable ("FirCoeff3",m_FirCoeff3) );
    CHECK( addVariable ("FirCoeff4",m_FirCoeff4) );
    CHECK( addVariable ("FirCoeff5",m_FirCoeff5) );
    CHECK( addVariable ("LutStrategy",m_LutStrategy) );
    CHECK( addVariable ("LutOffset",m_LutOffset) );
    CHECK( addVariable ("LutNoiseCut",m_LutNoiseCut) );
    CHECK( addVariable ("LutSlope",m_LutSlope) );
    CHECK( addVariable ("RxGain",m_RxGain) );
    CHECK( addVariable ("RxStatus",m_RxStatus) );
    CHECK( addVariable ("RxId",m_RxId) );

    CHECK( addVariable ("NCellsNonNominal",m_nCellsNonNominal) );
    CHECK( addVariable ("NCellsNonNominalByLayer",m_nCellsNonNominalByLayer) );
    CHECK( addVariable ("NonNominalMeanScale",m_nonNominalMeanScale) );
    CHECK( addVariable ("NonNominalMeanScaleByLayer",m_nonNominalMeanScaleByLayer) );
    CHECK( addVariable ("NonNominalMeanScaleByReceiver",m_nonNominalMeanScaleByReceiver) );

      
    return StatusCode::SUCCESS;
  }

  StatusCode L1CaloPPMdbDataFillerTool::fill(){
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

      const CondAttrListCollection* attrChanCalib = 0;
      const CondAttrListCollection* attrRxGains = 0;
      const CondAttrListCollection* attrRxStatus = 0;

      sc = evtStore()->retrieve(attrChanCalib,m_dbPpmChanCalib);
      if(sc.isFailure()){return sc;}

      sc = evtStore()->retrieve(attrRxGains,m_dbRxGains);
      if(sc.isFailure()){return sc;}

      sc = evtStore()->retrieve(attrRxStatus,m_dbRxStatus);
      if(sc.isFailure()){return sc;}
      
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


        const coral::AttributeList* emDbAttr = m_tools->emDbAttributes(*i,attrChanCalib);
        std::vector<const coral::AttributeList*> emDbAttrRx = m_tools->emDbRxGainsAttributes(*i,attrRxGains);
        std::vector<const coral::AttributeList*> emDbAttrRxStatus = m_tools->emDbRxGainsAttributes(*i,attrRxStatus);

        m_DacOffset->push_back(m_tools->DacOffset(emDbAttr));
        m_DacSlope->push_back(m_tools->DacSlope(emDbAttr));
        m_PedMean->push_back(m_tools->PedMean(emDbAttr));
        m_PedValue->push_back(m_tools->PedValue(emDbAttr));
        m_FullDelayData->push_back(m_tools->FullDelayData(emDbAttr));
        m_SatBcidThreshLow->push_back(m_tools->SatBcidThreshLow(emDbAttr));
        m_SatBcidThreshHigh->push_back(m_tools->SatBcidThreshHigh(emDbAttr));
        m_SatBcidLevel->push_back(m_tools->SatBcidLevel(emDbAttr));
        m_BcidEnergyRangeLow->push_back(m_tools->BcidEnergyRangeLow(emDbAttr));
        m_BcidEnergyRangeHigh->push_back(m_tools->BcidEnergyRangeHigh(emDbAttr));
        m_FirStartBit->push_back(m_tools->FirStartBit(emDbAttr));
        m_FirCoeff1->push_back(m_tools->FirCoeff1(emDbAttr));
        m_FirCoeff2->push_back(m_tools->FirCoeff2(emDbAttr));
        m_FirCoeff3->push_back(m_tools->FirCoeff3(emDbAttr));
        m_FirCoeff4->push_back(m_tools->FirCoeff4(emDbAttr));
        m_FirCoeff5->push_back(m_tools->FirCoeff5(emDbAttr));
        m_LutStrategy->push_back(m_tools->LutStrategy(emDbAttr));
        m_LutOffset->push_back(m_tools->LutOffset(emDbAttr));
        m_LutNoiseCut->push_back(m_tools->LutNoiseCut(emDbAttr));
        m_LutSlope->push_back(m_tools->LutSlope(emDbAttr));

        std::vector<float> emGains;
        for(Itr_Rx j=emDbAttrRx.begin();j!=emDbAttrRx.end();++j){
          emGains.push_back(m_tools->RxGain(*j));
        }
        m_RxGain->push_back(emGains);

        std::vector<int> emGainStatus;
        for(Itr_Rx j=emDbAttrRxStatus.begin();j!=emDbAttrRxStatus.end();++j){
          emGainStatus.push_back(m_tools->RxStatus(*j));
        }
        m_RxStatus->push_back(emGainStatus);

        m_RxId->push_back(m_tools->emRxId(*i));

        m_nCellsNonNominal->push_back(m_tools->emNCellsNonNominal(*i));
        m_nCellsNonNominalByLayer->push_back(m_tools->emNCellsNonNominalByLayer(*i));
        m_nonNominalMeanScale->push_back(m_tools->emNonNominalMeanScale(*i));
        m_nonNominalMeanScaleByLayer->push_back(m_tools->emNonNominalMeanScaleByLayer(*i));
        m_nonNominalMeanScaleByReceiver->push_back(m_tools->emNonNominalMeanScaleByReceiver(*i));
        
        

        // Had
        m_coolId->push_back(m_tools->hadCoolChannelId(*i));
        m_eta->push_back((*i)->eta());
        m_l1caloPhi->push_back((*i)->phi());
        m_atlasPhi->push_back((this->AtlasPhi((*i)->phi())));         
        
        m_isTile->push_back(m_tools->hadIsTile(*i));
        m_nCells->push_back(m_tools->hadNCells(*i));
        m_cellsLayerNames->push_back(m_tools->hadTTCellsLayerNames(*i));
        m_nCellsByLayer->push_back(m_tools->hadNCellsByLayer(*i));

        const coral::AttributeList* hadDbAttr = m_tools->hadDbAttributes(*i,attrChanCalib);
        std::vector<const coral::AttributeList*> hadDbAttrRx = m_tools->hadDbRxGainsAttributes(*i,attrRxGains);
        std::vector<const coral::AttributeList*> hadDbAttrRxStatus = m_tools->hadDbRxGainsAttributes(*i,attrRxStatus);

        m_DacOffset->push_back(m_tools->DacOffset(hadDbAttr));
        m_DacSlope->push_back(m_tools->DacSlope(hadDbAttr));
        m_PedMean->push_back(m_tools->PedMean(hadDbAttr));
        m_PedValue->push_back(m_tools->PedValue(hadDbAttr));
        m_FullDelayData->push_back(m_tools->FullDelayData(hadDbAttr));
        m_SatBcidThreshLow->push_back(m_tools->SatBcidThreshLow(hadDbAttr));
        m_SatBcidThreshHigh->push_back(m_tools->SatBcidThreshHigh(hadDbAttr));
        m_SatBcidLevel->push_back(m_tools->SatBcidLevel(hadDbAttr));
        m_BcidEnergyRangeLow->push_back(m_tools->BcidEnergyRangeLow(hadDbAttr));
        m_BcidEnergyRangeHigh->push_back(m_tools->BcidEnergyRangeHigh(hadDbAttr));
        m_FirStartBit->push_back(m_tools->FirStartBit(hadDbAttr));
        m_FirCoeff1->push_back(m_tools->FirCoeff1(hadDbAttr));
        m_FirCoeff2->push_back(m_tools->FirCoeff2(hadDbAttr));
        m_FirCoeff3->push_back(m_tools->FirCoeff3(hadDbAttr));
        m_FirCoeff4->push_back(m_tools->FirCoeff4(hadDbAttr));
        m_FirCoeff5->push_back(m_tools->FirCoeff5(hadDbAttr));
        m_LutStrategy->push_back(m_tools->LutStrategy(hadDbAttr));
        m_LutOffset->push_back(m_tools->LutOffset(hadDbAttr));
        m_LutNoiseCut->push_back(m_tools->LutNoiseCut(hadDbAttr));
        m_LutSlope->push_back(m_tools->LutSlope(hadDbAttr));


        std::vector<float> hadGains;
        for(Itr_Rx j=hadDbAttrRx.begin();j!=hadDbAttrRx.end();++j){
          hadGains.push_back(m_tools->RxGain(*j));
        }
        m_RxGain->push_back(hadGains);

        std::vector<int> hadGainStatus;
        for(Itr_Rx j=hadDbAttrRxStatus.begin();j!=hadDbAttrRxStatus.end();++j){
          hadGainStatus.push_back(m_tools->RxStatus(*j));
        }
        m_RxStatus->push_back(hadGainStatus);

        m_RxId->push_back(m_tools->hadRxId(*i));

        m_nCellsNonNominal->push_back(m_tools->hadNCellsNonNominal(*i));
        m_nCellsNonNominalByLayer->push_back(m_tools->hadNCellsNonNominalByLayer(*i));
        m_nonNominalMeanScale->push_back(m_tools->hadNonNominalMeanScale(*i));
        m_nonNominalMeanScaleByLayer->push_back(m_tools->hadNonNominalMeanScaleByLayer(*i));
        m_nonNominalMeanScaleByReceiver->push_back(m_tools->hadNonNominalMeanScaleByReceiver(*i));
        
      }
      m_firstEvent = false;
    }    
    return StatusCode::SUCCESS;
  }

  float L1CaloPPMdbDataFillerTool::AtlasPhi(const float& phi){
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
