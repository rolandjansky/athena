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
  ATH_MSG_INFO("Initialising this T2MbtsFex: " << name());
  ATH_MSG_INFO("================ Hypo Settings ================");
  ATH_MSG_INFO(" AcceptAll -------------------  " << (m_acceptAll==true ? "True" : "False")); 
  ATH_MSG_INFO(" MbtsCounters required -------  " << m_mbtsCounters);
  ATH_MSG_INFO(" Coincidence requirement -----  " << (m_coincidence==true ? "True" : "False"));
  ATH_MSG_INFO(" Veto requirement ------------  " << (m_veto==true ? "True" : "False"));
  ATH_MSG_INFO(" MBTS mode -------------------  " << (m_mbtsmode==0 ? "All" : ( m_mbtsmode==1 ? "Inner" : "Outer")));
  ATH_MSG_INFO(" Threshold -------------------  " << m_threshold << " pC");
  ATH_MSG_INFO(" TimeCut  --------------------  "); 
  if(m_timeCut<0) {
    ATH_MSG_INFO("Off");
  }
  else { 
    ATH_MSG_INFO(m_timeCut << " ns");
    ATH_MSG_INFO(" GlobalTimeOffset  -----------  " << m_globalTimeOffset << " ns");
    ATH_MSG_INFO(" TimeOffsets (A0 to C16) -----  ");
    for(int i=0; i<32; i++)  ATH_MSG_INFO(m_timeOffsets[i] << " ns");
  }
  ATH_MSG_INFO("===============================================");
 
  // little consistency checker
  if ( m_or && m_coincidence ) {
    ATH_MSG_ERROR("The Or and Coincidence logics can not be used together");
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }

  return HLT::OK;
}

//-----------------------------------------------------------------------------

HLT::ErrorCode T2MbtsHypo::hltExecute(const HLT::TriggerElement* outputTE, 
				      bool& pass) {
  ATH_MSG_DEBUG("Executing this T2MbtsHypo " << name());
     
  pass = false;
  
  // Try to retrieve the TrigT2MbtsBits object produced by the Fex
  if(getFeature(outputTE, m_t2MbtsBits, "T2Mbts") != HLT::OK){
    ATH_MSG_WARNING("Failed to retrieve features from TE.");
    return HLT::OK;
  }
  
  // If the object is not available the trigger fails without complaint.
  if(!m_t2MbtsBits){
    ATH_MSG_DEBUG("No trigger bits formed.");
    return HLT::OK;
  }
  
  // Calculate MBTS counter multiplicities after energy and an optional time cut.
  if(!calculateMultiplicities(m_t2MbtsBits,m_mbtsmode, msg(),msgLvl())) {
    ATH_MSG_DEBUG("calculateMultiplicities failed");
    return HLT::OK;
  } 

  // move flag here for monitoring
  if(m_acceptAll){

    pass=true;
    m_selMultEBA = m_mult.first;
    m_selMultEBC = m_mult.second;

    ATH_MSG_DEBUG("Accepting all events in " << name());
    return HLT::OK;
  }


  
  if(m_coincidence) { // Coincidence logic
    if(!m_veto){
      if(m_mult.first >= m_mbtsCounters && m_mult.second >= m_mbtsCounters) pass = true;
    } else {
      if(m_mult.first < m_mbtsCounters && m_mult.second < m_mbtsCounters) pass = true;
    }
  } else {
    if ( m_or ) { // Or logic
	    if(!m_veto) {
	      if ((m_mult.first >= m_mbtsCounters || m_mult.second >= m_mbtsCounters)) pass = true;       
	    } else {
	      if ((m_mult.first < m_mbtsCounters || m_mult.second < m_mbtsCounters)) pass = true;       
	    }
    } else {   // Sum logic
	    if(!m_veto){
	      if((m_mult.first + m_mult.second) >= m_mbtsCounters) pass = true;
	    } else {
	      if((m_mult.first + m_mult.second) < m_mbtsCounters) pass = true;
	    }
    }
  }
  
  ATH_MSG_DEBUG("REGTEST: event " << (pass?"accepted":"failed") << " with EBA(" << m_mult.first << ") + EBC(" << m_mult.second << ") hits above threshold.");

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
