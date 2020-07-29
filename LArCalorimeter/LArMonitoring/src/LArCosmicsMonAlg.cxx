/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArCosmicsMonAlg.cxx
// PACKAGE:  LArMonitoring
//
// AUTHOR:   Margherita Spalla (Migrated from original tool by Jessica Leveque)
//
// Class for monitoring : Detector Timing
//                        Cosmics Signal Reconstruction       
//                        Development towards LAr Data Quality Flags
//
// ********************************************************************

#include "LArCosmicsMonAlg.h"

#include "Identifier/IdentifierHash.h"

#include <algorithm>



/*---------------------------------------------------------*/
LArCosmicsMonAlg::LArCosmicsMonAlg(const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name, pSvcLocator),
    m_LArOnlineIDHelper(0),
    m_LArEM_IDHelper(0),
    m_LArHEC_IDHelper(0),
    m_LArFCAL_IDHelper(0),
    m_badChannelMask("BadLArRawChannelMask",this)
{
  declareProperty("LArBadChannelMask",m_badChannelMask); 
}

/*---------------------------------------------------------*/
LArCosmicsMonAlg::~LArCosmicsMonAlg()
{
}

/*---------------------------------------------------------*/
StatusCode 
LArCosmicsMonAlg::initialize()
{
  ATH_MSG_INFO( "Initialize LArCosmicsMonAlg" );

  
    /** Get LAr Online Id Helper*/
  if ( detStore()->retrieve( m_LArOnlineIDHelper, "LArOnlineID" ).isSuccess() ) { 
    ATH_MSG_DEBUG("connected non-tool: LArOnlineID" );
  } else {    
    ATH_MSG_FATAL( "unable to connect non-tool: LArOnlineID" );
    return StatusCode::FAILURE;    
  }
  
  
  // Retrieve ID helpers
  if ( detStore()->retrieve( m_LArEM_IDHelper, "LArEM_ID" ).isSuccess() ) ATH_MSG_DEBUG("connected non-tool: LArEM_ID" );
  else {  
    ATH_MSG_FATAL( "unable to connect non-tool: LArEM_ID" );
    return StatusCode::FAILURE;
  }

  if ( detStore()->retrieve( m_LArHEC_IDHelper, "LArHEC_ID" ).isSuccess() ) ATH_MSG_DEBUG("connected non-tool: LArHEC_ID" );
  else {  
    ATH_MSG_FATAL( "unable to connect non-tool: LArHEC_ID" );
    return StatusCode::FAILURE;
  }

  if ( detStore()->retrieve( m_LArFCAL_IDHelper, "LArFCAL_ID" ).isSuccess() ) ATH_MSG_DEBUG("connected non-tool: LArFCAL_ID" );
  else {  
    ATH_MSG_FATAL( "unable to connect non-tool: LArFCAL_ID" );
    return StatusCode::FAILURE;
  }
 
  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK(m_larPedestalKey.initialize());
  ATH_CHECK(m_LArDigitContainerKey.initialize());

  /** Get bad-channel mask */
  StatusCode sc=m_badChannelMask.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve BadChannelMask" << m_badChannelMask);
    return StatusCode::FAILURE;
  }

 return AthMonitorAlgorithm::initialize();  
}

/*---------------------------------------------------------*/
StatusCode 
LArCosmicsMonAlg::fillHistograms(const EventContext& ctx)  const {
  ATH_MSG_DEBUG( "in fillHists()" );
  

  //monitored variables
  auto mon_eta_EM = Monitored::Scalar<float>("mon_eta_EM",0);
  auto mon_eta_HEC = Monitored::Scalar<float>("mon_eta_HEC",0);
  auto mon_eta_FCal = Monitored::Scalar<float>("mon_eta_FCal",0);
  auto mon_phi = Monitored::Scalar<float>("mon_phi",0);



  /////////////////////////////////
  // Work with the LArDigits  //
  /////////////////////////////////

  //retrieve pedestal
  SG::ReadCondHandle<ILArPedestal> pedestalHdl{m_larPedestalKey,ctx};
  const ILArPedestal* pedestals=*pedestalHdl;

  //retrieve cabling
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey,ctx};
  const LArOnOffIdMapping* cabling=*cablingHdl;

  /** retrieve det. description manager */
  const CaloDetDescrManager* ddman = nullptr;
  ATH_CHECK( detStore()->retrieve (ddman, "CaloMgr") );

  //get digit container
  SG::ReadHandle<LArDigitContainer> pLArDigitContainer{m_LArDigitContainerKey,ctx};
  
  /** Define iterators to loop over Digits containers*/
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
    float eta=0;
    float phi=0;
    const CaloDetDescrElement* caloDetElement = ddman->get_element(offlineID);
    if(caloDetElement == 0 ){
      ATH_MSG_ERROR( "Cannot retrieve (eta,phi) coordinates" );
      continue; 
    }else{
      eta = caloDetElement->eta_raw();
      phi = caloDetElement->phi_raw();
    }

    // Fix phi range in HEC
    if (m_LArOnlineIDHelper->isHECchannel(id)) phi = CaloPhiRange::fix(phi);
    
    // Retrieve pedestals 
    CaloGain::CaloGain gain = pLArDigit->gain();
    float pedestal = pedestals->pedestal(id,gain);
    
    // Skip channel with no pedestal ref in db
    if(pedestal <= (1.0+LArElecCalib::ERRORCODE)) continue;      
    
    // Remove problematic channels
    if (m_badChannelMask->cellShouldBeMasked(id)) continue;
    
    //
    // HEC 
    //
    
    if(m_LArOnlineIDHelper->isHECchannel(id)){
      
      int sampling = m_LArHEC_IDHelper->sampling(offlineID);
      const std::vector < short >& samples = pLArDigit->samples();
      
      // Look for Muons Candidates in sampling 1
      if(sampling != 1) continue;
      
      // Get highest energy sample
      float sample_max = * std::max_element(samples.begin(), samples.end());
      sample_max = sample_max-pedestal;
      
      // If energy above threshold, we found a muon in the HEC
      if((sample_max) > m_muonADCthreshold_HEC){ 
	mon_eta_HEC=eta;
	mon_phi=phi;
	fill(m_CosmicsMonGroupName,mon_eta_HEC,mon_phi);
      }
      
    } // End HEC
    
    
    //
    // FCAL - Fill coverage / conditions / pedestal maps
    //
    
    if(m_LArOnlineIDHelper->isFCALchannel(id)){
      
      int sampling = m_LArFCAL_IDHelper->module(offlineID);;
      const std::vector < short > samples = pLArDigit->samples();
      
      // Look for Muons Candidates in sampling 2
      if(sampling != 2) continue;
      
      // Get highest energy sample 
      float sample_max = * std::max_element(samples.begin(), samples.end());
      sample_max = sample_max-pedestal;
      
      // If energy above threshold, we found a muon in the FCAL
      if( (sample_max) > m_muonADCthreshold_FCAL){
	mon_eta_FCal=eta;
	mon_phi=phi;
	fill(m_CosmicsMonGroupName,mon_eta_FCal,mon_phi);	
      }
      
    }//end FCAL 
    
    //
    // ECAL
    //
    if(m_LArOnlineIDHelper->isEMECchannel(id) || m_LArOnlineIDHelper->isEMBchannel(id) ){
      
      int sampling = m_LArEM_IDHelper->sampling(offlineID);
      const std::vector < short > samples = pLArDigit->samples();
      
      // Look for Muons Candidates in second sampling
      if(sampling != 2) continue;
      
      // Get highest energy sample
      float sample_max = * std::max_element(samples.begin(), samples.end());
      sample_max = sample_max-pedestal;
      
      // If energy above threshold, we found a muon in the barrel
      if( m_LArEM_IDHelper->is_em_barrel(offlineID) && (sample_max) > m_muonADCthreshold_EM_barrel ){
	mon_eta_EM=eta;
	mon_phi=phi;
	fill(m_CosmicsMonGroupName,mon_eta_EM,mon_phi);
      }
      
      // If energy above threshold, we found a muon in the endcap
      if(m_LArEM_IDHelper->is_em_endcap(offlineID) && (sample_max) > m_muonADCthreshold_EM_endcap ){
	mon_eta_EM=eta;
	mon_phi=phi;
	fill(m_CosmicsMonGroupName,mon_eta_EM,mon_phi);
      }
      
    } // end EM
    
  }// end of LArDigits loop  
  
  return StatusCode::SUCCESS;
}




