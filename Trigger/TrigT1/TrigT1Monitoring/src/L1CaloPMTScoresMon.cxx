/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     L1CaloPMTScoresMon.cxx
// PACKAGE:  TrigT1Monitoring  
//
// AUTHOR:   Peter Faulkner
//
// ********************************************************************

#include <cmath>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/ReadCondHandle.h"

#include "LWHists/TH2F_LW.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "TileEvent/TileCell.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileBchStatus.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/HWIdentifier.h"

#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloCells2TriggerTowers.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "L1CaloPMTScoresMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
const int L1CaloPMTScoresMon::s_numPMTPlots;

/*---------------------------------------------------------*/
L1CaloPMTScoresMon::L1CaloPMTScoresMon(const std::string & type,
                                       const std::string & name,
				       const IInterface* parent)
  : ManagedMonitorToolBase ( type, name, parent ),
    m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_ttIdTools("LVL1::L1CaloTTIdTools/L1CaloTTIdTools"),
    m_tileBadChanTool("TileBadChanTool"),
    m_tileHWID(0),
    m_events(0),
    m_histBooked(false),
    m_h_hadPMTScore(0),
    m_h_hadDisabled(0)
/*---------------------------------------------------*/
{
  declareProperty("TileBadChanTool", m_tileBadChanTool);
  declareProperty("CaloCellContainer", m_caloCellContainerName = "AllCalo");
  declareProperty("xAODTriggerTowerContainer",
                  m_xAODTriggerTowerContainerName = LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation);
  declareProperty("PathInRootFile",
                  m_PathInRootFile = "LVL1_Interfaces/Calorimeter") ;

}

/*---------------------------------------------------------*/
L1CaloPMTScoresMon::~L1CaloPMTScoresMon()
/*---------------------------------------------------------*/
{
}

/*---------------------------------------------------------*/
StatusCode L1CaloPMTScoresMon:: initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << endmsg;

  ATH_CHECK( m_cellMatch.retrieve() );

  StatusCode sc;
  
  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  sc = m_ttIdTools.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to locate tool L1CaloTTIdTools" << endmsg;
    return sc;
  }

  sc = m_ttTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to locate tool L1TriggerTowerTool" << endmsg;
    return sc;
  }

  sc = m_errorTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool"
                    << endmsg;
    return sc;
  }

  sc = m_histTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool"
                    << endmsg;
    return sc;
  }

  sc = detStore()->retrieve(m_tileHWID);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve TileHWID helper from DetectorStore" << endmsg;
    return sc;
  }
  sc = m_tileBadChanTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not access tileBadChanTool" << endmsg;
    return sc;
  }

  ATH_CHECK( m_cablingKey.initialize() );

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode L1CaloPMTScoresMon:: finalize()
/*---------------------------------------------------------*/
{
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloPMTScoresMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  
  msg(MSG::DEBUG) << "in L1CaloPMTScoresMon::bookHistograms" << endmsg;

  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
	
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  //if ( newLumiBlockFlag()) { }

  if ( newRunFlag() ) {	

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    MonGroup Calo_PMTScore( this, m_PathInRootFile+"/PMTGainScores",
                                          run, attr, "", "lowerLB" );
    MonGroup Calo_PMTScoreNow( this, m_PathInRootFile+"/PMTGainScores/PMTCurrent",
                                          run, attr, "", "lowerLB" );

    m_histTool->setMonGroup(&Calo_PMTScoreNow);

    TH2F_LW* hist = 0;
    m_h_hadPMTScore = m_histTool->bookPPMHadEtaVsPhi(
      "had_PMTScore", "Tile PMT Gain Score");
    m_v_hadPMTScores.assign(s_numPMTPlots, hist);
    m_v_hadPMTScores[0] = m_histTool->bookPPMHadEtaVsPhi(
      "had_PMTScore_a", "Tile PMT Gain Score A Layer");
    m_v_hadPMTScores[1] = m_histTool->bookPPMHadEtaVsPhi(
      "had_PMTScore_bc", "Tile PMT Gain Score BC Layers");
    m_v_hadPMTScores[2] = m_histTool->bookPPMHadEtaVsPhi(
      "had_PMTScore_d", "Tile PMT Gain Score D Layer");

    m_histTool->setMonGroup(&Calo_PMTScore);

    m_h_hadDisabled = m_histTool->bookPPMHadEtaVsPhi(
      "had_disabled", "Tile Disabled Towers");

    m_histTool->unsetMonGroup();
    m_histBooked = true;
    m_events = 0;

    StatusCode sc = m_ttTool->retrieveConditions();
    if(!sc.isSuccess()) {
      msg(MSG::WARNING) << "Conditions not retrieved " << endmsg;
    }
  }

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode L1CaloPMTScoresMon::fillHistograms()
/*---------------------------------------------------------*/
{
  if (m_events > 0) return StatusCode::SUCCESS;

  SG::ReadCondHandle<LArOnOffIdMapping> cabling (m_cablingKey);

  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG) << "in fillHistograms()" << endmsg;

  if (!m_histBooked) {
    if (debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt

  if (m_errorTool->corrupt()) {
    if (debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endmsg;
    return StatusCode::SUCCESS;
  }

  StatusCode sc;

  //Retreive Calo Cell collection from SG

  const CaloCellContainer* caloCellContainer = 0;
  sc = evtStore()->retrieve(caloCellContainer, m_caloCellContainerName); 
  if(!sc.isSuccess() || !caloCellContainer) {
    msg(MSG::WARNING) << "No CaloCellContainer found at AllCalo" << endmsg; 
    return StatusCode::SUCCESS;
  }

  ++m_events;

  // =========================================================================
  // ================= CaloCells  ============================================
  // =========================================================================
 
  // KW CHECK IF THIS IS OK!!!!
 
  CaloCellContainer::const_iterator CaloCellIterator    = caloCellContainer->begin();
  CaloCellContainer::const_iterator CaloCellIteratorEnd = caloCellContainer->end();
  
  for (; CaloCellIterator != CaloCellIteratorEnd; ++CaloCellIterator) {
      
    const CaloCell* caloCell(*CaloCellIterator);
    const CaloDetDescrElement* caloDDE = caloCell->caloDDE();
    if (!caloDDE->is_tile()) continue;

    //const Identifier cellId(caloCell->ID());
    const Identifier invalidId(0);
    Identifier ttId1(0);
    Identifier ttId2(0);

    m_cellMatch->matchCell2Tower(**cabling, caloCell, ttId1, ttId2);

    if (ttId1 != invalidId) {
      const double eta = m_ttIdTools->IDeta(ttId1);
      const double phi = m_ttIdTools->IDphi(ttId1);
      CaloCell_ID::CaloSample sampling = caloDDE->getSampling();
      int sample = sampling;
      sample = (sample-CaloCell_ID::TileBar0)%3;
      if (sampling == CaloCell_ID::TileGap1 || sampling == CaloCell_ID::TileGap2 ||
          sampling == CaloCell_ID::TileGap3) sample++;
      const double pmtScore = tileNonNominal(caloCell);
      m_histTool->fillPPMHadEtaVsPhi(m_h_hadPMTScore, eta, phi, pmtScore);
      if (sample < s_numPMTPlots) {
        m_histTool->fillPPMHadEtaVsPhi(m_v_hadPMTScores[sample], eta, phi, pmtScore);
      }
      if (ttId2 != invalidId) {
        const double eta2 = m_ttIdTools->IDeta(ttId2);
        const double phi2 = m_ttIdTools->IDphi(ttId2);
        m_histTool->fillPPMHadEtaVsPhi(m_h_hadPMTScore, eta2, phi2, pmtScore);
        if (sample < s_numPMTPlots) {
          m_histTool->fillPPMHadEtaVsPhi(m_v_hadPMTScores[sample], eta2, phi2, pmtScore);
        }
      }
    }
  }

  // =========================================================================
  // =================== TriggerTowers =======================================
  // =========================================================================

  //Retrieve xAODTriggerTowers from SG
  const xAOD::TriggerTowerContainer* triggerTowerTES = 0; 
  sc = evtStore()->retrieve(triggerTowerTES, m_xAODTriggerTowerContainerName); 
  if(sc==StatusCode::FAILURE || !triggerTowerTES) {
    msg(MSG::INFO) << "No xAODTriggerTower found in TES at "
                   << m_xAODTriggerTowerContainerName << endmsg ;
    return StatusCode::SUCCESS;
  }
      
  xAOD::TriggerTowerContainer::const_iterator ttIterator =
                                                      triggerTowerTES->begin(); 
  xAOD::TriggerTowerContainer::const_iterator ttIteratorEnd =
                                                      triggerTowerTES->end(); 
      
  L1CaloCoolChannelId had_coolId;

  for (; ttIterator != ttIteratorEnd; ++ttIterator) {
	  
    double eta = (*ttIterator)->eta();
    if (fabs(eta) > 1.5) continue;
    double phi = (*ttIterator)->phi();

    had_coolId = m_ttTool->channelID(eta, phi, 1);
    bool had_disabled = m_ttTool->disabledChannel(had_coolId);
    if (had_disabled) m_histTool->fillPPMHadEtaVsPhi(m_h_hadDisabled, eta, phi, 1.);
  }

  return StatusCode::SUCCESS;     
}
/*---------------------------------------------------------*/
StatusCode L1CaloPMTScoresMon::procHistograms()
/*---------------------------------------------------------*/
{
  //if ( endOfLumiBlockFlag() ) { }
	
  //if ( endOfRunFlag() ) { }
  
  return StatusCode::SUCCESS;
}

// Return Tile PMT Gain score.  Copied from L1CaloOfflineTriggerTowerTools.

double L1CaloPMTScoresMon::tileNonNominal(const CaloCell* cell)
{
  // There are either 1 or 2 PMTs per Calo Cell
  // A PMT may have Half Gain L1 or No Gain L1
  //
  // Assign a score as follows.
  //
  // Case (a) : 2 PMTs in the Cell
  // Half gain counts as 0.25
  // No gain counts as 0.5
  // So if a cells has no gain in PMT1 and half gain in PMT2 it's score is 0.75
  //
  // Case (b) : 1 PMT in the Cell
  // Half gain counts as 0.5
  // No Gain counts as 1.0

  // Cast the CaloCell as a TileCell
  const TileCell *tileCell = dynamic_cast<const TileCell*>(cell);
  if (!tileCell) return 0.0;

  // Get IdentifierHash for both PMTs
  const CaloDetDescrElement * caloDDE = tileCell->caloDDE();
  IdentifierHash hash1 = caloDDE->onl1();
  IdentifierHash hash2 = caloDDE->onl2();

  // Values for PMT 1 and 2
  int gain1(-9),gain2(-9);
  float score(0.0),multiplier(1.0);
  bool adc1_noGainL1(false),adc1_halfGainL1(false);
  bool pmt2Valid(false),adc2_noGainL1(false),adc2_halfGainL1(false);

  // PMT 1
  gain1 = tileCell->gain1();
  HWIdentifier adc_id1 = m_tileHWID->adc_id(hash1,(gain1<0)?1:gain1);
  const TileBchStatus& badChan1(m_tileBadChanTool->getAdcStatus(adc_id1));
  adc1_noGainL1 = badChan1.isNoGainL1();
  adc1_halfGainL1 = badChan1.isHalfGainL1();

  // gap/crack scintillators only have one PMT per cell
  // PMT 2, if valid
  if(hash2 != TileHWID::NOT_VALID_HASH){
    pmt2Valid = true;
    gain2 = tileCell->gain2();
    HWIdentifier adc_id2 = m_tileHWID->adc_id(hash2,(gain2<0)?1:gain2);
    const TileBchStatus& badChan2(m_tileBadChanTool->getAdcStatus(adc_id2));
    adc2_noGainL1 = badChan2.isNoGainL1();
    adc2_halfGainL1 = badChan2.isHalfGainL1();
  }

  // Calculate the TileCell score

  // Check that a PMT can't contribute more than 0.5
  // Should not be the case, but assumptions cause bugs.....
  if(adc1_noGainL1 == true && adc1_halfGainL1 == true){adc1_halfGainL1 = false;}
  if(adc2_noGainL1 == true && adc2_halfGainL1 == true){adc2_halfGainL1 = false;}

  if(pmt2Valid == false){multiplier = 2.0;}
  if(adc1_halfGainL1 == true){score += 0.25;}
  if(adc1_noGainL1 == true){score += 0.5;}
  if(adc2_halfGainL1 == true){score += 0.25;}
  if(adc2_noGainL1 == true){score += 0.5;}

  return score * multiplier;
}
// ============================================================================
}  // end namespace
