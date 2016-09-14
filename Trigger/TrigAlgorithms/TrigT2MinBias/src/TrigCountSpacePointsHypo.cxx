/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2MinBias/TrigCountSpacePointsHypo.h"
#include "TrigInDetEvent/TrigSpacePointCounts.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "xAODEventInfo/EventInfo.h"
#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"

#include "xAODTrigMinBias/TrigSpacePointCounts.h"


TrigCountSpacePointsHypo::TrigCountSpacePointsHypo(const std::string& name, 
						   ISvcLocator* pSvcLocator): HLT::HypoAlgo(name, pSvcLocator),
                        m_hltExecuteInitialisationRun(false),
									      m_log(msgSvc(), name) {
  declareProperty("TrigSpacePointCountsKey", m_trigSpacePointCountsKey = "spacepoints");
  declareProperty("AcceptAll",               m_acceptAll = false ); 

  // Cut made on time overthreshold before any multiplicities are calculated.
  declareProperty("PixelCLToTCut",           m_timeOverThreshold_cut = 20.);

  // If set to false the trigger combination of Pixel and SCT is an OR.
  declareProperty("TriggerTypeAND",          m_triggerTypeAND = true); 
  // veto large multiplicity events
  declareProperty("VetoLargeSP",          m_vetoLargeSP = false); 

  // Invert the final decision of the hypo - turning it into a SP veto
  declareProperty("Veto",                    m_veto = false); 

  // Threshold settings for multiplicity requirements.
  // A trigger threshold can be disabled by setting it to be < 0
  declareProperty("TotalPixelClus",          m_totalPixelClus_cut = 10.);
  declareProperty("TotalPixelClusMin",       m_totalPixelClus_cut_min = -1.);
  declareProperty("PixelClusRatioA",         m_pixelClusRatioA_cut = -1.);
  declareProperty("PixelClusRatioB",         m_pixelClusRatioB_cut = -1.);
  declareProperty("PixelClusEndcapC",        m_pixelClusEndcapC_cut = -1.);
  declareProperty("PixelClusBarrel",         m_pixelClusBarrel_cut = -1.);
  declareProperty("PixelClusEndcapA",        m_pixelClusEndcapA_cut = -1.);
  declareProperty("PixelClusEndcapC_max",    m_pixelClusEndcapC_max = -1.);
  declareProperty("PixelClusBarrel_max",     m_pixelClusBarrel_max = -1.);
  declareProperty("PixelClusEndcapA_max",    m_pixelClusEndcapA_max = -1.);
  declareProperty("PixelClusEndcaps_cut",    m_pixelClusEndcaps_cut = -1.);

  declareProperty("TotalSctSp",              m_totalSctSp_cut = 30.);
  declareProperty("TotalSctSpMin",           m_totalSctSp_cut_min = -1.);            
  declareProperty("SctSpEndcapC",            m_sctSpEndcapC_cut = -1.);
  declareProperty("SctSpBarrel",             m_sctSpBarrel_cut = -1.);
  declareProperty("SctSpEndcapA",            m_sctSpEndcapA_cut = -1.);
  declareProperty("SctSpEndcapC_max",        m_sctSpEndcapC_max = -1.);
  declareProperty("SctSpBarrel_max",         m_sctSpBarrel_max = -1.);
  declareProperty("SctSpEndcapA_max",        m_sctSpEndcapA_max = -1.);
  declareProperty("SctSpEndcaps_cut",        m_sctSpEndcaps_cut = -1.);

  // Monitoring
  declareMonitoredVariable("PIX_SPCountHypo",m_totNumPixSP );
  declareMonitoredVariable("SCT_SPCountHypo",m_totNumSctSP );
  declareMonitoredVariable("PIX_SPCountSel", m_totSelNumPixSP );
  declareMonitoredVariable("SCT_SPCountSel", m_totSelNumSctSP );

  // Detector mask flags
  declareProperty("sct_barrel_a_side",       m_sct_barrel_a_side = true);
  declareProperty("sct_barrel_c_side",       m_sct_barrel_c_side = true);
  declareProperty("sct_endcap_a_side",       m_sct_endcap_a_side = true);
  declareProperty("sct_endcap_c_side",       m_sct_endcap_c_side = true);
  declareProperty("pixel_barrel",            m_pixel_barrel = true);
  declareProperty("pixel_b_layer",           m_pixel_b_layer = true);
  declareProperty("pixel_disk",              m_pixel_disk = true);
  declareProperty("maxNumPixClusEC",         m_maxNumPixClusEC = -1.);
  declareProperty("maxNumSctSpEC",           m_maxNumSctSpEC = -1.);


  m_formFeaturesTimer = 0;
}

//-----------------------------------------------------------------------------

HLT::ErrorCode TrigCountSpacePointsHypo::hltInitialize() {
  m_log.setLevel(outputLevel());

  if(msgLvl() <= MSG::INFO) {
    m_log << MSG::INFO << "Initialising this TrigCountSpacePointsHypo: " << name() << endreq;
    m_log << MSG::INFO << "================ Hypo Settings ================" << endreq;    
    m_log << MSG::INFO << " AcceptAll -------------------  "
          << (m_acceptAll==true ? "True" : "False") << endreq;

    m_log << MSG::INFO << " TriggerTypeAND --------------  "
          << (m_triggerTypeAND==true ? "True" : "False") << endreq;

    m_log << MSG::INFO << " TotalPixelClus  -------------  ";
    if(m_totalPixelClus_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_totalPixelClus_cut << endreq;

    m_log << MSG::INFO << " PixelClusRatioA  ------------  ";
    if(m_pixelClusRatioA_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_pixelClusRatioA_cut << endreq;

    m_log << MSG::INFO << " PixelClusRatioB  ------------  ";
    if(m_pixelClusRatioB_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_pixelClusRatioB_cut << endreq;

    m_log << MSG::INFO << " PixelClusEndcapC  -----------  ";
    if(m_pixelClusEndcapC_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_pixelClusEndcapC_cut << endreq;

    m_log << MSG::INFO << " PixelClusBarrel  ------------  ";
    if(m_pixelClusBarrel_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_pixelClusBarrel_cut << endreq;

    m_log << MSG::INFO << " PixelClusEndcapA  -----------  ";
    if(m_pixelClusEndcapA_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_pixelClusEndcapA_cut << endreq;

    m_log << MSG::INFO << " PixelClusEndcapC_max  -------  ";
    if(m_pixelClusEndcapC_max < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_pixelClusEndcapC_max << endreq;

    m_log << MSG::INFO << " PixelClusBarrel_max  --------  ";
    if(m_pixelClusBarrel_max < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_pixelClusBarrel_max << endreq;

    m_log << MSG::INFO << " PixelClusEndcapA_max  -------  ";
    if(m_pixelClusEndcapA_max < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_pixelClusEndcapA_max << endreq;

    m_log << MSG::INFO << " PixelClusEndcaps_cut  -------  ";
    if(m_pixelClusEndcaps_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_pixelClusEndcaps_cut << endreq;

    m_log << MSG::INFO << " TotalSctSp  -----------------  ";
    if(m_totalSctSp_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_totalSctSp_cut << endreq;

    m_log << MSG::INFO << " SctSpEndcapC  ---------------  ";
    if(m_sctSpEndcapC_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_sctSpEndcapC_cut << endreq;

    m_log << MSG::INFO << " SctSpBarrel  ----------------  ";
    if(m_sctSpBarrel_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_sctSpBarrel_cut << endreq;

    m_log << MSG::INFO << " SctSpEndcapA  ---------------  ";
    if(m_sctSpEndcapA_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_sctSpEndcapA_cut << endreq;

    m_log << MSG::INFO << " SctSpEndcapC_max  -----------  ";
    if(m_sctSpEndcapC_max < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_sctSpEndcapC_max << endreq;

    m_log << MSG::INFO << " SctSpBarrel_max  ------------  ";
    if(m_sctSpBarrel_max < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_sctSpBarrel_max << endreq;

    m_log << MSG::INFO << " SctSpEndcapA_max  -----------  ";
    if(m_sctSpEndcapA_max < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_sctSpEndcapA_max << endreq;

    m_log << MSG::INFO << " SctSpEndcaps_cut  -----------  ";
    if(m_sctSpEndcaps_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_sctSpEndcaps_cut << endreq;

    m_log << MSG::INFO << "===============================================" << endreq;
  }

  // Create timers
  if ( timerSvc() ) {
    m_formFeaturesTimer = addTimer("FormFeatures");
  }

  return HLT::OK;
}

//-----------------------------------------------------------------------------
HLT::ErrorCode TrigCountSpacePointsHypo::hltBeginRun() {
  // This initialisation has been moved into the event loop.
  // @see TrigCountSpacePointsHypo::checkDetectorMask
  if (msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << " TrigCountSpacePointsHypo will be initialized in hltExecute" << endreq; 
  return HLT::OK;
}
//---------------------------------------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigCountSpacePointsHypo::checkDetectorMask() {
  m_hltExecuteInitialisationRun = true;
  m_log << MSG::DEBUG << "[TrigCountSpacePointsHypo::checkDetectorMask]  beginning run with this " << name() << endreq;

  const xAOD::EventInfo* evinfo = 0;
  if (store()->retrieve(evinfo).isFailure()) {
    if (msgLvl() <= MSG::ERROR) m_log << MSG::ERROR << "Cannot retrieve xAOD::EventInfo from SG for detmasks" << endreq;
    return HLT::SG_ERROR;
  }
  else {
    if (msgLvl() <= MSG::INFO) m_log << MSG::INFO << "xAOD::EventInfo Run Information [Run,Evt,Lumi,Time,BunchCross,DetMask] = [" 
      << evinfo->runNumber()
      << "," << evinfo->eventNumber()
      << "," << evinfo->lumiBlock()
      << "," << evinfo->timeStamp()
      << ":" << evinfo->timeStampNSOffset()
      << "," << evinfo->bcid()
      << ",0x" << std::hex << evinfo->detectorMask() << std::dec
      << "]" << endreq;

    uint64_t mask = evinfo->detectorMask();
    eformat::helper::DetectorMask decoder(mask);

    if (mask == 0) {
      if (msgLvl() <= MSG::INFO) m_log << MSG::INFO << "Detector Mask == 0. Assuming MC file and setting all of ID to ON." << endreq; 
      m_sct_barrel_a_side = true;
      m_sct_barrel_c_side = true;
      m_sct_endcap_a_side = true;
      m_sct_endcap_c_side = true;
      m_pixel_barrel  = true;
      m_pixel_b_layer = true;
      m_pixel_disk    = true;
    } else {
      m_sct_barrel_a_side = decoder.is_set(eformat::SCT_BARREL_A_SIDE);
      m_sct_barrel_c_side = decoder.is_set(eformat::SCT_BARREL_C_SIDE);
      m_sct_endcap_a_side = decoder.is_set(eformat::SCT_ENDCAP_A_SIDE);
      m_sct_endcap_c_side = decoder.is_set(eformat::SCT_ENDCAP_C_SIDE);
      m_pixel_barrel  = decoder.is_set(eformat::PIXEL_BARREL);
      m_pixel_b_layer = decoder.is_set(eformat::PIXEL_B_LAYER);
      m_pixel_disk    = decoder.is_set(eformat::PIXEL_DISK);
    }

    if( msgLvl() <= MSG::INFO ){
      m_log << MSG::INFO << "sct_barrel_a_side is " << (m_sct_barrel_a_side==true? "present" : "OFF! Warning: Check if this info is used in trigger decision!") << endreq;
      m_log << MSG::INFO << "sct_barrel_c_side is " << (m_sct_barrel_c_side==true? "present" : "OFF! Warning: Check if this info is used in trigger decision!") << endreq;
      m_log << MSG::INFO << "sct_endcap_a_side is " << (m_sct_endcap_a_side==true? "present" : "OFF! Warning: Check if this info is used in trigger decision!") << endreq;
      m_log << MSG::INFO << "sct_endcap_c_side is " << (m_sct_endcap_c_side==true? "present" : "OFF! Warning: Check if this info is used in trigger decision!") << endreq;
      
      m_log << MSG::INFO << "pixel_barrel is      " << (m_pixel_barrel==true? "present" : "OFF! Warning: Check if this info is used in trigger decision!") << endreq;
      m_log << MSG::INFO << "pixel_b_layer is     " << (m_pixel_b_layer==true? "present" : "OFF! Warning: Check if this info is used in trigger decision!") << endreq;
      m_log << MSG::INFO << "pixel_disk is        " << (m_pixel_disk==true? "present" : "OFF! Warning: Check if this info is used in trigger decision!") << endreq;
    }
  }
  return HLT::OK;
}

HLT::ErrorCode TrigCountSpacePointsHypo::hltExecute(const HLT::TriggerElement* outputTE, 
						    bool& pass) {
  float ratioA = -1. , ratioB = -1. ;
  bool pixelClusPass, sctSpPass;

  // Do initialisation at start of first event
  if (m_hltExecuteInitialisationRun == false) {
    HLT::ErrorCode ec = checkDetectorMask();
    if (ec != HLT::OK) return ec;
  }

  pass = false;
  pixelClusPass = true;  
  sctSpPass = true;

  // for monitoring
  m_totNumPixSP = -999;
  m_totNumSctSP = -999;

  if(msgLvl() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "Executing this TrigCountSpacePointsHypo " << name() << endreq;
  }

  if( timerSvc() ) m_formFeaturesTimer->start();

  // Retrieve a pointer to TrigSpacePointCounts created by the FEX.
  const xAOD::TrigSpacePointCounts* trigSpacePointCounts = 0;
  HLT::ErrorCode hltStatus = getFeature(outputTE, trigSpacePointCounts, m_trigSpacePointCountsKey);
  if(hltStatus != HLT::OK || !trigSpacePointCounts) {
    if( msgLvl() <= MSG::WARNING ){
      m_log << MSG::WARNING << "Failed to retrieve features from TE." << endreq;
    }
    return HLT::OK;
  }

  for(int i=0;i<9;i++) m_multiplicity[i] = 0.;
   
  // Retrieve the Pixel cluster multiplicities after a time over, if needed
  bool usePIX = ( m_totalPixelClus_cut   > 0. || 
		  m_pixelClusRatioA_cut  > 0. ||
		  m_pixelClusRatioB_cut  > 0. ||
		  m_pixelClusEndcapC_cut > 0. ||
		  m_pixelClusBarrel_cut  > 0. ||
		  m_pixelClusEndcapA_cut > 0. ||
		  m_pixelClusEndcapC_max > 0. ||
		  m_pixelClusBarrel_max  > 0. ||
		  m_pixelClusEndcapA_max > 0. || 
		  m_pixelClusEndcaps_cut > 0. );

  if( !(m_pixel_barrel &&  m_pixel_b_layer &&  m_pixel_disk) ) {
    if (msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "NO pixel detector present, proceeding to SCT." << endreq;
  }
  
  
  if( usePIX ){
    
    // threshold cut and with no cut on the pixel cluster size.
    m_multiplicity[PIXEL_CLUS_EC] = trigSpacePointCounts->pixelClusEndcapCSumEntries(m_timeOverThreshold_cut, 0., xAOD::TrigHistoCutType::ABOVE_X_ABOVE_Y);
    m_multiplicity[PIXEL_CLUS_EA] = trigSpacePointCounts->pixelClusEndcapASumEntries(m_timeOverThreshold_cut, 0., xAOD::TrigHistoCutType::ABOVE_X_ABOVE_Y);
    m_multiplicity[PIXEL_CLUS_BL] = trigSpacePointCounts->pixelClusBarrelSumEntries(m_timeOverThreshold_cut, 0., xAOD::TrigHistoCutType::ABOVE_X_ABOVE_Y);
    m_totNumPixSP = m_multiplicity[PIXEL_CLUS_EC] + m_multiplicity[PIXEL_CLUS_EA] + m_multiplicity[PIXEL_CLUS_BL];

    if( m_multiplicity[PIXEL_CLUS_EA] > m_multiplicity[PIXEL_CLUS_EC] )
      m_maxNumPixClusEC = m_multiplicity[PIXEL_CLUS_EA];
    else
      m_maxNumPixClusEC = m_multiplicity[PIXEL_CLUS_EC];
  
    // To save time only calculate the pixel cluster ratios if they are needed.
    if( m_pixelClusRatioA_cut > 0. || m_pixelClusRatioB_cut > 0. ){
    
      // Retrieve the Pixel cluster multiplicities after a time over
      // threshold cut for 3+ pixel cluster size.
      m_multiplicity[PIXEL_CLUS_SZ_3] = trigSpacePointCounts->pixelClusEndcapCSumEntries(m_timeOverThreshold_cut, 3., xAOD::TrigHistoCutType::ABOVE_X_ABOVE_Y);
      m_multiplicity[PIXEL_CLUS_SZ_3] += trigSpacePointCounts->pixelClusBarrelSumEntries(m_timeOverThreshold_cut, 3., xAOD::TrigHistoCutType::ABOVE_X_ABOVE_Y);
      m_multiplicity[PIXEL_CLUS_SZ_3] += trigSpacePointCounts->pixelClusEndcapASumEntries(m_timeOverThreshold_cut, 3., xAOD::TrigHistoCutType::ABOVE_X_ABOVE_Y);
      
      // Retrieve the Pixel cluster multiplicities after a time over threshold cut for 2+ pixel cluster size.
      m_multiplicity[PIXEL_CLUS_SZ_2] = trigSpacePointCounts->pixelClusEndcapCSumEntries(m_timeOverThreshold_cut, 2., xAOD::TrigHistoCutType::ABOVE_X_ABOVE_Y);
      m_multiplicity[PIXEL_CLUS_SZ_2] += trigSpacePointCounts->pixelClusBarrelSumEntries(m_timeOverThreshold_cut, 2., xAOD::TrigHistoCutType::ABOVE_X_ABOVE_Y);
      m_multiplicity[PIXEL_CLUS_SZ_2] += trigSpacePointCounts->pixelClusEndcapASumEntries(m_timeOverThreshold_cut, 2., xAOD::TrigHistoCutType::ABOVE_X_ABOVE_Y);
      m_multiplicity[PIXEL_CLUS_SZ_2] -= m_multiplicity[PIXEL_CLUS_SZ_3]; // Remove the 3+ clusters to leave size 2 only.
      
      // Calculate the number of size 1 clusters.
      m_multiplicity[PIXEL_CLUS_SZ_1] = m_totNumPixSP - m_multiplicity[PIXEL_CLUS_SZ_2] - m_multiplicity[PIXEL_CLUS_SZ_3];
    
      // Calculate the pixel cluser ratio values for this event.  
      if(m_totNumPixSP > 0.) {
        ratioA = (m_multiplicity[PIXEL_CLUS_SZ_2] + m_multiplicity[PIXEL_CLUS_SZ_3])/m_totNumPixSP;
      }
      else {
	if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "ratioA not defined, m_totNumPixSP =" << m_totNumPixSP << endreq;
	ratioA = -1.;
      }

      if(m_multiplicity[PIXEL_CLUS_SZ_1] > 0.) {
	ratioB = m_multiplicity[PIXEL_CLUS_SZ_2]/m_multiplicity[PIXEL_CLUS_SZ_1];
      }
      else {
	if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "ratioB not defined, m_multiplicity[PIXEL_CLUS_SZ_1] =" << m_multiplicity[PIXEL_CLUS_SZ_1] << endreq;
	ratioB = -1.;
      }
    }
  }
  else{
    if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Won't use pixel." << endreq;
    // do not decide on pixel features 
    pixelClusPass = false;
  }
  
  
  // Retrieve SCT space point multiplicities if needed
  bool useSCT = (m_totalSctSp_cut   > 0. ||
                 m_sctSpEndcapC_cut > 0. ||
                 m_sctSpBarrel_cut  > 0. ||
                 m_sctSpEndcapA_cut > 0. ||
                 m_sctSpEndcapC_max > 0. ||
                 m_sctSpBarrel_max  > 0. ||
                 m_sctSpEndcapA_max > 0. || 
                 m_sctSpEndcaps_cut > 0.);

  if( !(m_sct_barrel_a_side && m_sct_barrel_c_side &&  m_sct_endcap_a_side &&  m_sct_endcap_c_side) ){
    if (msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "NO sct detector present." << endreq ;
  }

  if( useSCT ){

    m_multiplicity[SCT_SP_EC] = trigSpacePointCounts->sctSpEndcapC();
    m_multiplicity[SCT_SP_EA] = trigSpacePointCounts->sctSpEndcapA();
    m_multiplicity[SCT_SP_BL] = trigSpacePointCounts->sctSpBarrel();
    m_totNumSctSP = m_multiplicity[SCT_SP_EC] + m_multiplicity[SCT_SP_EA] + m_multiplicity[SCT_SP_BL];

    if(  m_multiplicity[SCT_SP_EA] > m_multiplicity[SCT_SP_EC] )
      m_maxNumSctSpEC =  m_multiplicity[SCT_SP_EA];
    else
      m_maxNumSctSpEC =  m_multiplicity[SCT_SP_EC];

  }
  else{
    if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Won't use SCT." << endreq;

    // do not decide on sct features and use OR mode
    sctSpPass = false;
  }

  if( timerSvc() ) m_formFeaturesTimer->stop();


  // --- Now build the trigger decision.

  // move here to have meaningful monitoring
  if( m_acceptAll ){
    pass=true;
    if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Accepting all events in " << name() << endreq;
    return HLT::OK;
  }

  // Pixel trigger logic  
  if(m_totalPixelClus_cut >= 0.){
    if( ((m_totNumPixSP <= m_totalPixelClus_cut)&&!m_vetoLargeSP) ||
        (((m_totNumPixSP >= m_totalPixelClus_cut) || (m_totNumPixSP <= m_totalPixelClus_cut_min)) && m_vetoLargeSP)) {
      pixelClusPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails TotalPixelNClus, value = " << m_totNumPixSP << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes TotalPixelNClus, value = " << m_totNumPixSP << endreq;
  }

  if(m_pixelClusEndcapC_cut >= 0.){
    if( m_multiplicity[PIXEL_CLUS_EC] <= m_pixelClusEndcapC_cut ){
      pixelClusPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails PixelClusEndcapC, value = " << m_multiplicity[PIXEL_CLUS_EC] << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes PixelClusEndcapC, value = " << m_multiplicity[PIXEL_CLUS_EC] << endreq;
  }

  if(m_pixelClusBarrel_cut >= 0.){
    if( m_multiplicity[PIXEL_CLUS_BL] <= m_pixelClusBarrel_cut ){
      pixelClusPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails PixelClusBarrel, value = " << m_multiplicity[PIXEL_CLUS_BL] << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes PixelClusBarrel, value = " << m_multiplicity[PIXEL_CLUS_BL] << endreq;
  }

  if(m_pixelClusEndcapA_cut >= 0.){
    if( m_multiplicity[PIXEL_CLUS_EA] <= m_pixelClusEndcapA_cut ){
      pixelClusPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails PixelClusEndcapA, value = " << m_multiplicity[PIXEL_CLUS_EA] << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes PixelClusEndcapA, value = " << m_multiplicity[PIXEL_CLUS_EA] << endreq;
  }

  if(m_pixelClusEndcapC_max >= 0.){
    if( m_multiplicity[PIXEL_CLUS_EC] > m_pixelClusEndcapC_max ){
      pixelClusPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails PixelClusEndcapC_max, value = " << m_multiplicity[PIXEL_CLUS_EC] << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes PixelClusEndcapC_max, value = " << m_multiplicity[PIXEL_CLUS_EC] << endreq;
  }

  if(m_pixelClusBarrel_max >= 0.){
    if( m_multiplicity[PIXEL_CLUS_BL] > m_pixelClusBarrel_max ){
      pixelClusPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails PixelClusBarrel_max, value = " << m_multiplicity[PIXEL_CLUS_BL] << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes PixelClusBarrel_max, value = " << m_multiplicity[PIXEL_CLUS_BL] << endreq;
  }

  if(m_pixelClusEndcapA_max >= 0.){
    if( m_multiplicity[PIXEL_CLUS_EA] > m_pixelClusEndcapA_max) {
      pixelClusPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails PixelClusEndcapA_max, value = " << m_multiplicity[PIXEL_CLUS_EA] << endreq;
    }
    else
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes PixelClusEndcapA_max, value = " << m_multiplicity[PIXEL_CLUS_EA] << endreq;
  }

  if( m_pixelClusRatioA_cut >= 0.){
    if( ratioA < m_pixelClusRatioA_cut ){
      pixelClusPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails PixelClusRatioA, value = " << ratioA << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes PixelClusRatioA, value = " << ratioA << endreq;
  }

  if(m_pixelClusRatioB_cut >= 0.){
    if( ratioB < m_pixelClusRatioB_cut) {
      pixelClusPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails PixelClusRatioB, value = " << ratioB << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes PixelClusRatioB, value = " << ratioB << endreq;
  }

  if(m_pixelClusEndcaps_cut >= 0.){
    if( m_maxNumPixClusEC < m_pixelClusEndcaps_cut) {
      pixelClusPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails PixelClusEndcaps, value = " << m_maxNumPixClusEC << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes PixelClusEndcaps, value = " << m_maxNumPixClusEC << endreq;
  }

  // SCT trigger logic
  if(m_totalSctSp_cut >= 0.){
    if( ((m_totNumSctSP <= m_totalSctSp_cut) && !m_vetoLargeSP ) ||
	(((m_totNumSctSP >= m_totalSctSp_cut) || (m_totNumSctSP <= m_totalSctSp_cut_min)) && m_vetoLargeSP)) {
      sctSpPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails TotalSctSp, value = " << m_totNumSctSP << endreq;
    }
  else 
    if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes TotalSctSp, value = " << m_totNumSctSP << endreq;
  }

  if(m_sctSpEndcapC_cut >= 0.){
    if( m_multiplicity[SCT_SP_EC] <= m_sctSpEndcapC_cut) {
      sctSpPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails SctSpEndcapC, value = " << m_multiplicity[SCT_SP_EC] << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes SctSpEndcapC, value = " << m_multiplicity[SCT_SP_EC] << endreq;
  }

  if(m_sctSpBarrel_cut >= 0.){
    if( m_multiplicity[SCT_SP_BL] <= m_sctSpBarrel_cut) {
      sctSpPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails SctSpBarrel, value = " << m_multiplicity[SCT_SP_BL] << endreq;    
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes SctSpBarrel, value = " << m_multiplicity[SCT_SP_BL] << endreq;
  }

  if(m_sctSpEndcapA_cut >= 0.){
    if( m_multiplicity[SCT_SP_EA] <= m_sctSpEndcapA_cut) {
      sctSpPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails SctSpEndcapA, value = " << m_multiplicity[SCT_SP_EA] << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes SctSpEndcapA, value = " << m_multiplicity[SCT_SP_EA] << endreq;
  }

  if(m_sctSpEndcapC_max >= 0.){
    if( m_multiplicity[SCT_SP_EC] > m_sctSpEndcapC_max) { 
      sctSpPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails SctSpEndcapC_max, value = " << m_multiplicity[SCT_SP_EC] << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes SctSpEndcapC_max, value = " << m_multiplicity[SCT_SP_EC] << endreq;
  }

  if(m_sctSpBarrel_max >= 0.){
    if( m_multiplicity[SCT_SP_BL] > m_sctSpBarrel_max) {
      sctSpPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails SctSpBarrel_max, value = " << m_multiplicity[SCT_SP_BL] << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes SctSpBarrel_max, value = " << m_multiplicity[SCT_SP_BL] << endreq;
  }

  if(m_sctSpEndcapA_max >= 0.){ 
    if( m_multiplicity[SCT_SP_EA] > m_sctSpEndcapA_max) {
      sctSpPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails SctSpEndcapA_max, value = " << m_multiplicity[SCT_SP_EA] << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes SctSpBarrel_max, value = " << m_multiplicity[SCT_SP_EA] << endreq;
  }

  if(m_sctSpEndcaps_cut >= 0.){ 
    if( m_maxNumSctSpEC < m_sctSpEndcaps_cut ) {
      sctSpPass = false;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails SctSpEndcaps, value = " << m_maxNumSctSpEC << endreq;
    }
    else 
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes SctSpEndcaps, value = " << m_maxNumSctSpEC << endreq;
  }

  // Check the trigger decision.
  if( m_triggerTypeAND ){
    if( pixelClusPass && sctSpPass ){
      pass = true;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes Pixel AND SCT " << endreq;
    }
    else
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails Pixel AND SCT " << endreq;
  }
  else{
    if( pixelClusPass || sctSpPass ) {
      pass = true;
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event passes Pixel OR SCT " << endreq;
    }
    else
      if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Event fails Pixel OR SCT " << endreq;
  }

  // Veto
  if (m_veto == true) {
    pass = !pass;
    if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Using inverted/VETO logic, final decision is " << (pass ? "PASS" : "FAIL") << endreq;
  }

  // for monitoring  
  if( !pass ){
    m_totSelNumPixSP = -999;
    m_totSelNumSctSP = -999;
  }
  else{
    m_totSelNumPixSP = m_totNumPixSP;
    m_totSelNumSctSP = m_totNumSctSP;
  }
  trigSpacePointCounts = 0;
  return HLT::OK;  
}

//-----------------------------------------------------------------------------

HLT::ErrorCode TrigCountSpacePointsHypo::hltFinalize() {
  m_log << MSG::DEBUG << " finalizing this TrigCountSpacePointsHypo : " << name() << endreq; 

  return HLT::OK;  
}
