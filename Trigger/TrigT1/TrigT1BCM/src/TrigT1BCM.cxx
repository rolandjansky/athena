/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <stdint.h>
#include "TrigT1BCM.h"
#include "TrigT1Interfaces/BcmCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigConfL1Data/ThresholdConfig.h"

#include "InDetBCM_RawData/BCM_RawData.h"
#include "InDetBCM_RawData/BCM_RDO_Collection.h"

namespace LVL1 {
  
  TrigT1BCM::TrigT1BCM(const std::string& name,
		       ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator), 
						  m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
						  m_bcmL1ContainerName("BCM_RDOs"),
						  m_bcmRDO(0),
						  m_6bit_flag(false),
						  m_badDataFound(false)
  {
    declareProperty("LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
    declareProperty("BcmL1ContainerName",     m_bcmL1ContainerName = "BCM_RDOs");
    declareProperty("OutOfTimePulsePosition", m_OOTPulseX      = 11); // out-of-time pulse position
    declareProperty("InTimePulsePosition",    m_ITPulseX       = 43); // in-time pulse position
    declareProperty("WideInTimePulsePosition",m_WITPulseX      = 38); // Wide in-time pulse position
    declareProperty("NarrowTimeWindow",       m_TimeWindow     =  7); // width of time window in 390ns bins
    declareProperty("WideTimeWindow",         m_WideTimeWindow = 10); // width of wide time window in 390ns bins
  }

  StatusCode TrigT1BCM::initialize() {
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Initialising" << endmsg;

    if (AthAlgorithm::initialize().isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't initialize Algorithm base class." << endmsg;
      return StatusCode::FAILURE;
    }

    StatusCode sc;

    // Connect to the LVL1ConfigSvc to retrieve threshold settings.
    sc = m_configSvc.retrieve();
    if (sc.isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't connect to " << m_configSvc.typeAndName() << endmsg;
      return sc;
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Connected to " << m_configSvc.typeAndName() << endmsg;
    }
    
    // Get level 1 BCM threshold settings from the level 1
    // configuration service.
    m_cablestarts[0] = 0; //BCM_AtoC
    m_cablestarts[1] = 1; //BCM_CtoA
    m_cablestarts[2] = 2; //BCM_Wide
    m_cablestarts[3] = 3; //BCM_A1
    m_cablestarts[4] = 4; //BCM_A2
    m_cablestarts[5] = 5; //BCM_C1
    m_cablestarts[6] = 6; //BCM_C2
    m_cablestarts[7] = 7; //BCM_HTA
    m_cablestarts[8] = 8; //BCM_HTC
    m_cablestarts[9] = 3; //BCM_6BITMULTI    
    std::vector<TrigConf::TriggerThreshold*> thresholds = m_configSvc->thresholdConfig()->getBcmThresholdVector();
    std::vector<TrigConf::TriggerThreshold*>::iterator th_itr = thresholds.begin();
    std::vector<TrigConf::TriggerThreshold*>::iterator th_itr_end = thresholds.end();
    for(;th_itr!=th_itr_end;th_itr++) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "1-bit Threshold name=" << (*th_itr)->name() << endmsg;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "1-bit Threshold value=" << (*th_itr)->triggerThresholdValue(0, 0)->ptcut() << endmsg;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "1-bit Thershold cablestart=" << (*th_itr)->cableStart() << endmsg;
      if        ((*th_itr)->name() == "BCM_AtoC") {
        m_cablestarts[0]=(*th_itr)->cableStart();
      } else if ((*th_itr)->name() == "BCM_CtoA") {
        m_cablestarts[1]=(*th_itr)->cableStart();
      } else if ((*th_itr)->name() == "BCM_Wide") {
        m_cablestarts[2]=(*th_itr)->cableStart();
      } else if ((*th_itr)->name() == "BCM_A1") {
        m_cablestarts[3]=(*th_itr)->cableStart();
      } else if ((*th_itr)->name() == "BCM_A2") {
        m_cablestarts[4]=(*th_itr)->cableStart();
      } else if ((*th_itr)->name() == "BCM_C1") {
        m_cablestarts[5]=(*th_itr)->cableStart();
      } else if ((*th_itr)->name() == "BCM_C2") {
        m_cablestarts[6]=(*th_itr)->cableStart();
      } else if ((*th_itr)->name() == "BCM_HTA") {
        m_cablestarts[7]=(*th_itr)->cableStart();
      } else if ((*th_itr)->name() == "BCM_HTC") {
        m_cablestarts[8]=(*th_itr)->cableStart();
      }      
    }
    std::vector<TrigConf::TriggerThreshold*> cmbthresholds = m_configSvc->thresholdConfig()->getBcmcmbThresholdVector();
    std::vector<TrigConf::TriggerThreshold*>::iterator cmb_th_itr = cmbthresholds.begin();
    std::vector<TrigConf::TriggerThreshold*>::iterator cmb_th_itr_end = cmbthresholds.end();
    for(;cmb_th_itr!=cmb_th_itr_end;cmb_th_itr++) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "3-bit Threshold name=" << (*cmb_th_itr)->name() << endmsg;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "3-bit Threshold value=" << (*cmb_th_itr)->triggerThresholdValue(0, 0)->ptcut() << endmsg;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "3-bit Thershold cablestart=" << (*cmb_th_itr)->cableStart() << endmsg;
      if ((*cmb_th_itr)->name() == "BCM_6BITMULTI") {
        m_cablestarts[9]=(*cmb_th_itr)->cableStart();
      }
    }
    
    if (cmbthresholds.size() == 1) {
      m_6bit_flag = true;
    } else {
      m_6bit_flag = false;
    } 

    return StatusCode::SUCCESS;
  }

  StatusCode TrigT1BCM::execute() {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "execute()" << endmsg;
    
    StatusCode sc;
    
    //    Retrieve Lvl1 BCM container
    m_bcmRDO = 0;
    sc = evtStore()->retrieve( m_bcmRDO, m_bcmL1ContainerName);
    if( sc.isFailure()  || !m_bcmRDO ) {
      if (!m_badDataFound) {  
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << m_bcmL1ContainerName << " not found" << endmsg; 
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Algorithm will be turned off for the rest of the run." << endmsg;
	m_badDataFound = true;
	return StatusCode::SUCCESS;
      } else {
	return StatusCode::SUCCESS;
      }
    }
    else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << m_bcmL1ContainerName << " Container Successfully Retrieved" << endmsg;
    }

  
    // Loop over all Lvl 1 BCM hits
    BCM_RDO_Container::const_iterator l1_bcm_itr = m_bcmRDO->begin();
    BCM_RDO_Container::const_iterator l1_bcm_itr_end = m_bcmRDO->end();

    //Counters for number of modules with in/out-of-time hits on a/c side
    int nOOTModA = 0;
    int nOOTModC = 0;
    int nITModA = 0;
    int nITModC = 0;
    //Counters for number of modules with wide in/out-of-time hits on a/c side
    //int nWOOTModA = 0;
    //int nWOOTModC = 0;
    int nWITModA = 0;
    int nWITModC = 0;
    
    bool AttHitA = false; // is there a hit in the low gain channels, a side?
    bool AttHitC = false; // is there a hit in the low gain channels, c side?

    for (; l1_bcm_itr != l1_bcm_itr_end; l1_bcm_itr++) {
      // Find out which BCM channel this is.
      int channelID = (*l1_bcm_itr)->getChannel();
      
      if ( (*l1_bcm_itr)->size() != 0) {
	BCM_RDO_Collection::const_iterator sample = (*l1_bcm_itr)->begin();
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Channel ID: " << channelID << endmsg;
	
	int pulse1x = (*sample)->getPulse1Position();
	int pulse1w = (*sample)->getPulse1Width();
	int pulse2x = (*sample)->getPulse2Position();
	int pulse2w = (*sample)->getPulse2Width();

	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " pulse1pos: " << pulse1x << " pulse1width: " << pulse1w << endmsg;
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " pulse1pos: " << pulse2x << " pulse1width: " << pulse2w << endmsg;

	if (channelID >= 8 && channelID < 12) { // high gain channels, a side
	    if ((pulse1x > m_OOTPulseX - m_TimeWindow)     && (pulse1x < m_OOTPulseX + m_TimeWindow))     nOOTModA++;
	    if ((pulse1x > m_ITPulseX  - m_TimeWindow)     && (pulse1x < m_ITPulseX  + m_TimeWindow))     nITModA++;
	    if ((pulse1x > m_WITPulseX  - m_WideTimeWindow) && (pulse1x < m_WITPulseX  + m_WideTimeWindow)) nWITModA++;

	    if ((pulse2x > m_OOTPulseX - m_TimeWindow)     && (pulse2x < m_OOTPulseX + m_TimeWindow))     nOOTModA++;
	    if ((pulse2x > m_ITPulseX  - m_TimeWindow)     && (pulse2x < m_ITPulseX  + m_TimeWindow))     nITModA++;
	    if ((pulse2x > m_WITPulseX  - m_WideTimeWindow) && (pulse2x < m_WITPulseX  + m_WideTimeWindow)) nWITModA++;
	} else if (channelID >= 12 && channelID < 16) { // high gain channels, c side
	    if ((pulse1x > m_OOTPulseX - m_TimeWindow)     && (pulse1x < m_OOTPulseX + m_TimeWindow))     nOOTModC++;
	    if ((pulse1x > m_ITPulseX  - m_TimeWindow)     && (pulse1x < m_ITPulseX  + m_TimeWindow))     nITModC++;
	    if ((pulse1x > m_WITPulseX  - m_WideTimeWindow) && (pulse1x < m_WITPulseX  + m_WideTimeWindow)) nWITModC++;
	  
	    if ((pulse2x > m_OOTPulseX - m_TimeWindow)     && (pulse2x < m_OOTPulseX + m_TimeWindow))     nOOTModC++;
	    if ((pulse2x > m_ITPulseX  - m_TimeWindow)     && (pulse2x < m_ITPulseX  + m_TimeWindow))     nITModC++;
	    if ((pulse2x > m_WITPulseX  - m_WideTimeWindow) && (pulse2x < m_WITPulseX  + m_WideTimeWindow)) nWITModC++;
	  
	} else if (channelID >= 0 && channelID < 4) { // low gain channels, a side
	  if (pulse1w != 0 || pulse2w != 0) AttHitA = true;
	} else if (channelID >= 4 && channelID < 8) { // low gain channels, c side
	  if (pulse1w != 0 || pulse2w !=0) AttHitC = true;
	}
      }
    }
    // Check if event passes trigger bit requirements
    bool bit0 = false;
    bool bit1 = false; 
    bool bit2 = false; 
    bool bit3 = false;
    bool bit4 = false;
    bool bit5 = false;
    bool bit6 = false; 
    bool bit7 = false;
    bool bit8 = false; 
    if (nOOTModA>0 && nITModC>0) bit0 = true;
    if (nOOTModC>0 && nITModA>0) bit1 = true;
    if (nWITModA>0 && nWITModC>0) bit2 = true;
    if (nITModA<3) {
      bit3 = (nITModA & 0x1)>>0;
      bit4 = (nITModA & 0x2)>>1;
    } else { //multiplicity 3+ case
      bit3 = true;
      bit4 = true;
    }
    if (nITModC<3) {
      bit5 = (nITModC & 0x1)>>0;
      bit6 = (nITModC & 0x2)>>0;
    } else { //multiplicity 3+ case
      bit5 = true;
      bit6 = true;
    }
    //if (AttHitA>0) bit7 = 1;
    //if (AttHitC>0) bit8 = 1;
    // static code analyser cpp check 1631905147 and 4059822314
    // Comparison of a boolean with an integer
    if (AttHitA) bit7 = 1;
    if (AttHitC) bit8 = 1;
    
    uint32_t cableWord0 = 0;
    if (m_6bit_flag) {
      if (bit7 || bit8) {
        cableWord0 = (0x7<<m_cablestarts[9]) | (bit2<<m_cablestarts[2]) | (bit1<<m_cablestarts[1]) | (bit0<<m_cablestarts[0]);
      } else {
        cableWord0 = (((((bit6<<1)|bit5)+((bit4<<1)|bit3))&0x7)<<m_cablestarts[9]) | (bit2<<m_cablestarts[2]) | (bit1<<m_cablestarts[1]) | (bit0<<m_cablestarts[0]);
      }
    } else {
      cableWord0 = (bit8<<m_cablestarts[8]) | (bit7<<m_cablestarts[7]) | (bit6<<m_cablestarts[6]) | (bit5<<m_cablestarts[5]) | (bit4<<m_cablestarts[4]) | (bit3<<m_cablestarts[3]) | (bit2<<m_cablestarts[2]) | (bit1<<m_cablestarts[1]) | (bit0<<m_cablestarts[0]);
    }
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " cableWord: " << cableWord0 << endmsg;    

    // Record the CTP trigger word in StoreGate.
    BcmCTP *bcmCTP = new BcmCTP(cableWord0);
    sc = evtStore()->record(bcmCTP, LVL1::DEFAULT_BcmCTPLocation, false);
    if(sc.isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Failed to register " << LVL1::DEFAULT_BcmCTPLocation << endmsg;
      return StatusCode::FAILURE;
    } 
    else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << LVL1::DEFAULT_BcmCTPLocation << " registered successfully" << endmsg;
    }
    
    return StatusCode::SUCCESS;
  }

  StatusCode TrigT1BCM::finalize() {
    return StatusCode::SUCCESS;
  }
}
