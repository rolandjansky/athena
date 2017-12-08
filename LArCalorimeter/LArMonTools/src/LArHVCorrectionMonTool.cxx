/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     LArHVCorrectionMonTool.cxx
// PACKAGE:  LArMonTools
//
// AUTHOR:   Jessica Leveque
//
// Class for monitoring offline HV corrections based in DCS infos
// ********************************************************************

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "Identifier/IdentifierHash.h"

#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <functional>
#include <map>
#include <utility>

#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "TMath.h"


#include "LArMonTools/LArHVCorrectionMonTool.h"
#include "xAODEventInfo/EventInfo.h"

using namespace std;

/*---------------------------------------------------------*/
LArHVCorrectionMonTool::LArHVCorrectionMonTool(const std::string& type, 
					       const std::string& name,
					       const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_LArOnlineIDHelper(0),
    m_LArEM_IDHelper(0),
    m_LArFCAL_IDHelper(0),
    m_LArHEC_IDHelper(0),
    m_caloIdMgr(0),
    m_CaloDetDescrMgr(0),
    m_strHelper(0),
    m_rootStore(0),
    m_hvCorrTool("LArHVCorrTool"),
    m_keyHVScaleCorr("LArHVScaleCorr"),
    m_eventsCounter(0)
{
  declareProperty("LArRawChannelKey",m_channelKey="LArRawChannels");
  declareProperty("HVCorrTool",m_hvCorrTool);
  declareProperty("keyHVScaleCorr",m_keyHVScaleCorr);
  declareProperty("ErrorThreshold",m_threshold=0.02);
  declareProperty("EtaGranularity",m_delta_eta=0.01);
  declareProperty("PhiGranularity",m_delta_phi=0.01);
  declareProperty("NumberOfLBs",m_nlumi_blocks=1500);
  
  for( unsigned i = 0; i < 2; ++i )
  {
    m_hLArHVCorrectionEMB[i]		= NULL;
    m_hLArHVCorrectionEMEC[i]		= NULL;
    m_hLArHVCorrectionHEC[i]		= NULL;
    m_hLArHVCorrectionFCAL[i]		= NULL;  
    m_hNDeviatingChannelsEMB[i]		= NULL;
    m_hNDeviatingChannelsEMEC[i]	= NULL;
    m_hNDeviatingChannelsHEC[i]		= NULL;
    m_hNDeviatingChannelsFCAL[i]	= NULL;
  }
  
}

/*---------------------------------------------------------*/
LArHVCorrectionMonTool::~LArHVCorrectionMonTool()
{
}

/*---------------------------------------------------------*/
StatusCode LArHVCorrectionMonTool::initialize()
{
  
  ATH_MSG_INFO( "Initialize LArHVCorrectionMonTool" );
  StatusCode sc;
  
  sc = detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID");
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get LArOnlineIDHelper" );
    return sc;
  }
  
  // Retrieve HVCorrTool 
  sc = m_hvCorrTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to find tool for LArHVCorrTool" );
    return StatusCode::FAILURE;
  }
  
  // Retrieve HV Correction reference
  sc = detStore()->regHandle(m_dd_HVScaleCorr,m_keyHVScaleCorr);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to register handle to HVScaleCorr " );
    return StatusCode::FAILURE;
  }
  
  // Retrieve ID helpers
  sc =  detStore()->retrieve( m_caloIdMgr );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get CaloIdMgr" );
    return sc;
  }
  m_LArEM_IDHelper   = m_caloIdMgr->getEM_ID();
  m_LArHEC_IDHelper  = m_caloIdMgr->getHEC_ID();
  m_LArFCAL_IDHelper = m_caloIdMgr->getFCAL_ID();
  
  // CaloDetDescrMgr gives "detector description", including real positions of cells
  sc = detStore()->retrieve(m_CaloDetDescrMgr);
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get CaloDetDescrMgr ");
    return sc;
  }
  
  // Get LAr Cabling Service
  sc=m_larCablingService.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve LArCablingService" );
    return StatusCode::FAILURE;
  }
  
  // LArOnlineIDStrHelper
  m_strHelper = new  LArOnlineIDStrHelper(m_LArOnlineIDHelper);
  m_strHelper->setDefaultNameType(LArOnlineIDStrHelper::LARONLINEID);
  
  // End Initialize
  ManagedMonitorToolBase::initialize().ignore();
  ATH_MSG_DEBUG( "Successful Initialize LArHVCorrection " );
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode 
LArHVCorrectionMonTool::bookHistograms()
{
  ATH_MSG_DEBUG( "in bookHists()" );
  
  //  if(isNewRun){
    
    // Create top folder for histos
    MonGroup generalGroup( this, "/LAr/HVCorrection", run, ATTRIB_MANAGED, "", "weightedAverage" );
    
    // EM Calorimeter - Barrel
    m_hLArHVCorrectionEMB[0] = TH2F_LW::create("LArHVCorrectionEMBA",
					       "HV deviation (nominal - corrected) - EMBA", 
					       int(1.6/m_delta_eta), 0.,1.6,int(6.3/m_delta_phi),-3.15, 3.15);
    SetTH2Style(m_hLArHVCorrectionEMB[0]);
    generalGroup.regHist(m_hLArHVCorrectionEMB[0]).ignore();
    
    m_hLArHVCorrectionEMB[1] = TH2F_LW::create("LArHVCorrectionEMBC",
					       "HV deviation (nominal - corrected) - EMBC", 
					       int(1.6/m_delta_eta),-1.61, -0.01,int(6.3/m_delta_phi),-3.15, 3.15);
    SetTH2Style(m_hLArHVCorrectionEMB[1]);
    generalGroup.regHist(m_hLArHVCorrectionEMB[1]).ignore();
    
    m_hNDeviatingChannelsEMB[0] = TH1I_LW::create("NDeviatingChannelsEMBA",
						  Form("Number of channels per LB with HV corr>%.0f%% - EMBA",m_threshold*100), 
						  m_nlumi_blocks, 0.5, double(m_nlumi_blocks)+0.5);
    SetTH1Style(m_hNDeviatingChannelsEMB[0]);
    generalGroup.regHist(m_hNDeviatingChannelsEMB[0]).ignore();
    
    m_hNDeviatingChannelsEMB[1] = TH1I_LW::create("NDeviatingChannelsEMBC",
						  Form("Number of channels per LB with HV corr>%.0f%% - EMBC",m_threshold*100), 
						  m_nlumi_blocks, 0.5, double(m_nlumi_blocks)+0.5);
    SetTH1Style(m_hNDeviatingChannelsEMB[1]);
    generalGroup.regHist(m_hNDeviatingChannelsEMB[1]).ignore();
    
    // EM Calorimeter - Endcap
    m_hLArHVCorrectionEMEC[0] = TH2F_LW::create("LArHVCorrectionEMECA",
						"HV deviation (nominal - corrected) - EMECA", 
						int(1.8/m_delta_eta),1.375,3.2,int(6.3/m_delta_phi),-3.15, 3.15);
    SetTH2Style(m_hLArHVCorrectionEMEC[0]);
    generalGroup.regHist(m_hLArHVCorrectionEMEC[0]).ignore();
    
    m_hLArHVCorrectionEMEC[1] = TH2F_LW::create("LArHVCorrectionEMECC",
						"HV deviation (nominal - corrected) - EMECC", 
						int(1.8/m_delta_eta),-3.21,-1.385,int(6.3/m_delta_phi),-3.15, 3.15);
    SetTH2Style(m_hLArHVCorrectionEMEC[1]);
    generalGroup.regHist(m_hLArHVCorrectionEMEC[1]).ignore();
    
    m_hNDeviatingChannelsEMEC[0] = TH1I_LW::create("NDeviatingChannelsEMECA",
						   Form("Number of channels per LB with HV corr>%.0f%% - EMECA",m_threshold*100),
						   m_nlumi_blocks, 0.5, double(m_nlumi_blocks)+0.5);
    SetTH1Style(m_hNDeviatingChannelsEMEC[0]);
    generalGroup.regHist(m_hNDeviatingChannelsEMEC[0]).ignore();
    
    m_hNDeviatingChannelsEMEC[1] = TH1I_LW::create("NDeviatingChannelsEMECC",
						   Form("Number of channels per LB with HV corr>%.0f%% - EMECC",m_threshold*100),
						   m_nlumi_blocks, 0.5, double(m_nlumi_blocks)+0.5);
    SetTH1Style(m_hNDeviatingChannelsEMEC[1]);
    generalGroup.regHist(m_hNDeviatingChannelsEMEC[1]).ignore();
    
    // HEC
    m_hLArHVCorrectionHEC[0] = TH2F_LW::create("LArHVCorrectionHECA",
					       "HV deviation (nominal - corrected) - HECA", 
					       int(1.7/m_delta_eta),1.5, 3.2,int(6.3/m_delta_phi), -3.15, 3.15);
    
    SetTH2Style(m_hLArHVCorrectionHEC[0]);
    generalGroup.regHist(m_hLArHVCorrectionHEC[0]).ignore();
    
    
    m_hLArHVCorrectionHEC[1] = TH2F_LW::create("LArHVCorrectionHECC",
					       "HV deviation (nominal - corrected) - HECC", 
					       int(1.7/m_delta_eta),-3.21, -1.51, int(6.3/m_delta_phi), -3.15, 3.15);
    SetTH2Style(m_hLArHVCorrectionHEC[1]);
    generalGroup.regHist(m_hLArHVCorrectionHEC[1]).ignore();
    
    m_hNDeviatingChannelsHEC[0] = TH1I_LW::create("NDeviatingChannelsHECA",
						  Form("Number of channels per LB with HV corr>%.0f%% - HECA",m_threshold*100),
						  m_nlumi_blocks, 0.5, double(m_nlumi_blocks)+0.5);
    SetTH1Style(m_hNDeviatingChannelsHEC[0]);
    generalGroup.regHist(m_hNDeviatingChannelsHEC[0]).ignore();
    
    m_hNDeviatingChannelsHEC[1] = TH1I_LW::create("NDeviatingChannelsHECC",
						  Form("Number of channels per LB with HV corr>%.0f%% - HECC",m_threshold*100),
						  m_nlumi_blocks, 0.5, double(m_nlumi_blocks)+0.5);
    SetTH1Style(m_hNDeviatingChannelsHEC[1]);
    generalGroup.regHist(m_hNDeviatingChannelsHEC[1]).ignore();
    
    // FCAL
    m_hLArHVCorrectionFCAL[0] = TH2F_LW::create("LArHVCorrectionFCALA",
						"HV deviation (nominal - corrected) - FCalA", 
						int(3./m_delta_eta), 2.9, 4.9,int(6.4/m_delta_phi), -3.2, 3.2);
    SetTH2Style(m_hLArHVCorrectionFCAL[0]);
    generalGroup.regHist(m_hLArHVCorrectionFCAL[0]).ignore();
    
    m_hLArHVCorrectionFCAL[1] = TH2F_LW::create("LArHVCorrectionFCALC",
						"HV deviation (nominal - corrected) - FCalC", 
						int(3./m_delta_eta), -4.91, -3.11, int(6.4/m_delta_phi), -3.2, 3.2);
    SetTH2Style(m_hLArHVCorrectionFCAL[1]);
    generalGroup.regHist(m_hLArHVCorrectionFCAL[1]).ignore();
    
    m_hNDeviatingChannelsFCAL[0] = TH1I_LW::create("NDeviatingChannelsFCALA",
						   Form("Number of channels with HV corr>%.0f%% - FCALA",m_threshold*100),
						   m_nlumi_blocks, 0.5, double(m_nlumi_blocks)+0.5);
    SetTH1Style(m_hNDeviatingChannelsFCAL[0]);
    generalGroup.regHist(m_hNDeviatingChannelsFCAL[0]).ignore();
    
    m_hNDeviatingChannelsFCAL[1] = TH1I_LW::create("NDeviatingChannelsFCALC",
						   Form("Number of channels with HV corr>%.0f%% - FCALC",m_threshold*100),
						   m_nlumi_blocks, 0.5, double(m_nlumi_blocks)+0.5);
    SetTH1Style(m_hNDeviatingChannelsFCAL[1]);
    generalGroup.regHist(m_hNDeviatingChannelsFCAL[1]).ignore();
    
    //return StatusCode::SUCCESS;
    
    //  }// end isNewRun
  
  // Reset event counter at the start of each LB 
  if(newLumiBlockFlag()){
    
    m_eventsCounter = 0;
    
  }
  
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode 
LArHVCorrectionMonTool::fillHistograms()
{
  ATH_MSG_DEBUG( "in fillHists()" );
  
  m_eventsCounter++;
  
  if(m_eventsCounter == 1){ // Fill only at the beginning of LB. m_eventsCounter is reset at the begining of each LB.
    
    // Retrieve event information
    const xAOD::EventInfo* thisEventInfo;
    if (evtStore()->retrieve(thisEventInfo).isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve EventInfo object" );
      return StatusCode::FAILURE;
    }
    //const DataHandle<EventInfo> event_info;
    //EventID::number_type bunch_crossing_id = event_info->event_ID()->bunch_crossing_id();
    //EventID::number_type lumi_block        = event_info->event_ID()->lumi_block();
    int lumi_block = thisEventInfo->lumiBlock();
    
    // Counter for deviating channels in each partition
    float nonNominal[] = {0.,0.,0.,0.,0.,0.,0.,0.};
    
    // Retrieve Raw Channels Container
    const LArRawChannelContainer* pRawChannelsContainer;
    StatusCode sc = evtStore()->retrieve(pRawChannelsContainer, m_channelKey);
    if(sc.isFailure()) {
      ATH_MSG_WARNING( "Can't retrieve LArRawChannelContainer with key " << m_channelKey );
      return StatusCode::SUCCESS;
    }
    
    // Loop over LArRawChannels
    //SelectAllLArRawChannels AllRaw(pRawChannelsContainer);
    //for (SelectAllLArRawChannels::const_iterator itRaw = AllRaw.begin(); itRaw != AllRaw.end(); ++itRaw) {
    //const LArRawChannel* pRawChannel = (*itRaw) ;
    //FIXME: If we loop over CaloCells we get the offline identifer directly! 
    //Or: Why don't loop over channels w/o retrieving any event data object?
    for (const LArRawChannel& rawChannel : *pRawChannelsContainer) {
      HWIdentifier id  = rawChannel.hardwareID();
      //CaloGain::CaloGain gain = pRawChannel->gain();
      Identifier offlineID = m_larCablingService->cnvToIdentifier(id);
      
      // Skip disconnected channels
      if(!m_larCablingService->isOnlineConnected(id)) continue;
      
      // Get Physical Coordinates
      float etaChan = 0; float phiChan = 0.;
      const CaloDetDescrElement* caloDetElement = m_CaloDetDescrMgr->get_element(offlineID);
      if(caloDetElement == 0 ){
	ATH_MSG_ERROR( "Cannot retrieve (eta,phi) coordinates for raw channels" );
	continue; 
      }else{
	etaChan = caloDetElement->eta_raw();
	phiChan = caloDetElement->phi_raw();
      }
      
      // Fix phi range in HEC
      if (m_LArOnlineIDHelper->isHECchannel(id)) phiChan = CaloPhiRange::fix(phiChan);
      
      // Retrieve HV correction info
      float hvdev = 0;
      float hvcorr = m_hvCorrTool->Scale(offlineID);
      //ATH_MSG_VERBOSE( "hvcorr" << hvcorr );
      float hvonline = m_dd_HVScaleCorr->HVScaleCorr(offlineID);
      if (hvonline<=0) continue; //No valid online correction
      //ATH_MSG_VERBOSE( "hvonline" << hvonline );
      if (hvcorr>hvonline) hvdev = hvonline-hvcorr; //Monitor only channels that get a higher correction from DCS (eg are at a lower voltage). 

      //ATH_MSG_VERBOSE( "hvdev" << hvdev );
      
    if (fabs(hvdev/hvonline)> m_threshold){
	
	// A-Side
	if(etaChan >= 0){
	  if(m_LArOnlineIDHelper->isEMBchannel(id)){
	    nonNominal[0]++;
	    m_hLArHVCorrectionEMB[0]->Fill(etaChan,phiChan, hvdev);
	  }
	  if(m_LArOnlineIDHelper->isEMECchannel(id)){
	    nonNominal[1]++;
	    m_hLArHVCorrectionEMEC[0]->Fill(etaChan,phiChan, hvdev);
	  }
	  if(m_LArOnlineIDHelper->isHECchannel(id)){
	    nonNominal[2]++;
	    m_hLArHVCorrectionHEC[0]->Fill(etaChan,phiChan, hvdev);
	  }
	  if(m_LArOnlineIDHelper->isFCALchannel(id)){
	    nonNominal[3]++;
	    m_hLArHVCorrectionFCAL[0]->Fill(etaChan,phiChan, hvdev);
	  }
	  
	  // C-side
	} else {
	  if(m_LArOnlineIDHelper->isEMBchannel(id)){
	    nonNominal[4]++;
	    m_hLArHVCorrectionEMB[1]->Fill(etaChan,phiChan, hvdev);
	  }
	  if(m_LArOnlineIDHelper->isEMECchannel(id)){
	    nonNominal[5]++;
	    m_hLArHVCorrectionEMEC[1]->Fill(etaChan,phiChan, hvdev);
	  }
	  if(m_LArOnlineIDHelper->isHECchannel(id)){
	    //std::cout << "HECC, hvdev=" << hvdev << ", onlCorr=" << hvonline << ", oflCorr=" << hvcorr << std::endl;
	    nonNominal[6]++;
	    m_hLArHVCorrectionHEC[1]->Fill(etaChan,phiChan, hvdev);
	  }
	  if(m_LArOnlineIDHelper->isFCALchannel(id)){
	    nonNominal[7]++;
	    m_hLArHVCorrectionFCAL[1]->Fill(etaChan,phiChan, hvdev);
	  }
	}
      }
    }// end Raw Channels Loop
    
    // Fill number of problematic cell per LUMI_BLOCK
    m_hNDeviatingChannelsEMB[0]->Fill(lumi_block,nonNominal[0]);
    m_hNDeviatingChannelsEMEC[0]->Fill(lumi_block,nonNominal[1]);
    m_hNDeviatingChannelsHEC[0]->Fill(lumi_block,nonNominal[2]);
    m_hNDeviatingChannelsFCAL[0]->Fill(lumi_block,nonNominal[3]);
    
    m_hNDeviatingChannelsEMB[1]->Fill(lumi_block,nonNominal[4]);
    m_hNDeviatingChannelsEMEC[1]->Fill(lumi_block,nonNominal[5]);
    m_hNDeviatingChannelsHEC[1]->Fill(lumi_block,nonNominal[6]);
    m_hNDeviatingChannelsFCAL[1]->Fill(lumi_block,nonNominal[7]);
    
  } else {
    
    return StatusCode::SUCCESS;
    
  }// End of event treatment
  
  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/
StatusCode LArHVCorrectionMonTool::procHistograms()
{
  
  ATH_MSG_DEBUG( "In procHistograms " );
  
//  //
//  // Normalize and fix bins only at the end of a block, and only ONCE ! Otherwise it's a mess
//  //
//  
//  if (isEndOfEventsBlock || isEndOfLumiBlock || isEndOfRun){
//    
//    return StatusCode::SUCCESS;
//    
//  }
  
  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/
void LArHVCorrectionMonTool::SetTH2Style(TH2F_LW* h){
  
  h->GetXaxis()->SetTitle("cell #eta");
  h->GetYaxis()->SetTitle("cell #phi"); 
  
  return;
}
/*---------------------------------------------------------*/
void LArHVCorrectionMonTool::SetTH1Style(TH1I_LW* h){
  
  h->GetYaxis()->SetTitle("Number of channels deviating from nominal HV");
  h->GetXaxis()->SetTitle("Luminosity Block"); 
  
  return;
}
/*---------------------------------------------------------*/


