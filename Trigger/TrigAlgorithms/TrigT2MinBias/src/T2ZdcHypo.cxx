/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2MinBias/T2ZdcHypo.h"
#include "xAODTrigMinBias/TrigT2ZdcSignals.h"

#include <bitset>
#include <cmath>

//-----------------------------------------------------------------------------

T2ZdcHypo::T2ZdcHypo(const std::string &name, 
		     ISvcLocator* pSvcLocator): HLT::HypoAlgo(name, pSvcLocator),
						//  T2ZdcUtils(),
						m_log(msgSvc(), name),
						m_acceptAll(false),
						m_t2ZdcSignals(0) {
  
  declareProperty("AcceptAll",                    m_acceptAll=false);
  //  declareProperty("Threshold",                    m_threshold = 40.0/222.0 );  // Value in pC
  // declareProperty("GlobalTimeOffset",             m_globalTimeOffset=0.0);

  declareProperty("TimeLogic",          m_timeLogic=0); // 0 no cut, 1 In the window, 2 out of window
  declareProperty("EnergyLogic",        m_energyLogic=1);// 0 no cut, 1 AND, 2 = OR
  declareProperty("MultiplicityLogic",  m_multiLogic=0);

  declareProperty("TimeOffset",           m_timeOffset);
  declareProperty("Pedestal",             m_EnergyPed);
  declareProperty("EnergyCalibration",             m_EnergyCal);

  declareProperty("TimeModuleCut",        m_timeModuleCut=99999.); 

  declareProperty("SumEnergyCut",            m_SumEn);
  declareProperty("MultCut",                 m_Mult);
  declareProperty("TimeCut",                 m_timeCut=-1.0);


  declareMonitoredVariable("MultiplicityZDC_A",     m_mult.first); // Number of counters within cuts, side A
  declareMonitoredVariable("MultiplicityZDC_C",     m_mult.second); // Number of counters within cuts, side C
  declareMonitoredVariable("TimeZDC_A",             m_meanTime.first);  // Mean Time A
  declareMonitoredVariable("TimeZDC_C",             m_meanTime.second);  // Mean Time C
  declareMonitoredVariable("SumEnergyZDC_A",        m_sumEn.first); 
  declareMonitoredVariable("SumEnergyZDC_C",        m_sumEn.second);   
  declareMonitoredVariable("TimeDiff_A_C",          m_timeDiff_A_C);  // Time difference A - C

  declareMonitoredVariable("SelMultiplicityZDC_A",  m_selMult_A); 
  declareMonitoredVariable("SelMultiplicityZDC_C",  m_selMult_C); 
  declareMonitoredVariable("SelTimeZDC_A",          m_selMeanTime_A);  // Mean Time A
  declareMonitoredVariable("SelTimeZDC_C",          m_selMeanTime_C);  // Mean Time C
  declareMonitoredVariable("SelSumEnergyZDC_A",     m_selSumEn_A); 
  declareMonitoredVariable("SelSumEnergyZDC_C",     m_selSumEn_C); 
  declareMonitoredVariable("SelTimeDiff_A_C",       m_seltimeDiff_A_C);  // Time difference A - C


}

//-----------------------------------------------------------------------------
HLT::ErrorCode T2ZdcHypo::hltInitialize() {
  m_log.setLevel(outputLevel());
  if(msgLvl() <= MSG::INFO) {
    m_log << MSG::INFO << "Initialising this T2ZdcFex: " << name() << endreq;
    
    m_log << MSG::INFO << "================ Hypo Settings ================" << endreq;
    m_log << MSG::INFO << " AcceptAll -------------------  " << (m_acceptAll==true ? "True" : "False") << endreq; 
    m_log << MSG::INFO << " SelectionLogic --------------  Time: " <<  m_timeLogic << "  Energy:" << m_energyLogic << " Multi:" << m_multiLogic << endreq;
    for(int ic=0;ic< ZDC_MOD*2 ;++ic){
      m_log << MSG::INFO << " TimeOffset[" << ic << " ]  " <<  m_timeOffset[ic] << "  Pedestal" << m_EnergyPed[ic] << " EnergyModuleCalibrations: " <<  m_EnergyCal[ic] << endreq;
    }
    m_log << MSG::INFO << " TimeModuleCut" << m_timeModuleCut << endreq;
    m_log << MSG::INFO << " TimeDiffCut" << m_timeCut << endreq;
    m_log <<  MSG::INFO << " EnergyCut: SideA " << m_SumEn[0] << "  " << m_SumEn[1] << endreq;
    m_log <<  MSG::INFO << " EnergyCut: SideC " << m_SumEn[2] << "  " << m_SumEn[3] << endreq;
    m_log <<  MSG::INFO << " MultiCut: SideA " << m_Mult[0] << "  SideC " << m_Mult[1] << endreq;
    m_log << MSG::INFO << "===============================================" << endreq;
  }
 
  return HLT::OK;
}

//-----------------------------------------------------------------------------

HLT::ErrorCode T2ZdcHypo::hltExecute(const HLT::TriggerElement* outputTE, 
				      bool& pass) {
  if(msgLvl() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "Executing this T2ZdcHypo " << name() << endreq;
  }
     
  pass = false;
  bool passflags[3];
  for(int ib=0; ib<3; ++ib) passflags[ib]=false;
  
  
  // Try to retrieve the TrigT2ZdcSignals object produced by the Fex
  if(getFeature(outputTE, m_t2ZdcSignals, "zdcsignals") != HLT::OK){
    if(msgLvl() <= MSG::WARNING){
      m_log << MSG::WARNING << "Failed to retrieve features from TE." << endreq;
    }
    return HLT::OK;
  }
  
  // If the object is not available the trigger fails without complaint.
  if(!m_t2ZdcSignals){
    m_log << MSG::DEBUG << "No trigger bits formed." << endreq;
    return HLT::OK;
  }
  
//   // Print the contents of the object if debug is on.
//   if(msgLvl() <= MSG::DEBUG) {
//     m_t2ZdcSignals->print(m_log);
//   }

  // Calculate ZDC counter multiplicities, time mean and energy sums after energy and an optional time cut.
  if(!calculateSumMean()) { 
    m_log << MSG::DEBUG << "calculateSumMean failed" << endreq;
    return HLT::OK;
  }


  if(m_acceptAll){
    if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Accepting all events in " << name() << endreq;
    pass=true;
    m_selMult_A = m_mult.first;
    m_selMult_C = m_mult.second;
    m_selSumEn_A=m_sumEn.first;
    m_selSumEn_C=m_sumEn.second;
    m_selMeanTime_A = m_meanTime.first;
    m_selMeanTime_C = m_meanTime.second;
    m_seltimeDiff_A_C = m_timeDiff_A_C;
    return HLT::OK;
  }

  if(m_timeLogic==2){ // selects out of window
    if(fabs(m_timeDiff_A_C)>m_timeCut) passflags[0] = true;
  }
  if(m_timeLogic==1){ // selects in time of window
    if(fabs(m_timeDiff_A_C)<=m_timeCut) passflags[0] = true;
  } 

  if(m_energyLogic==1){ // AND
    if(m_sumEn.first>= m_SumEn[0] && m_sumEn.first<= m_SumEn[1]){
      if(m_sumEn.second>= m_SumEn[2] && m_sumEn.second<= m_SumEn[3]){
	passflags[1] = true;
      }
    }
  } else if (m_energyLogic==2){ // OR
    if((m_sumEn.first>= m_SumEn[0] && m_sumEn.first<= m_SumEn[1])
       || (m_sumEn.second>= m_SumEn[2] && m_sumEn.second<= m_SumEn[3])){
      passflags[1] = true;
    }
  }
  
  if(m_multiLogic==1){ // AND
    if(m_mult.first >= m_Mult[0] && m_mult.second >= m_Mult[1]){
      passflags[2] = true;
    }
  } else if(m_multiLogic==2){ // OR
    if(m_mult.first >= m_Mult[0] || m_mult.second >= m_Mult[1]){
      passflags[2] = true;
    }
  }
  
  if( ((m_timeLogic==0) || passflags[0]) &&
      ((m_energyLogic==0) || passflags[1]) &&
      ((m_multiLogic==0) || passflags[2])){
    pass=true;
    m_selMult_A = m_mult.first;
    m_selMult_C = m_mult.second;
    m_selSumEn_A=m_sumEn.first;
    m_selSumEn_C=m_sumEn.second;
    m_selMeanTime_A = m_meanTime.first;
    m_selMeanTime_C = m_meanTime.second;
    m_seltimeDiff_A_C = m_timeDiff_A_C;
  } else {
    m_selMult_A = -999;
    m_selMult_C = -999;
    m_selSumEn_A = -999.;
    m_selSumEn_C = -999.;
    m_selMeanTime_A = -999.;
    m_selMeanTime_C = -999.;
    m_seltimeDiff_A_C = -999.;
  }
  return HLT::OK;

}

//-----------------------------------------------------------------------------

HLT::ErrorCode T2ZdcHypo::hltFinalize() {
  return HLT::OK;
}

//-----------------------------------------------------------------------------


int T2ZdcHypo::calculateSumMean(){
  m_mult = std::make_pair(0,0);
  m_meanTime = std::make_pair(0.,0.);
  m_sumEn = std::make_pair(0.,0.);
  if(!m_t2ZdcSignals){
    m_log << MSG::WARNING << "Cannot access T2ZdcSignals " << endreq; 
    return 0;
  }
  std::vector<float> triggerEnergies = m_t2ZdcSignals->triggerEnergies();
  std::vector<float> triggerTimes = m_t2ZdcSignals->triggerTimes();
#ifdef MY_DEBUG
  std::cout << "size of triggerEnergies:" << triggerEnergies.size() << "  triggerTimes: " << triggerTimes.size() << std::endl;
  std::cout << "Dumping triggerEnergies:" << std::endl;
  int itc=0;
  for(std::vector<float>::iterator itf=triggerEnergies.begin();itf!=triggerEnergies.end();++itf,++itc){
    std::cout << "[ " << itc << " --> " << *itf << std::endl;
  }
  itc=0;
  for(std::vector<float>::iterator itf=triggerTimes.begin();itf!=triggerTimes.end();++itf,++itc){
    std::cout << "[ " << itc << " --> " << *itf << std::endl;
  }
#endif
  
  if(triggerEnergies.size() != ZDC_MOD*2 || triggerTimes.size() != ZDC_MOD*2 ) {
    m_log << MSG::WARNING << "Vector sizes " << triggerEnergies.size() << " / " << triggerTimes.size() << " are not equal to number of ZDC counters: " <<  ZDC_MOD*2 << endreq;     
    //    return 0;
  }
  
  // side A
  float sumEnA=0.;
  float timeA=0.;
  int multA=0;
  float sumEnC=0.;
  float timeC=0.;
  int multC=0;
  //  for(int izdc=0; izdc< (TrigT2ZdcSignals::NUM_ZDC); izdc++) {
  for(int izdc=0; izdc< ZDC_MOD*2; izdc++) {
    float time_diff = triggerTimes[izdc]-m_timeOffset[izdc];
    if(triggerEnergies[izdc] <= m_EnergyPed[izdc])  continue;
    if( (fabs(time_diff)>= m_timeModuleCut)) continue;
    if(izdc< ZDC_MOD ){ 
    //    if(izdc<((TrigT2ZdcSignals::NUM_ZDC)/2)){
      sumEnA += m_EnergyCal[izdc] *triggerEnergies[izdc];
      ++multA;
      timeA += time_diff;
    } else {
      sumEnC += m_EnergyCal[izdc] *triggerEnergies[izdc];
      ++multC;
      timeC += time_diff;
    }
  }
  
  if(multA>=1)
    timeA /= multA;
  if(multC>=1)
    timeC /= multC;
  
  m_mult = std::make_pair(multA,multC);
  m_sumEn = std::make_pair(sumEnA,sumEnC);
  m_meanTime = std::make_pair(timeA,timeC);
  m_timeDiff_A_C = timeA-timeC;
  return 1;
}
