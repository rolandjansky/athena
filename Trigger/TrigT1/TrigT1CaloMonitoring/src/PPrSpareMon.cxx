/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     PPrSpareMon.cxx
// PACKAGE:  TrigT1CaloMonitoring  
//
// AUTHOR:   Peter Faulkner
//           
//
// ********************************************************************

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile2D_LW.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloUtils/DataError.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "PPrSpareMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
PPrSpareMon::PPrSpareMon(const std::string & type, const std::string & name,
					           const IInterface* parent)
  : ManagedMonitorToolBase ( type, name, parent ),
    m_SliceNo(15), m_histBooked(false),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_h_ppmspare_2d_tt_adc_HitMap(0),
    m_h_ppmspare_2d_tt_adc_ProfileMap(0),
    m_h_ppmspare_1d_ErrorSummary(0),
    m_h_ppmspare_2d_Status25(0),
    m_h_ppmspare_2d_ErrorField25(0),
    m_v_ppmspare_2d_ASICErrorsDetail(0),
    m_h_ppmspare_2d_ErrorEventNumbers(0),
    m_h_ppmspare_2d_ASICErrorEventNumbers(0),
    m_h_ppmspare_1d_tt_adc_TriggeredSlice(0)
/*---------------------------------------------------------*/
{
  declareProperty("BS_TriggerTowerContainer",
                  m_TriggerTowerContainerName = "TriggerTowersSpare");
  declareProperty("BS_xAODTriggerTowerContainer",
                  m_xAODTriggerTowerContainerName = LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation + "Spare");  
  declareProperty("ADCHitMap_Thresh",  m_TT_ADC_HitMap_Thresh = 40,
                  "ADC cut for hitmaps");

  declareProperty("PathInRootFile",
                  m_PathInRootFile="L1Calo/PPM/SpareChannels") ;
  declareProperty("ErrorPathInRootFile",
                  m_ErrorPathInRootFile="L1Calo/PPM/SpareChannels/Errors") ;

}

/*---------------------------------------------------------*/
PPrSpareMon::~PPrSpareMon()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode PPrSpareMon::initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  StatusCode sc;

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  sc = m_errorTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool"
                    << endmsg;
    return sc;
  }

  sc = m_histTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool"
                    << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode PPrSpareMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "in PPrSpareMon::bookHistograms" << endmsg;

  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
	
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  //if ( newLumiBlock) { }

  if ( newRun ) {

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    MonGroup TT_ADC(this, m_PathInRootFile+"/ADC", run, attr);
    MonGroup TT_Error(this, m_ErrorPathInRootFile, run, attr);
    MonGroup TT_ErrorEvents(this, m_ErrorPathInRootFile, run, attr, "",
                                                                "eventSample" );
    MonGroup TT_ErrorDetail(this, m_ErrorPathInRootFile+"/Detail", run, attr);

    std::string name,title;
    std::stringstream buffer;

    //------------------- ADC Hitmaps for Triggered Timeslice ----------------

    m_histTool->setMonGroup(&TT_ADC);

    buffer.str("");
    buffer << m_TT_ADC_HitMap_Thresh;

    title = "Spare Channels Hit Map of FADC > " + buffer.str()
                                                + " for Triggered Timeslice";
    m_h_ppmspare_2d_tt_adc_HitMap = m_histTool->bookPPMCrateModuleVsSubmoduleChannel(
                         "ppmspare_2d_tt_adc_HitMap", title, 2, 5);
    title = "Spare Channels Profile Map of FADC for Triggered Timeslice";
    m_h_ppmspare_2d_tt_adc_ProfileMap = m_histTool->bookProfilePPMCrateModuleVsSubmoduleChannel(
                         "ppmspare_2d_tt_adc_ProfileMap", title, 2, 5);

    //-------------------------Summary of Errors------------------------------

    m_histTool->setMonGroup(&TT_Error);

    m_h_ppmspare_1d_ErrorSummary = m_histTool->book1F("ppmspare_1d_ErrorSummary",
                   "Spare Channels Summary of SubStatus Errors", 8, 0., 8.);
    m_histTool->subStatus(m_h_ppmspare_1d_ErrorSummary);

    //---------------------- SubStatus Word errors ---------------------------
      
    //L1Calo Substatus word
    m_h_ppmspare_2d_Status25 = m_histTool->bookPPMSubStatusVsCrateModule(
      "ppmspare_2d_Status25",
      "Spare Channels: Errors from TT SubStatus Word (crates 2-5)", 2, 5);

    //error bit field from ASIC data
    m_h_ppmspare_2d_ErrorField25 = m_histTool->bookPPMErrorsVsCrateModule(
      "ppmspare_2d_ErrorField25",
      "Spare Channels: Errors from ASIC error field (crates 2-5)", 2, 5);

    m_histTool->setMonGroup(&TT_ErrorEvents);

    m_h_ppmspare_2d_ErrorEventNumbers = m_histTool->bookEventNumbers(
      "ppmspare_2d_ErrorEventNumbers",
      "Spare Channels SubStatus Error Event Numbers", 8, 0., 8.);
    m_histTool->subStatus(m_h_ppmspare_2d_ErrorEventNumbers, 0, false);
    m_h_ppmspare_2d_ASICErrorEventNumbers = m_histTool->bookEventNumbers(
      "ppmspare_2d_ASICErrorEventNumbers",
      " Spare Channels ASIC Error Field Event Numbers", 8, 0., 8.);
    m_histTool->ppmErrors(m_h_ppmspare_2d_ASICErrorEventNumbers, 0, false);

    m_histTool->setMonGroup(&TT_ErrorDetail);

    m_v_ppmspare_2d_ASICErrorsDetail.clear();
    std::vector<std::string> errNames;
    errNames.push_back("Channel0Disabled");
    errNames.push_back("Channel1Disabled");
    errNames.push_back("Channel2Disabled");
    errNames.push_back("Channel3Disabled");
    errNames.push_back("MCMAbsent");
    errNames.push_back("");
    errNames.push_back("Timeout");
    errNames.push_back("ASICFull");
    errNames.push_back("EventMismatch");
    errNames.push_back("BunchMismatch");
    errNames.push_back("FIFOCorrupt");
    errNames.push_back("PinParity");
    for (int error = 0; error < 12; error+=2) {
      for (int crate = 2; crate < 6; crate+=2) {
	buffer.str("");
	buffer << crate;
	name = "ppmspare_2d_"+errNames[error]+errNames[error+1]
	                     +"Crate"+buffer.str();
	title = "ASIC Errors "+errNames[error]+" "
	                      +errNames[error+1]+" for Crates "+buffer.str();
	buffer.str("");
	buffer << (crate+1);
	name += buffer.str();
	title += "-"+buffer.str();
	const int nbins = (error != 4) ? 32 : 16;
	TH2F_LW* hist = m_histTool->book2F(name,title,nbins,0,nbins,32,0,32);
	m_histTool->numbers(hist, 0, 15, 2);
	LWHist::LWHistAxis* axis = hist->GetXaxis();
	axis->SetBinLabel(1, errNames[error].c_str());
	if (error != 4) {
	  m_histTool->numbers(hist, 0, 15, 2, 16);
	  axis->SetBinLabel(17, errNames[error+1].c_str());
        }
	axis->SetTitle("MCM");
	m_histTool->ppmCrateModule(hist, crate, crate+1, 0, false);
	m_v_ppmspare_2d_ASICErrorsDetail.push_back(hist);
      }
    }
	  
    //--------------------- number of triggered slice ------------------------
    m_histTool->setMonGroup(&TT_ADC);
    
    m_h_ppmspare_1d_tt_adc_TriggeredSlice = m_histTool->book1F(
      "ppmspare_1d_tt_adc_TriggeredSlice",
      "Spare Channels Number of the Triggered Slice", m_SliceNo, 0, m_SliceNo);
    m_histTool->numbers(m_h_ppmspare_1d_tt_adc_TriggeredSlice, 0, m_SliceNo-1);
     
    m_histTool->unsetMonGroup();
    m_histBooked = true;
  }	

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode PPrSpareMon::fillHistograms()
/*---------------------------------------------------------*/
{
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

  // Error vector for global overview
  std::vector<int> overview(8);

  //Retrieve TriggerTowers from SG
  StatusCode sc;
  const xAOD::TriggerTowerContainer_v2* TriggerTowerTES = 0; 
  if (evtStore()->contains<xAOD::TriggerTowerContainer_v2>(m_xAODTriggerTowerContainerName)) {
    sc = evtStore()->retrieve(TriggerTowerTES, m_xAODTriggerTowerContainerName); 
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !TriggerTowerTES) {
    if (debug) msg(MSG::DEBUG) << "No TriggerTower found in TES at "
                               << m_TriggerTowerContainerName<< endmsg ;
    return StatusCode::SUCCESS;
  }

    
  // =========================================================================
  // ================= Container: TriggerTower ===============================
  // =========================================================================

  xAOD::TriggerTowerContainer_v2::const_iterator TriggerTowerIterator =
                                                     TriggerTowerTES->begin(); 
  xAOD::TriggerTowerContainer_v2::const_iterator TriggerTowerIteratorEnd =
                                                     TriggerTowerTES->end(); 
 
  for (; TriggerTowerIterator != TriggerTowerIteratorEnd;
                                                     ++TriggerTowerIterator) {

    //---------------------------- ADC HitMaps -------------------------------

    const double crateModule      = (*TriggerTowerIterator)->eta();
    const double submoduleChannel = (*TriggerTowerIterator)->phi();
    const int icm       = crateModule;
    const int isc       = submoduleChannel;
    const int crate     = icm/16;
    const int module    = icm%16;
    const int submodule = isc/4;
    const int channel   = isc%4;

    if (crate < 2 || crate > 5) continue;
    
    const short unsigned int adc = (*TriggerTowerIterator)->adc()[
                                         (*TriggerTowerIterator)->adcPeak()];
    if (adc > m_TT_ADC_HitMap_Thresh) {
      m_h_ppmspare_2d_tt_adc_HitMap->Fill(crateModule-32., submoduleChannel, 1);
    }
    m_h_ppmspare_2d_tt_adc_ProfileMap->Fill(crateModule-32., submoduleChannel, adc);

    //------------------------ SubStatus Word errors -------------------------

    if ((*TriggerTowerIterator)-> errorWord()) {

      using LVL1::DataError;
      const DataError error((*TriggerTowerIterator)-> errorWord());
   
      //Summary

      int ypos = module+(crate-2)*16;

      for (int bit = 0; bit < 8; ++bit) {
        if (error.get(bit + DataError::ChannelDisabled)) {
          m_h_ppmspare_2d_ErrorField25->Fill(bit, ypos);
 	  m_histTool->fillEventNumber(m_h_ppmspare_2d_ASICErrorEventNumbers, bit);
        }
        if (error.get(bit + DataError::GLinkParity)) {
          m_h_ppmspare_2d_Status25->Fill(bit, ypos);
	  m_h_ppmspare_1d_ErrorSummary->Fill(bit);
	  m_histTool->fillEventNumber(m_h_ppmspare_2d_ErrorEventNumbers, bit);
        }
      }

      // Detailed plots by MCM
      ypos = (crate%2)*16+module;
      const int index = (crate-2)/2;
      if (error.get(DataError::ChannelDisabled)) {
        m_v_ppmspare_2d_ASICErrorsDetail[(channel/2)*4+index]->Fill(
                                               (channel%2)*16+submodule, ypos);
      }
      if (error.get(DataError::MCMAbsent)) {
        m_v_ppmspare_2d_ASICErrorsDetail[4+index]->Fill(submodule, ypos);
      }
      if (error.get(DataError::Timeout)) {
        m_v_ppmspare_2d_ASICErrorsDetail[6+index]->Fill(submodule, ypos);
      }
      if (error.get(DataError::ASICFull)) {
        m_v_ppmspare_2d_ASICErrorsDetail[6+index]->Fill(16+submodule, ypos);
      }
      if (error.get(DataError::EventMismatch)) {
        m_v_ppmspare_2d_ASICErrorsDetail[8+index]->Fill(submodule, ypos);
      }
      if (error.get(DataError::BunchMismatch)) {
        m_v_ppmspare_2d_ASICErrorsDetail[8+index]->Fill(16+submodule, ypos);
      }
      if (error.get(DataError::FIFOCorrupt)) {
        m_v_ppmspare_2d_ASICErrorsDetail[10+index]->Fill(submodule, ypos);
      }
      if (error.get(DataError::PinParity)) {
        m_v_ppmspare_2d_ASICErrorsDetail[10+index]->Fill(16+submodule, ypos);
      }

      if (error.get(DataError::ChannelDisabled) ||
          error.get(DataError::MCMAbsent)) overview[crate] |= 1;

      if (error.get(DataError::Timeout)       ||
          error.get(DataError::ASICFull)      ||
          error.get(DataError::EventMismatch) ||
	  error.get(DataError::BunchMismatch) ||
          error.get(DataError::FIFOCorrupt)   ||
	  error.get(DataError::PinParity)) overview[crate] |= (1 << 1);

      if (error.get(DataError::GLinkParity)   ||
          error.get(DataError::GLinkProtocol) ||
          error.get(DataError::FIFOOverflow)  ||
	  error.get(DataError::ModuleError)   ||
          error.get(DataError::GLinkDown)     ||
	  error.get(DataError::GLinkTimeout)  ||
          error.get(DataError::BCNMismatch)) overview[crate] |= (1 << 2);
     
     }

     // number of triggered slice
     m_h_ppmspare_1d_tt_adc_TriggeredSlice->Fill((*TriggerTowerIterator)->adcPeak(),1);

  }	     
     
  // Write overview vector to StoreGate
  //std::vector<int>* save = new std::vector<int>(overview);
  //sc = evtStore()->record(save, "L1CaloPPMSpareErrorVector");
  //if (sc != StatusCode::SUCCESS)
  //  {
  //    msg(MSG::ERROR) << "Error recording PPMSpare error vector in TES "
  //                    << endmsg;
  //    return sc;
  //  }
  
  return StatusCode::SUCCESS;
}


   
/*---------------------------------------------------------*/
StatusCode PPrSpareMon::procHistograms()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "in procHistograms" << endmsg ;

  //if( endOfLumiBlock || endOfRun ) { }
	
  return StatusCode::SUCCESS;
}

// ============================================================================
}  // end namespace
// ============================================================================
