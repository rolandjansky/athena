/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2MinBias/T2MbtsHypo.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"

#include <bitset>
#include <cmath>

//-----------------------------------------------------------------------------

T2MbtsHypo::T2MbtsHypo(const std::string &name, 
		       ISvcLocator* pSvcLocator): HLT::HypoAlgo(name, pSvcLocator),
						  T2MbtsUtils(),
						  m_log(msgSvc(), name),
						  m_acceptAll(false),
						  m_mbtsCounters(0),
						  m_coincidence(false),
						  m_or(false),
						  m_veto(false),
						  m_mbtsmode(0),
						  m_t2MbtsBits(0) {

  declareProperty("AcceptAll",                          m_acceptAll=false);
  declareProperty("MbtsCounters",                       m_mbtsCounters=2);
  declareProperty("Coincidence",                        m_coincidence=false);
  declareProperty("Or",                                 m_or=false);
  declareProperty("Veto",                               m_veto=false);
  declareProperty("MBTSMode",                           m_mbtsmode=0);
  declareProperty("Threshold",                          m_threshold = 40.0/222.0 );  // Value in pC
  declareProperty("TimeCut",                            m_timeCut=-1.0);
  declareProperty("GlobalTimeOffset",                   m_globalTimeOffset=0.0);
  declareProperty("TimeOffsetA0",                       m_timeOffsets[0]=0.0);
  declareProperty("TimeOffsetA1",                       m_timeOffsets[1]=0.0);
  declareProperty("TimeOffsetA2",                       m_timeOffsets[2]=0.0);
  declareProperty("TimeOffsetA3",                       m_timeOffsets[3]=0.0);
  declareProperty("TimeOffsetA4",                       m_timeOffsets[4]=0.0);
  declareProperty("TimeOffsetA5",                       m_timeOffsets[5]=0.0);
  declareProperty("TimeOffsetA6",                       m_timeOffsets[6]=0.0);
  declareProperty("TimeOffsetA7",                       m_timeOffsets[7]=0.0);
  declareProperty("TimeOffsetA8",                       m_timeOffsets[8]=0.0);
  declareProperty("TimeOffsetA9",                       m_timeOffsets[9]=0.0);
  declareProperty("TimeOffsetA10",                      m_timeOffsets[10]=0.0);
  declareProperty("TimeOffsetA11",                      m_timeOffsets[11]=0.0);
  declareProperty("TimeOffsetA12",                      m_timeOffsets[12]=0.0);
  declareProperty("TimeOffsetA13",                      m_timeOffsets[13]=0.0);
  declareProperty("TimeOffsetA14",                      m_timeOffsets[14]=0.0);
  declareProperty("TimeOffsetA15",                      m_timeOffsets[15]=0.0);
  declareProperty("TimeOffsetC0",                       m_timeOffsets[16]=0.0);
  declareProperty("TimeOffsetC1",                       m_timeOffsets[17]=0.0);
  declareProperty("TimeOffsetC2",                       m_timeOffsets[18]=0.0);
  declareProperty("TimeOffsetC3",                       m_timeOffsets[19]=0.0);
  declareProperty("TimeOffsetC4",                       m_timeOffsets[20]=0.0);
  declareProperty("TimeOffsetC5",                       m_timeOffsets[21]=0.0);
  declareProperty("TimeOffsetC6",                       m_timeOffsets[22]=0.0);
  declareProperty("TimeOffsetC7",                       m_timeOffsets[23]=0.0);
  declareProperty("TimeOffsetC8",                       m_timeOffsets[24]=0.0);
  declareProperty("TimeOffsetC9",                       m_timeOffsets[25]=0.0);
  declareProperty("TimeOffsetC10",                      m_timeOffsets[26]=0.0);
  declareProperty("TimeOffsetC11",                      m_timeOffsets[27]=0.0);
  declareProperty("TimeOffsetC12",                      m_timeOffsets[28]=0.0);
  declareProperty("TimeOffsetC13",                      m_timeOffsets[29]=0.0);
  declareProperty("TimeOffsetC14",                      m_timeOffsets[30]=0.0);
  declareProperty("TimeOffsetC15",                      m_timeOffsets[31]=0.0);

  declareMonitoredVariable("MultiplicityEBA",           m_mult.first); // Number of counters within cuts, side A
  declareMonitoredVariable("MultiplicityEBC",           m_mult.second); // Number of counters within cuts, side C
  declareMonitoredVariable("TimeDiff_A_C",              m_timeDiff_A_C);  // Time difference A - C
  declareMonitoredVariable("SelMultiplicityEBA",        m_selMultEBA); 
  declareMonitoredVariable("SelMultiplicityEBC",        m_selMultEBC); 

}

//-----------------------------------------------------------------------------
HLT::ErrorCode T2MbtsHypo::hltInitialize() {
  m_log.setLevel(outputLevel());
  if(msgLvl() <= MSG::INFO) {
    m_log << MSG::INFO << "Initialising this T2MbtsFex: " << name() << endreq;
    
    m_log << MSG::INFO << "================ Hypo Settings ================" << endreq;
    m_log << MSG::INFO << " AcceptAll -------------------  " << (m_acceptAll==true ? "True" : "False") << endreq; 
    m_log << MSG::INFO << " MbtsCounters required -------  " << m_mbtsCounters << endreq;
    m_log << MSG::INFO << " Coincidence requirement -----  " << (m_coincidence==true ? "True" : "False") << endreq;
    m_log << MSG::INFO << " Veto requirement ------------  " << (m_veto==true ? "True" : "False") << endreq;
    m_log << MSG::INFO << " MBTS mode -------------------  " << (m_mbtsmode==0 ? "All" : ( m_mbtsmode==1 ? "Inner" : "Outer")) << endreq;
    m_log << MSG::INFO << " Threshold -------------------  " << m_threshold << " pC" << endreq;
    m_log << MSG::INFO << " TimeCut  --------------------  "; 
    if(m_timeCut<0) {
      m_log << MSG::INFO << "Off" << endreq;
    }
    else { 
      m_log << MSG::INFO << m_timeCut << " ns" << endreq;
      m_log << MSG::INFO << " GlobalTimeOffset  -----------  " << m_globalTimeOffset << " ns" << endreq;
      m_log << MSG::INFO << " TimeOffsets (A0 to C16) -----  ";
      for(int i=0; i<32; i++)  m_log << MSG::INFO << m_timeOffsets[i] << " ";
      m_log << MSG::INFO << " ns" << endreq;
    }
    m_log << MSG::INFO << "===============================================" << endreq;
  }
 
  // little consistency checker
  if ( m_or && m_coincidence ) {
    m_log << MSG::ERROR << "The Or and Coincidence logics can not be used together" << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }

  return HLT::OK;
}

//-----------------------------------------------------------------------------

HLT::ErrorCode T2MbtsHypo::hltExecute(const HLT::TriggerElement* outputTE, 
				      bool& pass) {
  if(msgLvl() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "Executing this T2MbtsHypo " << name() << endreq;
  }
     
  pass = false;
  
  // Try to retrieve the TrigT2MbtsBits object produced by the Fex
  if(getFeature(outputTE, m_t2MbtsBits, "T2Mbts") != HLT::OK){
    if(msgLvl() <= MSG::WARNING){
      m_log << MSG::WARNING << "Failed to retrieve features from TE." << endreq;
    }
    return HLT::OK;
  }
  
  // If the object is not available the trigger fails without complaint.
  if(!m_t2MbtsBits){
    m_log << MSG::DEBUG << "No trigger bits formed." << endreq;
    return HLT::OK;
  }
  
  // Print the contents of the object if debug is on.
  if(msgLvl() <= MSG::DEBUG) {
//    m_t2MbtsBits->print(m_log);
  }

  // Calculate MBTS counter multiplicities after energy and an optional time cut.
  if(!calculateMultiplicities(m_t2MbtsBits,m_mbtsmode, m_log,msgLvl())) {
    m_log << MSG::DEBUG << "calculateMultiplicities failed" << endreq;
    return HLT::OK;
  } 

  // move flag here for monitoring
  if(m_acceptAll){

    pass=true;
    m_selMultEBA = m_mult.first;
    m_selMultEBC = m_mult.second;

    if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Accepting all events in " << name() << endreq;
    return HLT::OK;
  }


  
  if(m_coincidence) { // Coincidence logic
    if(!m_veto){
      if(m_mult.first >= m_mbtsCounters && m_mult.second >= m_mbtsCounters) 
	pass = true;
    } else {
      if(m_mult.first < m_mbtsCounters && m_mult.second < m_mbtsCounters) 
	pass = true;
    }
  } else {
      if ( m_or ) { // Or logic
	if(!m_veto){
	  if ((m_mult.first >= m_mbtsCounters || m_mult.second >= m_mbtsCounters)) 
	    pass = true;       
	} else {
	  if ((m_mult.first < m_mbtsCounters || m_mult.second < m_mbtsCounters)) 
	    pass = true;       
	}
      } else {   // Sum logic
	if(!m_veto){
	  if((m_mult.first + m_mult.second) >= m_mbtsCounters) 
	    pass = true;
	} else {
	  if((m_mult.first + m_mult.second) < m_mbtsCounters) 
	    pass = true;
	}
      }
  }
  
  if(msgLvl() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "REGTEST: event " << (pass?"accepted":"failed") << " with EBA(" << m_mult.first << ") + EBC(" << m_mult.second << ") hits above threshold." <<  endreq;
  }


  if( pass ){
    m_selMultEBA = m_mult.first;
    m_selMultEBC = m_mult.second;
  }
  else{
    m_selMultEBA = -999;
    m_selMultEBC = -999;
  }

  return HLT::OK;
}

//-----------------------------------------------------------------------------

HLT::ErrorCode T2MbtsHypo::hltFinalize() {
  return HLT::OK;
}

//-----------------------------------------------------------------------------
