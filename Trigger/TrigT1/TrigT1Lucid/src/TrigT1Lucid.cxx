/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1Lucid.h"
#include "TrigT1Interfaces/LucidCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigT1Interfaces/BitOp.h"

namespace LVL1 {

  //---------------------------------------------------------------------

  TrigT1Lucid::TrigT1Lucid(const std::string& name, 
			   ISvcLocator* pSvcLocator): 
    AthAlgorithm(name, pSvcLocator), 
    m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
    m_LUCID_LVL1T1ContainerName("Lucid_Digits"),
    m_digitContainer(0),
    m_threshold_a (-1),
    m_threshold_c (-1),
    m_cablestart_a(-1),
    m_cablestart_c(-1),
    m_qdcThreshold(500), 
    m_badDataFound(false)
  {
    declareProperty("LVL1ConfigSvc"      , m_configSvc, "LVL1 Config Service");
    declareProperty("LVL1T1ContainerName", m_LUCID_LVL1T1ContainerName = "Lucid_Digits");
    declareProperty("qdcThreshold"       , m_qdcThreshold);
  }

  //---------------------------------------------------------------------

  StatusCode TrigT1Lucid::initialize() {
    if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "entering intialize()" << endmsg;
    
    if (AthAlgorithm::initialize().isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't initialize Algorithm base class." << endmsg;
      return StatusCode::FAILURE;
    }

    StatusCode sc;
    
    // Connect to the LVL1ConfigSvc to retrieve threshold settings.
    sc = m_configSvc.retrieve();
    if (sc.isFailure()) { 
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't connect to " 
					      << m_configSvc.typeAndName() 
					      << endmsg; 
      return sc;
    }
    else if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Connected to " << m_configSvc.typeAndName() 
		      << endmsg;
    }

    // Get level 1 LUCID threshold settings from the level 1
    // configuration service.  The thresholds are in QDC and are not a
    // ptcut.  The ptcut() methods just returns the value in the xml file.
    std::vector<TrigConf::TriggerThreshold*>           thresholds = m_configSvc->thresholdConfig()->getLucidThresholdVector();
    std::vector<TrigConf::TriggerThreshold*>::iterator th_itr     = thresholds.begin();
    std::vector<TrigConf::TriggerThreshold*>::iterator th_itr_end = thresholds.end();
    
    for(; th_itr!=th_itr_end; th_itr++) {
      if(msgLvl(MSG::DEBUG)) {
	msg(MSG::DEBUG) << "Threshold name  =" << (*th_itr)->name() << endmsg;
	msg(MSG::DEBUG) << "Threshold value =" << (*th_itr)->triggerThresholdValue(0, 0)->ptcut() << endmsg;
      }

      if     ((*th_itr)->name() == "LUCID_A"){ 
	m_cablestart_a = (*th_itr)->cableStart(); 
	m_threshold_a = (*th_itr)->triggerThresholdValue(0, 0)->ptcut();
      } else if((*th_itr)->name() == "LUCID_C"){
	m_cablestart_c = (*th_itr)->cableStart();
	m_threshold_c = (*th_itr)->triggerThresholdValue(0, 0)->ptcut();
      }

    }
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Cable positions: " 
					  << m_cablestart_a << ", " 
					  << m_cablestart_c << endmsg;
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------

  StatusCode TrigT1Lucid::execute() {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "execute()" << endmsg;
    
    m_digitContainer = 0;
    std::string containerName = m_LUCID_LVL1T1ContainerName;

    if(m_badDataFound) return StatusCode::SUCCESS;

    StatusCode sc = evtStore()->retrieve(m_digitContainer, containerName);

    if (sc.isFailure() || !m_digitContainer) {
      if(msgLvl(MSG::WARNING)) {
	msg(MSG::WARNING) << "BAD DATA!!! Input data does not include " << containerName << endmsg;
	msg(MSG::WARNING) << "Alogrithm will shut down for the rest of the run." << endmsg;
      }
      m_badDataFound = true;
      return StatusCode::SUCCESS;
    } 
    else if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << containerName << " Container Successfully Retrieved"
		      << endmsg; 
    }


    LUCID_DigitConstIterator itr     = m_digitContainer->begin();
    LUCID_DigitConstIterator itr_end = m_digitContainer->end();
    
    unsigned short nHitsA = 0;
    unsigned short nHitsC = 0;
    
    for (; itr!=itr_end; itr++) {

      unsigned short tubeID   = (*itr)->getTubeID();
      unsigned short qdcCount = (*itr)->getQDC();
      const bool     isSideA  = (*itr)->isSideA();
      const bool     isSideC  = (*itr)->isSideC();
      m_qdcThreshold = isSideA ? m_threshold_a : m_threshold_c; 

// Instead of reading the threshold from menu, use the isHit method from Digitization where thresholds are correct
//       const bool     isHit    = (qdcCount > m_qdcThreshold );
      const bool     isHit    = (*itr)->isHit();
      
      if      (isHit && isSideA) nHitsA++;
      else if (isHit && isSideC) nHitsC++;

      if(msgLvl(MSG::DEBUG)) {
	msg(MSG::DEBUG) << " tubeID     : " << tubeID
			<< " qdcCount   : " << qdcCount
			<< " isSideA    : " << isSideA 
			<< " isSideC    : " << isSideC
			<< " qdcThresold: " << m_qdcThreshold
			<< " isHit      : " << isHit << endmsg;
      }
    }
    
    unsigned int cableWord0 = 0;
    
    if (nHitsA) cableWord0 += 1 << m_cablestart_a;
    if (nHitsC) cableWord0 += 1 << m_cablestart_c; 
    
    if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << " nHitsA "           << nHitsA     << " ," 
		      << " nHitsC "           << nHitsC     << " ," 
		      << " lucid cableWord0 " << cableWord0 << " ," 
		      << " bits "             << std::setw(6) 
		      << BitOp::printBits(cableWord0, 0, 20) << endmsg;
    }
    
    LucidCTP* lucidCTP = new LucidCTP(cableWord0);

    if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << lucidCTP->print();
      msg(MSG::DEBUG) << lucidCTP->dump();
    }
    
    containerName = DEFAULT_LucidCTPLocation;
    
    sc = evtStore()->record(lucidCTP, containerName, false);
    
    if (sc.isFailure()) { 
      if(msgLvl(MSG::ERROR)) {
	msg(MSG::ERROR) << containerName << " failed to register. " 
			<< endmsg; 
      }
      return StatusCode::FAILURE;
    }
    else if(msgLvl(MSG::DEBUG)) { 
      msg(MSG::DEBUG) << containerName << " registered successfully. " 
		      << endmsg;
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------

  StatusCode TrigT1Lucid::finalize() { return StatusCode::SUCCESS; }
}
