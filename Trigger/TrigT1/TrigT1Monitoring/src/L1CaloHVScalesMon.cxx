/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     L1CaloHVScalesMon.cxx
// PACKAGE:  TrigT1Monitoring  
//
// AUTHOR:   Peter Faulkner
//
// ********************************************************************

#include <cmath>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "LWHists/TH2F_LW.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/HWIdentifier.h"
#include "StoreGate/ReadCondHandle.h"

#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloHVCorrections.h"
#include "TrigT1CaloCalibConditions/L1CaloHVCorrectionsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloRxCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloRxLayers.h"
#include "TrigT1CaloCalibConditions/L1CaloRxLayersContainer.h"
#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloMatchCell2Tower.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloLArTowerEnergy.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "L1CaloHVScalesMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
L1CaloHVScalesMon::L1CaloHVScalesMon(const std::string & type,
                                     const std::string & name,
				     const IInterface* parent)
  : ManagedMonitorToolBase ( type, name, parent ),
    m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_larEnergy("LVL1::L1CaloLArTowerEnergy/L1CaloLArTowerEnergy"),
    m_ttIdTools("LVL1::L1CaloTTIdTools/L1CaloTTIdTools"),
    m_lvl1Helper(0),
    m_l1CondSvc("L1CaloCondSvc", name),
    m_ttSvc("CaloTriggerTowerService"),
    m_events(0),
    m_histBooked(false),
    m_h_emHVScale(0),
    m_h_hadHVScale(0),
    m_h_emHVEntry(0),
    m_h_hadHVEntry(0),
    m_h_emHVScaleRef(0),
    m_h_hadHVScaleRef(0),
    m_h_emHVScaleDif(0),
    m_h_hadHVScaleDif(0),
    m_h_emFEB(0),
    m_h_hadFEB(0),
    m_h_emDisabled(0),
    m_h_hadDisabled(0)
/*---------------------------------------------------*/
{
  declareProperty("CaloCellContainer", m_caloCellContainerName = "AllCalo");
  declareProperty("xAODTriggerTowerContainer", 
                   m_xAODTriggerTowerContainerName = LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation);
  declareProperty("PathInRootFile",
                  m_PathInRootFile = "LVL1_Interfaces/Calorimeter") ;
  declareProperty("DoHVDifference", m_hvDifference = true);

}

/*---------------------------------------------------------*/
L1CaloHVScalesMon::~L1CaloHVScalesMon()
/*---------------------------------------------------------*/
{
}

/*---------------------------------------------------------*/
StatusCode L1CaloHVScalesMon:: initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << endmsg;

  ATH_CHECK( ManagedMonitorToolBase::initialize() );
  ATH_CHECK( m_cellMatch.retrieve() );
  ATH_CHECK( m_larEnergy.retrieve() );
  ATH_CHECK( m_ttIdTools.retrieve() );
  ATH_CHECK( m_ttTool.retrieve() );
  ATH_CHECK( m_errorTool.retrieve() );
  ATH_CHECK( m_histTool.retrieve() );
  ATH_CHECK( detStore()->retrieve(m_lvl1Helper, "CaloLVL1_ID") );

  if (m_hvDifference) {
    ATH_CHECK( m_l1CondSvc.retrieve() );
    ATH_CHECK( m_ttSvc.retrieve() );
  }

  ATH_CHECK( m_scaleCorrKey.initialize() );
  ATH_CHECK( m_onlineScaleCorrKey.initialize() );
  ATH_CHECK( m_cablingKey.initialize() );

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode L1CaloHVScalesMon:: finalize()
/*---------------------------------------------------------*/
{
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloHVScalesMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  
  msg(MSG::DEBUG) << "in L1CaloHVScalesMon::bookHistograms" << endmsg;

  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
	
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  //if ( newLumiBlockFlag()) { }

  if ( newRunFlag() ) {	

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    MonGroup Calo_HVCorr( this, m_PathInRootFile+"/HVCorrections",
                                          run, attr, "", "lowerLB" );
    MonGroup Calo_HVCorrNow( this, m_PathInRootFile+"/HVCorrections/HVCurrent",
                                          run, attr, "", "lowerLB" );
    MonGroup Calo_HVCorrThen( this, m_PathInRootFile+"/HVCorrections/HVLastCalibration",
                                          run, attr, "", "lowerLB" );

    m_histTool->setMonGroup(&Calo_HVCorrNow);

    TH2F_LW* hist = 0;
    m_h_emHVScale = m_histTool->bookPPMEmEtaVsPhi(
      "em_HVScale", "EM Mean HV Scale");
    m_h_hadHVScale = m_histTool->bookPPMHadEtaVsPhi(
      "had_HVScale", "Had Mean HV Scale");
    m_v_emHVScales.assign(s_numEmHVPlots, hist);
    m_v_hadHVScales.assign(s_numHadHVPlots, hist);
    m_v_emHVScales[0] = m_histTool->bookPPMEmEtaVsPhi(
      "em_HVScale_pre", "EM Mean HV Scale Presampler");
    m_v_emHVScales[1] = m_histTool->bookPPMEmEtaVsPhi(
      "em_HVScale_emb1", "EM Mean HV Scale EMB Layer 1");
    m_v_emHVScales[2] = m_histTool->bookPPMEmEtaVsPhi(
      "em_HVScale_emb2", "EM Mean HV Scale EMB Layer 2");
    m_v_emHVScales[3] = m_histTool->bookPPMEmEtaVsPhi(
      "em_HVScale_emb3", "EM Mean HV Scale EMB Layer 3");
    m_v_emHVScales[4] = m_histTool->bookPPMEmEtaVsPhi(
      "em_HVScale_emec1", "EM Mean HV Scale EMEC Layer 1");
    m_v_emHVScales[5] = m_histTool->bookPPMEmEtaVsPhi(
      "em_HVScale_emec2", "EM Mean HV Scale EMEC Layer 2");
    m_v_emHVScales[6] = m_histTool->bookPPMEmEtaVsPhi(
      "em_HVScale_emec3", "EM Mean HV Scale EMEC Layer 3");
    m_v_emHVScales[7] = m_histTool->bookPPMEmEtaVsPhi(
      "em_HVScale_fcal1", "EM Mean HV Scale FCAL1");
    m_v_hadHVScales[0] = m_histTool->bookPPMHadEtaVsPhi(
      "had_HVScale_hec0", "Had Mean HV Scale HEC Layer 0");
    m_v_hadHVScales[1] = m_histTool->bookPPMHadEtaVsPhi(
      "had_HVScale_hec1", "Had Mean HV Scale HEC Layer 1");
    m_v_hadHVScales[2] = m_histTool->bookPPMHadEtaVsPhi(
      "had_HVScale_hec2", "Had Mean HV Scale HEC Layer 2");
    m_v_hadHVScales[3] = m_histTool->bookPPMHadEtaVsPhi(
      "had_HVScale_fcal23r0", "Had Mean HV Scale FCAL23 Receiver 0");
    m_v_hadHVScales[4] = m_histTool->bookPPMHadEtaVsPhi(
      "had_HVScale_fcal23r1", "Had Mean HV Scale FCAL23 Receiver 1");

    m_histTool->unsetMonGroup();

    m_h_emHVEntry = m_histTool->bookPPMEmEtaVsPhi(
      "em_HVScale_e", "Entries");
    m_h_hadHVEntry = m_histTool->bookPPMHadEtaVsPhi(
      "had_HVScale_e", "Entries");
    m_v_emHVEntries.assign(s_numEmHVPlots, hist);
    m_v_hadHVEntries.assign(s_numHadHVPlots, hist);
    for (int i = 0; i < s_numEmHVPlots; ++i) {
      std::string name(m_v_emHVScales[i]->GetName());
      m_v_emHVEntries[i] = m_histTool->bookPPMEmEtaVsPhi(name+"_e", "Entries");
    }
    for (int i = 0; i < s_numHadHVPlots; ++i) {
      std::string name(m_v_hadHVScales[i]->GetName());
      m_v_hadHVEntries[i] = m_histTool->bookPPMHadEtaVsPhi(name+"_e", "Entries");
    }

    if (m_hvDifference) {

      m_histTool->setMonGroup(&Calo_HVCorrThen);

      m_h_emHVScaleRef = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleRef", "EM Mean HV Scale Last Calibration");
      m_h_hadHVScaleRef = m_histTool->bookPPMHadEtaVsPhi(
        "had_HVScaleRef", "Had Mean HV Scale Last Calibration");
      m_v_emHVScalesRef.assign(s_numEmHVPlots, hist);
      m_v_hadHVScalesRef.assign(s_numHadHVPlots, hist);
      m_v_emHVScalesRef[0] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleRef_pre", "EM Mean HV Scale Last Calibration Presampler");
      m_v_emHVScalesRef[1] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleRef_emb1", "EM Mean HV Scale Last Calibration EMB Layer 1");
      m_v_emHVScalesRef[2] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleRef_emb2", "EM Mean HV Scale Last Calibration EMB Layer 2");
      m_v_emHVScalesRef[3] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleRef_emb3", "EM Mean HV Scale Last Calibration EMB Layer 3");
      m_v_emHVScalesRef[4] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleRef_emec1", "EM Mean HV Scale Last Calibration EMEC Layer 1");
      m_v_emHVScalesRef[5] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleRef_emec2", "EM Mean HV Scale Last Calibration EMEC Layer 2");
      m_v_emHVScalesRef[6] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleRef_emec3", "EM Mean HV Scale Last Calibration EMEC Layer 3");
      m_v_emHVScalesRef[7] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleRef_fcal1", "EM Mean HV Scale Last Calibration FCAL1");
      m_v_hadHVScalesRef[0] = m_histTool->bookPPMHadEtaVsPhi(
        "had_HVScaleRef_hec0", "Had Mean HV Scale Last Calibration HEC Layer 0");
      m_v_hadHVScalesRef[1] = m_histTool->bookPPMHadEtaVsPhi(
        "had_HVScaleRef_hec1", "Had Mean HV Scale Last Calibration HEC Layer 1");
      m_v_hadHVScalesRef[2] = m_histTool->bookPPMHadEtaVsPhi(
        "had_HVScaleRef_hec2", "Had Mean HV Scale Last Calibration HEC Layer 2");
      m_v_hadHVScalesRef[3] = m_histTool->bookPPMHadEtaVsPhi(
        "had_HVScaleRef_fcal23r0", "Had Mean HV Scale Last Calibration FCAL23 Receiver 0");
      m_v_hadHVScalesRef[4] = m_histTool->bookPPMHadEtaVsPhi(
        "had_HVScaleRef_fcal23r1", "Had Mean HV Scale Last Calibration FCAL23 Receiver 1");

      m_histTool->setMonGroup(&Calo_HVCorr);

      m_h_emHVScaleDif = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleDif", "EM Mean HV Scale Difference ((Current-Last)/Last)");
      m_h_hadHVScaleDif = m_histTool->bookPPMHadEtaVsPhi(
        "had_HVScaleDif", "Had Mean HV Scale Difference ((Current-Last)/Last)");
      m_v_emHVScalesDif.assign(s_numEmHVPlots, hist);
      m_v_hadHVScalesDif.assign(s_numHadHVPlots, hist);
      m_v_emHVScalesDif[0] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleDif_pre", "EM Mean HV Scale Difference ((Current-Last)/Last) Presampler");
      m_v_emHVScalesDif[1] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleDif_emb1", "EM Mean HV Scale Difference ((Current-Last)/Last) EMB Layer 1");
      m_v_emHVScalesDif[2] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleDif_emb2", "EM Mean HV Scale Difference ((Current-Last)/Last) EMB Layer 2");
      m_v_emHVScalesDif[3] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleDif_emb3", "EM Mean HV Scale Difference ((Current-Last)/Last) EMB Layer 3");
      m_v_emHVScalesDif[4] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleDif_emec1", "EM Mean HV Scale Difference ((Current-Last)/Last) EMEC Layer 1");
      m_v_emHVScalesDif[5] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleDif_emec2", "EM Mean HV Scale Difference ((Current-Last)/Last) EMEC Layer 2");
      m_v_emHVScalesDif[6] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleDif_emec3", "EM Mean HV Scale Difference ((Current-Last)/Last) EMEC Layer 3");
      m_v_emHVScalesDif[7] = m_histTool->bookPPMEmEtaVsPhi(
        "em_HVScaleDif_fcal1", "EM Mean HV Scale Difference ((Current-Last)/Last) FCAL1");
      m_v_hadHVScalesDif[0] = m_histTool->bookPPMHadEtaVsPhi(
        "had_HVScaleDif_hec0", "Had Mean HV Scale Difference ((Current-Last)/Last) HEC Layer 0");
      m_v_hadHVScalesDif[1] = m_histTool->bookPPMHadEtaVsPhi(
        "had_HVScaleDif_hec1", "Had Mean HV Scale Difference ((Current-Last)/Last) HEC Layer 1");
      m_v_hadHVScalesDif[2] = m_histTool->bookPPMHadEtaVsPhi(
        "had_HVScaleDif_hec2", "Had Mean HV Scale Difference ((Current-Last)/Last) HEC Layer 2");
      m_v_hadHVScalesDif[3] = m_histTool->bookPPMHadEtaVsPhi(
        "had_HVScaleDif_fcal23r0", "Had Mean HV Scale Difference ((Current-Last)/Last) FCAL23 Receiver 0");
      m_v_hadHVScalesDif[4] = m_histTool->bookPPMHadEtaVsPhi(
        "had_HVScaleDif_fcal23r1", "Had Mean HV Scale Difference ((Current-Last)/Last) FCAL23 Receiver 1");
    }

    m_histTool->setMonGroup(&Calo_HVCorr);

    m_h_emFEB = m_histTool->bookPPMEmEtaVsPhi(
      "em_missing_FEB", "EM Towers with Missing FEB");
    m_h_hadFEB = m_histTool->bookPPMHadEtaVsPhi(
      "had_missing_FEB", "Had Towers with Missing FEB");
    m_h_emDisabled = m_histTool->bookPPMEmEtaVsPhi(
      "em_disabled", "EM Disabled Towers");
    m_h_hadDisabled = m_histTool->bookPPMHadEtaVsPhi(
      "had_disabled", "LAr Had Disabled Towers");

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
StatusCode L1CaloHVScalesMon::fillHistograms()
/*---------------------------------------------------------*/
{
  if (m_events > 0) return StatusCode::SUCCESS;

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

  // Retrieve HV conditions at last calibration

  ++m_events;
  L1CaloHVCorrectionsContainer* hvCorrectionsContainer = 0;
  L1CaloRxLayersContainer* rxLayersContainer = 0;
  if (m_hvDifference) {
    sc = m_l1CondSvc->retrieve(hvCorrectionsContainer);
    if (sc.isFailure() || !hvCorrectionsContainer) {
      msg(MSG::WARNING) << "Could not retrieve HVCorrectionsContainer" << endmsg;
    }
    sc = m_l1CondSvc->retrieve(rxLayersContainer);
    if (sc.isFailure() || !rxLayersContainer) {
      msg(MSG::WARNING) << "Could not retrieve RxLayersContainer" << endmsg;
    }
  }

  // =========================================================================
  // ================= CaloCells  ============================================
  // =========================================================================
  
  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadCondHandle<ILArHVScaleCorr> scaleCorr (m_scaleCorrKey, ctx);
  SG::ReadCondHandle<ILArHVScaleCorr> onlineScaleCorr (m_onlineScaleCorrKey, ctx);
  SG::ReadCondHandle<LArOnOffIdMapping> cabling (m_cablingKey, ctx);

  CaloCellContainer::const_iterator CaloCellIterator    = caloCellContainer->begin();
  CaloCellContainer::const_iterator CaloCellIteratorEnd = caloCellContainer->end();
  
  for (; CaloCellIterator != CaloCellIteratorEnd; ++CaloCellIterator) {
      
    const CaloCell* caloCell(*CaloCellIterator);
    const CaloDetDescrElement* caloDDE = caloCell->caloDDE();
    if (caloDDE->is_tile()) continue;
    const Identifier invalidId(0);
    Identifier ttId1(0);
    Identifier ttId2(0);

    m_cellMatch->matchCell2Tower(**cabling, caloCell, ttId1, ttId2);

    if (ttId1 != invalidId) {
      const double eta = m_ttIdTools->IDeta(ttId1);
      const double phi = m_ttIdTools->IDphi(ttId1);
      const CaloCell_ID::CaloSample sampling = caloDDE->getSampling();
      int sample = sampling;
      if (sample < 12) sample %= 4;
      else sample = (sample-12)%3;
      const int layer = m_lvl1Helper->sampling(ttId1);
      const Identifier cellId(caloCell->ID());
      HWIdentifier hwid = cabling->createSignalChannelID(cellId);
      const double scale = scaleCorr->HVScaleCorr(hwid) * onlineScaleCorr->HVScaleCorr(hwid);
      if (debug && scale < 1.) {
        msg(MSG::DEBUG) << " Current Mean Scale " << scale << " for sampling " << sampling
                        << " eta/phi " << eta << "/" << phi << endmsg;
      }
      if (layer == 0) {
        m_histTool->fillPPMEmEtaVsPhi(m_h_emHVScale, eta, phi, scale);
	m_histTool->fillPPMEmEtaVsPhi(m_h_emHVEntry, eta, phi);
        if (sampling == CaloCell_ID::EME1 || sampling == CaloCell_ID::EME2 ||
            sampling == CaloCell_ID::EME3) sample += 3;
	if (sampling == CaloCell_ID::FCAL0) sample = 7;
	if (sample < s_numEmHVPlots) {
	  m_histTool->fillPPMEmEtaVsPhi(m_v_emHVScales[sample], eta, phi, scale);
	  m_histTool->fillPPMEmEtaVsPhi(m_v_emHVEntries[sample], eta, phi);
	}
      } else {
	m_histTool->fillPPMHadEtaVsPhi(m_h_hadHVScale, eta, phi, scale);
	m_histTool->fillPPMHadEtaVsPhi(m_h_hadHVEntry, eta, phi);
	if (sampling == CaloCell_ID::FCAL1 || sampling == CaloCell_ID::FCAL2) {
	  sample = 3 + fcalReceiver(caloDDE, eta);
	}
	if (sample < s_numHadHVPlots) {
	  m_histTool->fillPPMHadEtaVsPhi(m_v_hadHVScales[sample], eta, phi, scale);
	  m_histTool->fillPPMHadEtaVsPhi(m_v_hadHVEntries[sample], eta, phi);
	}
      }
    }

  }

  int nbinsx = 0;
  int nbinsy = 0;
  double total = 0.;
  double entries = 0.;
  for (int i = 0; i < s_numEmHVPlots; ++i) {
    nbinsx = m_v_emHVScales[i]->GetNbinsX();
    nbinsy = m_v_emHVScales[i]->GetNbinsY();
    for (int bx = 1; bx <= nbinsx; ++bx) {
      for (int by = 1; by <= nbinsy; ++by) {
        entries = m_v_emHVEntries[i]->GetBinContent(bx, by);
        if (entries != 0.) {
          total = m_v_emHVScales[i]->GetBinContent(bx, by);
	  m_v_emHVScales[i]->SetBinContent(bx, by, total/entries);
	}
	if (i == 0) {
          entries = m_h_emHVEntry->GetBinContent(bx, by);
          if (entries != 0.) {
	    total = m_h_emHVScale->GetBinContent(bx, by);
	    m_h_emHVScale->SetBinContent(bx, by, total/entries);
	  }
        }
      }
    }
  }
  for (int i = 0; i < s_numHadHVPlots; ++i) {
    nbinsx = m_v_hadHVScales[i]->GetNbinsX();
    nbinsy = m_v_hadHVScales[i]->GetNbinsY();
    for (int bx = 1; bx <= nbinsx; ++bx) {
      for (int by = 1; by <= nbinsy; ++by) {
        entries = m_v_hadHVEntries[i]->GetBinContent(bx, by);
        if (entries != 0.) {
          total = m_v_hadHVScales[i]->GetBinContent(bx, by);
          m_v_hadHVScales[i]->SetBinContent(bx, by, total/entries);
        }
	if (i == 0) {
          entries = m_h_hadHVEntry->GetBinContent(bx, by);
          if (entries != 0.) {
	    total = m_h_hadHVScale->GetBinContent(bx, by);
	    m_h_hadHVScale->SetBinContent(bx, by, total/entries);
	  }
        }
      }
    }
  }

  // =========================================================================
  // =================== TriggerTowers =======================================
  // =========================================================================

  //Retrieve TriggerTowers from SG
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
  
  Identifier em_ident;
  Identifier had_ident;
  L1CaloCoolChannelId em_coolId;
  L1CaloCoolChannelId had_coolId;
  const int maxSamplings = 4;
  const float defaultMean = 1.;
  const std::vector<float> defaultLayerMeans(maxSamplings, defaultMean);
  std::vector<unsigned int> rxIds;

  for (; ttIterator != ttIteratorEnd; ++ttIterator) {
	  
    const double eta = (*ttIterator)->eta();
    const double phi = (*ttIterator)->phi();
    const double absEta = fabs(eta);

    em_ident = m_ttTool->identifier(eta, phi, 0);
    em_coolId = m_ttTool->channelID(eta, phi, 0);
    if (m_ttTool->disabledChannel(em_coolId)) {
      m_histTool->fillPPMEmEtaVsPhi(m_h_emDisabled, eta, phi);
    }
    if (m_larEnergy->hasMissingFEB(**cabling, em_ident)) {
      m_histTool->fillPPMEmEtaVsPhi(m_h_emFEB, eta, phi);
    }
    if (absEta > 1.5) {
      had_ident = m_ttTool->identifier(eta, phi, 1);
      had_coolId = m_ttTool->channelID(eta, phi, 1);
      if (m_ttTool->disabledChannel(had_coolId)) {
        m_histTool->fillPPMHadEtaVsPhi(m_h_hadDisabled, eta, phi);
      }
      if (m_larEnergy->hasMissingFEB(**cabling, had_ident)) {
        m_histTool->fillPPMHadEtaVsPhi(m_h_hadFEB, eta, phi);
      }
    }

    if (hvCorrectionsContainer && rxLayersContainer) {
      for (int layer = 0; layer < 2; ++layer) {
	if (layer == 1 && absEta < 1.5) break;
	rxIds = (layer == 0) ? emRxId(em_coolId) : hadRxId(had_coolId);
	const unsigned int nRx = rxIds.size();
	if (nRx == 0 || nRx > 2) continue;
	const L1CaloRxLayers* pTL1 = 0;
	const unsigned int coolId1 = rxIds[0];
	pTL1 = rxLayersContainer->rxLayers(coolId1);
	if (!pTL1) continue;
	const L1CaloHVCorrections* pHV1 = 0;
        pHV1 = hvCorrectionsContainer->hvCorrections(coolId1);
	const double refMean1 = (pHV1) ? (double)pHV1->rxMean() : defaultMean;
	double refMean = (double)refMean1;
	const std::vector<int>& ncells1(pTL1->ncells());
	int n1 = 0;
	int n2 = 0;
	std::vector<int>::const_iterator ic  = ncells1.begin();
	std::vector<int>::const_iterator icE = ncells1.end();
	for (; ic != icE; ++ic) n1 += *ic;
	const L1CaloRxLayers* pTL2 = 0;
	const L1CaloHVCorrections* pHV2 = 0;
	if (nRx > 1) {
	  const unsigned int coolId2 = rxIds[1];
	  pTL2 = rxLayersContainer->rxLayers(coolId2);
	  if (!pTL2) continue;
	  pHV2 = hvCorrectionsContainer->hvCorrections(coolId2);
	  const double refMean2 = (pHV2) ? (double)pHV2->rxMean() : defaultMean;
	  const std::vector<int>& ncells2(pTL2->ncells());
	  ic  = ncells2.begin();
	  icE = ncells2.end();
	  for (; ic != icE; ++ic) n2 += *ic;
	  if (n1+n2 != 0) refMean = (n1*refMean1 + n2*refMean2)/(n1+n2);
	}
	int binx = 0;
	int biny = 0;
	int currentNCells = 0;
	double currentMean = 0;
	double diffMean = 0;
        if (layer == 0) {
	  m_histTool->fillPPMEmEtaVsPhi(m_h_emHVScaleRef, eta, phi, refMean);
	  m_histTool->findBinPPMEmEtaVsPhi(m_h_emHVScale, eta, phi, binx, biny);
	  currentMean = m_h_emHVScale->GetBinContent(binx, biny);
	  currentNCells = m_h_emHVEntry->GetBinContent(binx, biny);
          if (debug && currentNCells != n1+n2) {
	    msg(MSG::DEBUG) << "NCells differ at eta/phi " << eta << "/" << phi << endmsg;
	  }
	  diffMean = (currentNCells == n1+n2) ? (currentMean - refMean)/refMean : 0.;
	  if (diffMean != 0.) {
            m_histTool->fillPPMEmEtaVsPhi(m_h_emHVScaleDif, eta, phi, diffMean);
	  }
	} else {
	  m_histTool->fillPPMHadEtaVsPhi(m_h_hadHVScaleRef, eta, phi, refMean);
	  m_histTool->findBinPPMHadEtaVsPhi(m_h_hadHVScale, eta, phi, binx, biny);
	  currentMean = m_h_hadHVScale->GetBinContent(binx, biny);
	  currentNCells = m_h_hadHVEntry->GetBinContent(binx, biny);
          if (debug && currentNCells != n1+n2) {
	    msg(MSG::DEBUG) << "NCells differ at eta/phi " << eta << "/" << phi << endmsg;
	  }
	  diffMean = (currentNCells == n1+n2) ? (currentMean - refMean)/refMean : 0.;
	  if (diffMean != 0.) {
            m_histTool->fillPPMHadEtaVsPhi(m_h_hadHVScaleDif, eta, phi, diffMean);
	  }
	}
	for (unsigned int rx = 0; rx < nRx; ++rx) {
	  std::vector<int>::const_iterator it;
	  std::vector<int>::const_iterator itE;
	  std::vector<float>::const_iterator is;
	  std::vector<float>::const_iterator isE;
	  if (rx == 0) {
	    const std::vector<int>& names1(pTL1->names());
	    const std::vector<float>& layerMeans1((pHV1) ? pHV1->layerMeans() : defaultLayerMeans);
	    it  = names1.begin();
	    itE = names1.end();
            ic  = ncells1.begin();
            icE = ncells1.end();
            is  = layerMeans1.begin();
            isE = layerMeans1.end();
          } else {
	    const std::vector<int>& names2(pTL2->names());
	    const std::vector<float>& layerMeans2((pHV2) ? pHV2->layerMeans() : defaultLayerMeans);
	    const std::vector<int>& ncells2(pTL2->ncells());
	    it  = names2.begin();
            itE = names2.end();
            ic  = ncells2.begin();
            icE = ncells2.end();
            is  = layerMeans2.begin();
            isE = layerMeans2.end();
	  }
	  for (; it != itE && ic != icE && is != isE; ++it, ++ic, ++is) {
	    refMean = (double)(*is);
	    const int sampling = *it;
	    if (debug && refMean < 1.) {
	      msg(MSG::DEBUG) << " Reference Mean Scale " << refMean << " for sampling " << sampling
	                      << " eta/phi " << eta << "/" << phi << endmsg;
            }
            int sample = sampling%4;
	    if (layer == 0) {
              if (sampling == CaloCell_ID::EME1 || sampling == CaloCell_ID::EME2 ||
	          sampling == CaloCell_ID::EME3) sample += 3;
	      if (sampling == CaloCell_ID::FCAL0) sample = 7;
	      if (sample < s_numEmHVPlots) {
	        m_histTool->fillPPMEmEtaVsPhi(m_v_emHVScalesRef[sample], eta, phi, refMean);
	        currentMean = m_v_emHVScales[sample]->GetBinContent(binx, biny);
	        currentNCells = m_v_emHVEntries[sample]->GetBinContent(binx, biny);
	        diffMean = (currentNCells == (*ic)) ? (currentMean - refMean)/refMean : 0.;
	        if (debug && currentNCells != (*ic)) {
	          msg(MSG::DEBUG) << "NCells differ for sampling " << sampling 
		                  << " eta/phi " << eta << "/" << phi << endmsg;
	        }
	        if (diffMean != 0.) {
	          m_histTool->fillPPMEmEtaVsPhi(m_v_emHVScalesDif[sample], eta, phi, diffMean);
	        }
	      }
	    }
            else {
	      if (sampling == CaloCell_ID::FCAL1 || sampling == CaloCell_ID::FCAL2) sample = 3 + rx;
	      if (sample < s_numHadHVPlots) {
	        m_histTool->fillPPMHadEtaVsPhi(m_v_hadHVScalesRef[sample], eta, phi, refMean);
	        currentMean = m_v_hadHVScales[sample]->GetBinContent(binx, biny);
	        currentNCells = m_v_hadHVEntries[sample]->GetBinContent(binx, biny);
	        if (debug && currentNCells != (*ic)) {
		  msg(MSG::DEBUG) << "NCells differ for sampling " << sampling
		                  << " eta/phi " << eta << "/" << phi << endmsg;
		}
	        diffMean = (currentNCells == (*ic)) ? (currentMean - refMean)/refMean : 0.;
	        if (diffMean != 0.) {
	          m_histTool->fillPPMHadEtaVsPhi(m_v_hadHVScalesDif[sample], eta, phi, diffMean);
	        }
	      }
	    }
	  }
	}
      }
    }
  }

  LWHist::safeDelete(m_h_emHVEntry);
  LWHist::safeDelete(m_h_hadHVEntry);
  for (int i = 0; i < s_numEmHVPlots; ++i) {
    LWHist::safeDelete(m_v_emHVEntries[i]);
  }
  for (int i = 0; i < s_numHadHVPlots; ++i) {
    LWHist::safeDelete(m_v_hadHVEntries[i]);
  }
  m_v_emHVEntries.clear();
  m_v_hadHVEntries.clear();

  return StatusCode::SUCCESS;     
}
/*---------------------------------------------------------*/
StatusCode L1CaloHVScalesMon::procHistograms()
/*---------------------------------------------------------*/
{
  //if ( endOfLumiBlockFlag() ) { }
	
  //if ( endOfRunFlag() ) { }
  
  return StatusCode::SUCCESS;
}

// Return receiver (0/1) FCAL2/3 cell belongs to.
// Adapted from L1CaloOfflineTriggerTowerTools::sortFCAL23Cells(CaloCells, eta)

unsigned int L1CaloHVScalesMon::fcalReceiver(const CaloDetDescrElement* caloDDE, double eta)
{
  double absEta = fabs(eta);
  CaloCell_ID::CaloSample sampling = caloDDE->getSampling();
  // _raw is ideal position
  double cellEta = fabs(caloDDE->eta_raw());
  double x = fabs(caloDDE->x_raw());
  double y = fabs(caloDDE->y_raw());
  bool lowEta = true;

  // FCAL 2
  if (sampling == CaloCell_ID::FCAL1) {
    // TTs split the eta range equally, hence the apparent inconsistencies in next line
    lowEta = (absEta < 4.05 && cellEta < 3.3) || (absEta > 4.05 && cellEta < 3.97);
    // Exceptions, based on radial map
    if ((x > 100. && x < 160. && y > 340. && y < 380.) ||
        (x > 320. && x < 360. && y > 170. && y < 210.) ||
        (x >  30. && x <  60. && y > 170. && y < 210.)) lowEta = true;
    if ((x > 160. && x < 190. && y > 340. && y < 380.) ||
        (x > 230. && x < 260. && y > 300. && y < 340.) ||
        (x > 130. && x < 160. && y > 140. && y < 170.) ||
        (x > 160. && x < 200. && y >  90. && y < 140.)) lowEta = false;
  }
  // FCAL 3
  else if (sampling == CaloCell_ID::FCAL2) {
    lowEta = (absEta < 4.05 && cellEta < 3.43) || (absEta > 4.05 && cellEta < 3.9);
    // Exceptions
    double xs = caloDDE->x_raw();
    double ys = caloDDE->y_raw();
    if ((x  >  120. && x  <  160. && y  >  140. && y  <  200.) ||
        (x  >  160. && x  <  220. && y  >  100. && y  <  140.) ||
        (xs >  120. && xs <  160. && ys > -140. && ys < -100.) ||
        (xs > -160. && xs < -100. && ys >  100. && ys <  140.)) lowEta = true;
    if ((x  >    0. && x  <   60. && y  >  380. && y  <  420.) ||
        (xs >  320. && xs <  370. && ys >  140. && ys <  190.) ||
        (xs > -360. && xs < -320. && ys > -180. && ys < -140.)) lowEta = false;
  }
  return (lowEta) ? 0 : 1;
}

// Return receiver ID(s) for given coolID
// Adapted from L1CaloOfflineTriggerTowerTools::emRxId(..)

std::vector<unsigned int> L1CaloHVScalesMon::emRxId(const L1CaloCoolChannelId& coolId)
{

  std::vector<L1CaloRxCoolChannelId> rx = m_ttSvc->cnvCoolChannelIdToRxId(coolId);
  std::vector<unsigned int> output;

  // Sort EM Crack recievers. End cap first, barrel second
  if(rx.size() == 3){

    // For the EM crack region, there are 3 recievers.
    // 1 Rx have Crate 0 or 1  - correspond to the barrel before the Octopus cable magic - ignore
    // 2 Rx have Crate 2 or 3  - correspond to the Rx we want:
    //      1 Rx has inputConn() == 0 - EMEC receiver
    //      1 Rx has inputConn() == 2 - Barrel receiver

    typedef std::vector<L1CaloRxCoolChannelId>::const_iterator Itr_Id;
    std::vector<L1CaloRxCoolChannelId> rx2;
    for(Itr_Id a=rx.begin();a!=rx.end();++a){
      if((*a).crate() == 2 || (*a).crate() == 3){ // Select only interesting recievers
        rx2.push_back(*a);
      }
    }

    if(rx2.size() == 2){

      unsigned int inputA = rx2.at(0).inputConn();
      unsigned int inputB = rx2.at(1).inputConn();

      // End cap first, barrel second
      if(inputA == 2 && inputB == 0){
        output.push_back(rx2.at(1).id());
        output.push_back(rx2.at(0).id());
      }
      if(inputA == 0 && inputB == 2){
        output.push_back(rx2.at(0).id());
        output.push_back(rx2.at(1).id());
      }
    }

  }

  if(rx.size() == 1){
    output.push_back(rx.at(0).id());
  }

  return output;
}

// Return receiver ID(s) for given coolID
// Adapted from L1CaloOfflineTriggerTowerTools::hadRxId(..)

std::vector<unsigned int> L1CaloHVScalesMon::hadRxId(const L1CaloCoolChannelId& coolId)
{

  std::vector<L1CaloRxCoolChannelId> rx = m_ttSvc->cnvCoolChannelIdToRxId(coolId);
  std::vector<unsigned int> output;

  // Sort FCAL 23 recievers. Always have -ve eta first and +ve eta second
  if(rx.size() == 2){

    // Odd receiver is lowest |eta|
    unsigned int inputPairA = rx.at(0).inputPair();
    unsigned int inputPairB = rx.at(1).inputPair();

    bool oddA(false),oddB(false);
    if(inputPairA % 2 != 0) oddA = true;
    if(inputPairB % 2 != 0) oddB = true;

    if(oddA == true && oddB == false){
      output.push_back(rx.at(0).id());
      output.push_back(rx.at(1).id());
    }
    if(oddA == false && oddB == true){
      output.push_back(rx.at(1).id());
      output.push_back(rx.at(0).id());
    }

  }

  if(rx.size() == 1){
    output.push_back(rx.at(0).id());
  }

  return output;
}
// ============================================================================
} // end namespace
// ============================================================================
