/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//----------------RECO TRIGGER MATCHING TOOL FOR PAU--------------------------
////////////////////////////////////////////////////////////////////////////////// 
/// Author  : Xabier Anduaga, Martin Tripiana
/// Created : July 2009
/// * See the Twiki for details : https://twiki.cern.ch/twiki/bin/view/Main/TriggerHandlerForPhotonAnalysisUtils
////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "egammaEvent/egamma.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigParticle/TrigPhoton.h"
#include "TrigParticle/TrigPhotonContainer.h"
#include "TrigParticle/TrigElectron.h"
#include "TrigParticle/TrigElectronContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "CaloEvent/CaloCluster.h"
#include "TrigObjectMatching/TrigMatchTool.h"
#include "PhotonAnalysisUtils/PAUtrigRecoMatch.h"

#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"

#include <iostream>
using namespace TrigMatch;

PAUtrigRecoMatch::PAUtrigRecoMatch(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_matchTool("TrigMatchTool/TrigMatchTool")
{
  // job option configurable properties
  declareInterface<IPAUtrigRecoMatch>(this);
  declareProperty("TrigMatchTool", m_matchTool);
  declareProperty("TrigDecisionTool", m_trigDecisionTool);
  declareProperty("TrigHLTRecoDeltaR", m_trigHLTRecoDeltaR = 0.1);
  declareProperty("TrigL1RecoDeltaR", m_trigL1RecoDeltaR = 0.15);
}

// destructor
PAUtrigRecoMatch::~PAUtrigRecoMatch()
{}

StatusCode PAUtrigRecoMatch::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "PAUtrigRecoMatch::Intialize()" << endreq;

  // load the matching tool
  if( !m_matchTool.empty() ) {
    StatusCode sc = m_matchTool.retrieve();
    if( !sc.isSuccess() ) {
      log << MSG::FATAL << "Unable to retrieve the TrigMatchTool!" << endreq;
      log << MSG::FATAL << "Please check your job options file" << endreq;
      return sc;
    }
    log << MSG::INFO << "Successfully retrieved the TrigMatchTool!" << endreq;
  }
  else {
    log << MSG::FATAL << "Could not retrieve the TrigMatchTool as it was not specified!" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode PAUtrigRecoMatch::finalize() {
  return StatusCode::SUCCESS;
}



unsigned int PAUtrigRecoMatch::causedTrigger(const EmTau_ROI* bestL1, const std::vector<std::string> chainNames){
  unsigned int failed = 0;
  int i = 0;
  
  std::vector<std::string>::const_iterator ItCn = chainNames.begin();
  for(; ItCn !=  chainNames.end(); ++ItCn,i++){
    bool PassHLT =   m_matchTool->chainPassedByObject<TrigPhotonL1, EmTau_ROI>(bestL1, *ItCn, 0.01);
    if(!PassHLT) failed |= (1<<i);
  }
  return failed;
}




unsigned int PAUtrigRecoMatch::causedTrigger(const TrigEMCluster* bestL2, const std::vector<std::string> chainNames){
  unsigned int failed = 0;
  int i = 0;
  
  std::vector<std::string>::const_iterator ItCn = chainNames.begin();
  for(; ItCn !=  chainNames.end(); ++ItCn,i++){
    bool PassHLT =  m_matchTool->chainPassedByObject<TrigPhotonL2, TrigEMCluster>(bestL2, *ItCn, 0.01);
    if(!PassHLT) failed |= (1<<i);
  }
  return failed;
}


unsigned int PAUtrigRecoMatch::causedTrigger(const egamma* bestEF, const std::vector<std::string> chainNames){
  unsigned int failed = 0;
  int i = 0;
  
  std::vector<std::string>::const_iterator ItCn = chainNames.begin();
  for(; ItCn !=  chainNames.end(); ++ItCn,i++){
    bool PassHLT =  m_matchTool->chainPassedByObject<TrigPhotonEF, egamma>(bestEF, *ItCn, 0.01);
    if(!PassHLT) failed |= (1<<i);
  }
  return failed;
}

const EmTau_ROI* PAUtrigRecoMatch::getBestL1match(const PAU::egamma* m){
  MsgStream log(msgSvc(), name());
  std::vector<const TrigPhotonL1*> bestL1 = m_matchTool->matchToTriggerObjects<TrigPhotonL1, CaloCluster>(m->cluster(), "L1_EM[0-9]+",m_trigL1RecoDeltaR,false);//"L2_[0-9]?g[0-9]+");        
  if(bestL1.size()>0){
    return bestL1.at(0);
  }
  return NULL;
}


const TrigEMCluster* PAUtrigRecoMatch::getBestL2match(const PAU::egamma* m){
  MsgStream log(msgSvc(), name());
  std::vector<const TrigPhotonL2*> bestL2 = m_matchTool->matchToTriggerObjects<TrigPhotonL2, CaloCluster>(m->cluster(), "L2_g.*",m_trigHLTRecoDeltaR,false);//"L2_[0-9]?g[0-9]+");            
  if(bestL2.size()>0){
    return bestL2.at(0)->cluster();
  }
  return NULL;
}

const egamma* PAUtrigRecoMatch::getBestEFmatch(const PAU::egamma* m){
  std::vector<const TrigPhotonEF*> bestCont = m_matchTool->matchToTriggerObjects<TrigPhotonEF, PAU::egamma>(m, "EF_g.*",m_trigHLTRecoDeltaR,false);
  if(bestCont.size()>0){
    return bestCont.at(0);
  }
  return NULL;
}


//Electrons
unsigned int PAUtrigRecoMatch::causedTrigger(const TrigElectron* bestL2, const std::vector<std::string> chainNames){
  unsigned int failed = 0;
  int i = 0;
  
  std::vector<std::string>::const_iterator ItCn = chainNames.begin();
  for(; ItCn !=  chainNames.end(); ++ItCn,i++){
    bool PassHLT =  m_matchTool->chainPassedByObject<TrigElectronL2, TrigElectron>(bestL2, *ItCn, 0.01);
    if(!PassHLT) failed |= (1<<i);
  }
  return failed;
}


const EmTau_ROI* PAUtrigRecoMatch::getBestL1match(const Analysis::Electron* m){
  MsgStream log(msgSvc(), name());
  std::vector<const TrigElectronL1*> bestL1 = m_matchTool->matchToTriggerObjects<TrigElectronL1, CaloCluster>(m->cluster(), "L1_EM[0-9]+",m_trigL1RecoDeltaR,false);//"L2_[0-9]?g[0-9]+");        
  if(bestL1.size()>0){
    return bestL1.at(0);
  }
  return NULL;
}

//const TrigEMCluster* PAUtrigRecoMatch::getBestL2match(const Analysis::Electron* m){
const TrigElectron* PAUtrigRecoMatch::getBestL2match(const Analysis::Electron* m){
  MsgStream log(msgSvc(), name());
  std::vector<const TrigElectronL2*> bestL2 = m_matchTool->matchToTriggerObjects<TrigElectron, Analysis::Electron>(m, "L2_e.*",m_trigHLTRecoDeltaR,false);//"L2_[0-9]?g[0-9]+");            
  //std::vector<const TrigEMCluster*> bestL2 = m_matchTool->matchToTriggerObjects<TrigEMCluster, CaloCluster>(m->electron()->cluster(), "L2_e.*",m_trigHLTRecoDeltaR,false);//"L2_[0-9]?g[0-9]+");            
  if(bestL2.size()>0){
    //return bestL2.at(0)->cluster();
    return bestL2.at(0);
  }
  return NULL;
}

const TrigEMCluster* PAUtrigRecoMatch::getBestL2match_ph(const Analysis::Electron* m){
  MsgStream log(msgSvc(), name());
  std::vector<const TrigPhotonL2*> bestL2 = m_matchTool->matchToTriggerObjects<TrigPhotonL2, CaloCluster>(m->cluster(), "L2_g.*",m_trigHLTRecoDeltaR,false);//"L2_[0-9]?g[0-9]+");            
  if(bestL2.size()>0){
    return bestL2.at(0)->cluster();
  }
  return NULL;
}


const egamma* PAUtrigRecoMatch::getBestEFmatch(const Analysis::Electron* m){
  std::vector<const TrigElectronEF*> bestCont = m_matchTool->matchToTriggerObjects<TrigElectronEF,Analysis::Electron>(m, "EF_e.*",m_trigHLTRecoDeltaR,false);
  if(bestCont.size()>0){
    return bestCont.at(0);
  }
  return NULL;
}

const egamma* PAUtrigRecoMatch::getBestEFmatch_ph(const Analysis::Electron* m){
  std::vector<const TrigPhotonEF*> bestCont = m_matchTool->matchToTriggerObjects<TrigPhotonEF,CaloCluster>(m->cluster(), "EF_g.*",m_trigHLTRecoDeltaR,false);
  if(bestCont.size()>0){
    return bestCont.at(0);
  }
  return NULL;
}


