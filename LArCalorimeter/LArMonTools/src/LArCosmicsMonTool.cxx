/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArCosmicsMonTool.cxx
// PACKAGE:  LArMonTools
//
// AUTHOR:   Jessica Leveque
//
// Class for monitoring : Detector Timing
//                        Cosmics Signal Reconstruction       
//                        Development towards LAr Data Quality Flags
//
// ********************************************************************

#include "LArCosmicsMonTool.h"

#include "StoreGate/ReadCondHandle.h"
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

using namespace std;

// To create a map containing seeds and their energy
struct order {    
  bool operator()( float s1, float s2 ) const {
    return s1 > s2;
  }  
};
typedef map<float,pair<Identifier,bool>, order> MAP;

/*---------------------------------------------------------*/
LArCosmicsMonTool::LArCosmicsMonTool(const std::string& type, 
				     const std::string& name,
				     const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent), 
    m_rootStore(nullptr),
    m_newrun(true)
{
  declareProperty("LArDigitContainerKey", m_LArDigitContainerKey = "FREE");
  declareProperty("muonADCthreshold_EM_barrel", m_muonADCthreshold_EM_barrel = 30);
  declareProperty("muonADCthreshold_EM_endcap", m_muonADCthreshold_EM_endcap = 40);
  declareProperty("muonADCthreshold_HEC", m_muonADCthreshold_HEC = 40);
  declareProperty("muonADCthreshold_FCAL", m_muonADCthreshold_FCAL = 40);
  
  m_eventsCounter = 0;
  
  m_LArOnlineIDHelper	= nullptr;
  m_LArEM_IDHelper	= nullptr;
  m_LArFCAL_IDHelper	= nullptr;
  m_LArHEC_IDHelper	= nullptr;
  m_caloIdMgr		= nullptr;

  m_hMuonMapEMDig	= nullptr;
  m_hMuonMapHECDig	= nullptr;
  m_hMuonMapFCALDig	= nullptr;

  for( unsigned i = 0; i < 4; ++i )
  {
    m_hMuonTimeEMDig[i]		= nullptr;
    m_hMuonEnergyEMDig[i]	= nullptr;
    m_hMuonEvsTimeEMDig[i]	= nullptr;
    m_hMuonShapeEMDig[i]	= nullptr;
  }
  for( unsigned i = 0; i < 2; ++i )
  {
    m_hMuonTimeHECDig[i]	= nullptr;
    m_hMuonTimeFCALDig[i]	= nullptr;
    m_hMuonEnergyHECDig[i]	= nullptr;
    m_hMuonEnergyFCALDig[i]	= nullptr;
    m_hMuonEvsTimeHECDig[i]	= nullptr;
    m_hMuonEvsTimeFCALDig[i]	= nullptr;
    m_hMuonShapeHECDig[i]	= nullptr;
    m_hMuonShapeFCALDig[i]	= nullptr;
  }
}

/*---------------------------------------------------------*/
LArCosmicsMonTool::~LArCosmicsMonTool()
{
}

/*---------------------------------------------------------*/
StatusCode 
LArCosmicsMonTool::initialize()
{
  ATH_MSG_INFO( "Initialize LArCosmicsMonTool" );
 
  // Retrieve ID helpers
  ATH_CHECK(  detStore()->retrieve( m_caloIdMgr ) );
  m_LArEM_IDHelper   = m_caloIdMgr->getEM_ID();
  m_LArHEC_IDHelper  = m_caloIdMgr->getHEC_ID();
  m_LArFCAL_IDHelper = m_caloIdMgr->getFCAL_ID();
  
  ATH_CHECK( detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID") );
  
  ATH_CHECK(m_bcContKey.initialize());
  ATH_CHECK(m_bcMask.buildBitMask(m_problemsToMask,msg()));

  ATH_CHECK( this->initMonInfo() );
  ATH_CHECK( m_larPedestalKey.initialize() );
  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_caloMgrKey.initialize() );
  // End Initialize
  ManagedMonitorToolBase::initialize().ignore();
  ATH_MSG_DEBUG( "Successful Initialize LArCosmicsMonTool " );
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode 
LArCosmicsMonTool::bookHistograms() {

  ATH_MSG_DEBUG( "in bookHists()" );
  
  //  if(isNewRun ){// Commented by B.Trocme to comply with new ManagedMonitorToolBase
    m_newrun=true;
    //
    // Create top folder for histos
    //
    
    MonGroup generalGroupShift( this, "/LAr/CosmicsOldTool/", run, ATTRIB_MANAGED );
    
    // Store cells granularity and plots boundaries for each sampling in ECAL
    float etaminECAL = -3.2 ; float etamaxECAL = 3.2;
    float phiminECAL = -TMath::Pi(); float phimaxECAL = TMath::Pi();
    float detaECAL[] = {  0.025,  0.025/8, 0.025, 0.050};
    float dphiECAL[] = { (float)(2*TMath::Pi()/64),
                         (float)(2*TMath::Pi()/64),
                         (float)(2*TMath::Pi()/256),
                         (float)(2*TMath::Pi()/256)};
    
    // Store cells granularity and plots boundaries for each sampling in HCAL
    float etaminHCAL = -3.2 ; float etamaxHCAL = 3.2;
    float phiminHCAL = -TMath::Pi(); float phimaxHCAL = TMath::Pi();
    float detaHCAL[] = {  0.1,  0.1, 0.1, 0.1};
    float dphiHCAL[] = { (float)(2*TMath::Pi()/64),
                         (float)(2*TMath::Pi()/64),
                         (float)(2*TMath::Pi()/64),
                         (float)(2*TMath::Pi()/64)};
    
    // Store cells granularity and plots boundaries in FCAL
    float etaminFCAL = -4.9 ; float etamaxFCAL = 4.9;
    float phiminFCAL = -TMath::Pi(); float phimaxFCAL = TMath::Pi();
    float detaFCAL[] = {1111, 0.1, 0.2, 0.2}; 
    float dphiFCAL[] = {1111,
                        (float)(2*TMath::Pi()/64),
                        (float)(2*TMath::Pi()/32),
                        (float)(2*TMath::Pi()/32)};
    
    //
    // Muon Seeds - Digits
    //
    
    // ECAL 
    
    m_hMuonMapEMDig = new TH2F("Muon2DHitsECAL", 
			       Form("Cosmics Seeds - Digit Max > %i/%i [ADC] in S2 Barrel/Endcap - EM",
				    int(m_muonADCthreshold_EM_barrel),int(m_muonADCthreshold_EM_endcap)),
			       int(fabs(etamaxECAL-etaminECAL)/detaECAL[2]),etaminECAL,etamaxECAL,
			       int(fabs(phimaxECAL-phiminECAL)/dphiECAL[2]),phiminECAL,phimaxECAL);
    m_hMuonMapEMDig->GetXaxis()->SetTitle("#eta cell");
    m_hMuonMapEMDig->GetYaxis()->SetTitle("#phi cell");
    m_hMuonMapEMDig->GetZaxis()->SetTitle("Number of Hits");
    generalGroupShift.regHist(m_hMuonMapEMDig).ignore();  
    
    
    // HCAL 
    
    m_hMuonMapHECDig = new TH2F("Muon2DHitsHCAL", 
				Form("Cosmics Seeds - Digit Max > %i [ADC] in S1 - HEC", int(m_muonADCthreshold_HEC)),
				int(fabs(etamaxHCAL-etaminHCAL)/detaHCAL[1]),etaminHCAL,etamaxHCAL,
				int(fabs(phimaxHCAL-phiminHCAL)/dphiHCAL[1]),phiminHCAL,phimaxHCAL);
    m_hMuonMapHECDig->GetXaxis()->SetTitle("#eta cell");
    m_hMuonMapHECDig->GetYaxis()->SetTitle("#phi cell");
    m_hMuonMapHECDig->GetZaxis()->SetTitle("Number of Hits");
    generalGroupShift.regHist(m_hMuonMapHECDig).ignore();  
    
    // FCAL
    
    m_hMuonMapFCALDig = new TH2F("Muon2DHitsFCAL",
				 Form("Cosmics Seeds - Digit Max > %i [ADC] in S2 - FCAL",int(m_muonADCthreshold_FCAL)),
				 int(fabs(etamaxFCAL-etaminFCAL)/detaFCAL[1]),etaminFCAL,etamaxFCAL,
				 int(fabs(phimaxFCAL-phiminFCAL)/dphiFCAL[1]),phiminFCAL,phimaxFCAL);
    m_hMuonMapFCALDig->GetXaxis()->SetTitle("#eta cell");
    m_hMuonMapFCALDig->GetYaxis()->SetTitle("#phi cell");
    m_hMuonMapFCALDig->GetZaxis()->SetTitle("Number of Hits");
    generalGroupShift.regHist(m_hMuonMapFCALDig).ignore();  
    
    return StatusCode::SUCCESS;
    
    //  }// end isNewRun
  
//  // this is just to avoid warnings at compilation
//  else if(isNewLumiBlock || isNewEventsBlock){
//    return StatusCode::SUCCESS;
//  }
//  
//  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode 
LArCosmicsMonTool::fillHistograms() {
  const EventContext& ctx = Gaudi::Hive::currentContext();
  ATH_MSG_DEBUG( "in fillHists()" );
  StatusCode sc;
  
  // Increment event counter
  m_eventsCounter++;
  
  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey,ctx};
  ATH_CHECK(caloMgrHandle.isValid());
  const CaloDetDescrManager* ddman = *caloMgrHandle;
  
  /////////////////////////////////
  // Work with the LArDigits  //
  /////////////////////////////////
  
  // retrieve LArDigits
  const LArDigitContainer* pLArDigitContainer = nullptr;
  sc = evtStore()->retrieve(pLArDigitContainer, m_LArDigitContainerKey);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Can\'t retrieve LArDigitContainer with key " 
		      << m_LArDigitContainerKey );
    return StatusCode::SUCCESS;
  }
  
  // Retrieve pedestals container
  SG::ReadCondHandle<ILArPedestal> pedestals (m_larPedestalKey, ctx);

  //retrieve BadChannel info:
  SG::ReadCondHandle<LArBadChannelCont> bcContHdl{m_bcContKey,ctx};
  const LArBadChannelCont* bcCont{*bcContHdl};

  SG::ReadCondHandle<LArOnOffIdMapping> cabling (m_cablingKey, ctx);

  // loop over LArDigits
  LArDigitContainer::const_iterator itDig = pLArDigitContainer->begin(); 
  LArDigitContainer::const_iterator itDig_e= pLArDigitContainer->end(); 
  const LArDigit* pLArDigit;
  for ( ; itDig!=itDig_e;++itDig) {
    pLArDigit = *itDig;
    HWIdentifier id = pLArDigit->hardwareID();
    Identifier offlineID = cabling->cnvToIdentifier(id);
    
    // Skip disconnected channels
    if(!cabling->isOnlineConnected(id)) continue;
    
    // Get Physical Coordinates     
    float eta = 0; float phi = 0;
    sc = returnEtaPhiCoord(ddman, offlineID, eta, phi);
    if(sc.isFailure()) {
      ATH_MSG_ERROR( "Cannot retrieve (eta,phi) coordinates" );
      continue;
    } 
    
    // Fix phi range in HEC
    if (m_LArOnlineIDHelper->isHECchannel(id)) phi = CaloPhiRange::fix(phi);
    
    // Retrieve pedestals 
    CaloGain::CaloGain gain = pLArDigit->gain();
    float pedestal = pedestals->pedestal(id,gain);
    
    // Skip channel with no pedestal ref in db
    if(pedestal <= (1.0+LArElecCalib::ERRORCODE)) continue;      
    
    // Remove problematic channels
    if (m_bcMask.cellShouldBeMasked(bcCont,id)) continue;
    //
    // HEC 
    //
    
    if(m_LArOnlineIDHelper->isHECchannel(id)){
      
      int sampling = m_LArHEC_IDHelper->sampling(offlineID);
      const std::vector < short >& samples = pLArDigit->samples();
      
      // Look for Muons Candidates in sampling 1
      if(sampling != 1) continue;
      
      // Get highest energy sample
      float sample_max = 0.; //float time_max = 0.;
      int nsamples = samples.size();
      for (int im = 0 ; im < nsamples; im++)  {
	if (samples[im] > sample_max){
	  sample_max = samples[im];
	  //time_max = im;
	}
      }
      sample_max = sample_max-pedestal;
      
      // If energy above threshold, we found a muon in the HEC
      if( (sample_max) > m_muonADCthreshold_HEC){
	
	m_hMuonMapHECDig->Fill(eta,phi, 1.);
	
      }
      
    } // End HEC
    
    
    //
    // FCAL - Fill coverage / conditions / pedestal maps
    //
    
    if(m_LArOnlineIDHelper->isFCALchannel(id)){
      
      int sampling = m_LArFCAL_IDHelper->module(offlineID);
      const std::vector < short >& samples = pLArDigit->samples();
      
      // Look for Muons Candidates in sampling 2
      if(sampling != 2) continue;
      
      // Get highest energy sample 
      float sample_max = 0.; //float time_max = 0.;
      int nsamples = samples.size();
      for (int im = 0 ; im < nsamples; im++)  {
	if (samples[im] > sample_max){
	  sample_max = samples[im];
	  //time_max = im;
	}
      }
      sample_max = sample_max-pedestal;
      
      // If energy above threshold, we found a muon in the FCAL
      if( (sample_max) > m_muonADCthreshold_FCAL){
	
	m_hMuonMapFCALDig->Fill(eta,phi, 1.);
	
      }
      
    }//end FCAL 
    
    //
    // ECAL
    //
    if(m_LArOnlineIDHelper->isEMECchannel(id) || m_LArOnlineIDHelper->isEMBchannel(id) ){
      
      int sampling = m_LArEM_IDHelper->sampling(offlineID);
      const std::vector < short >& samples = pLArDigit->samples();
      
      // Look for Muons Candidates in second sampling
      if(sampling != 2) continue;
      
      // Get highest energy sample
      float sample_max = 0.; //float time_max = 0.;
      int nsamples = samples.size();
      for (int im = 0 ; im < nsamples; im++)  {
	if (samples[im] > sample_max){
	  sample_max = samples[im];
	  //time_max = im;
	}
      }
      sample_max = sample_max-pedestal;
      
      // If energy above threshold, we found a muon in the barrel
      if( m_LArEM_IDHelper->is_em_barrel(offlineID) 
	  && (sample_max) > m_muonADCthreshold_EM_barrel ){
	
	m_hMuonMapEMDig->Fill(eta,phi, 1.);
	
      }
      
      // If energy above threshold, we found a muon in the endcap
      if(m_LArEM_IDHelper->is_em_endcap(offlineID) 
	 && (sample_max) > m_muonADCthreshold_EM_endcap ){
	
	m_hMuonMapEMDig->Fill(eta,phi, 1.);
	
      }
      
    } // end EM
    
  }// end of LArDigits loop  
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode LArCosmicsMonTool::procHistograms()
{
    
  ATH_MSG_DEBUG( "End of procHistograms " );
  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/
StatusCode LArCosmicsMonTool::initMonInfo()  
{
  ATH_MSG_DEBUG( "in initMonInfo()" );
  
  std::vector<CaloGain::CaloGain> gains;
  gains.push_back(CaloGain::LARHIGHGAIN);
  gains.push_back(CaloGain::LARMEDIUMGAIN);
  gains.push_back(CaloGain::LARLOWGAIN); 
  
  ATH_MSG_DEBUG( "Init Monitoring ended successfully " );
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode LArCosmicsMonTool::returnEtaPhiCoord(const CaloDetDescrManager* ddman,
                                                Identifier offlineID,float& eta,float& phi)
{
  // Get Calo detector description element to retrieve true eta/phi
  const CaloDetDescrElement* caloDetElement = ddman->get_element(offlineID);
  
  if(caloDetElement == nullptr ){
    return StatusCode::FAILURE;
  }else{
    eta = caloDetElement->eta_raw();
    phi = caloDetElement->phi_raw();
    return StatusCode::SUCCESS;
  }
}
/*------------------------------------------------------*/
std::string LArCosmicsMonTool::gain_str(CaloGain::CaloGain gain)
{
  // return a string for the gain
  std::string gStr = "";
  if (gain == CaloGain::LARHIGHGAIN) {
    gStr = "High";
  } else if (gain == CaloGain::LARMEDIUMGAIN) {
    gStr = "Medium";
  } else if (gain == CaloGain::LARLOWGAIN) {
    gStr = "Low";
  } else if (gain == CaloGain::UNKNOWNGAIN) {
    gStr = "Unknown";  
  }
  return gStr;
}

/*----------------------------------------------------------*/
std::string LArCosmicsMonTool::sampling_str(int sampling)
{
  // return a string for the sampling
  std::string sStr = "";
  switch(sampling){
    case 0:
      sStr = "PS";
      break;
    case 1:
      sStr = "Front";
      break;
    case 2:
      sStr = "Middle";
      break;
    case 3:
      sStr = "Back";
      break;
  }
  
  return sStr;
}
