/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/TrigMinBiasNtupleAlg.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigMinBiasNtuple/BcmLvl1NtComponent.h"
#include "TrigMinBiasNtuple/BcmRdoNtComponent.h"
#include "TrigMinBiasNtuple/CaloCellNtComponent.h"
#include "TrigMinBiasNtuple/CtpDecisionNtComponent.h"
#include "TrigMinBiasNtuple/CtpRdoNtComponent.h"
#include "TrigMinBiasNtuple/EventInfoNtComponent.h"
#include "TrigMinBiasNtuple/TriggerDecisionNtComponent.h"
#include "TrigMinBiasNtuple/LucidLvl1NtComponent.h"
#include "TrigMinBiasNtuple/Lvl1ConfigNtComponent.h"
#include "TrigMinBiasNtuple/MbtsContainerNtComponent.h"
#include "TrigMinBiasNtuple/MbtsLvl1NtComponent.h"
#include "TrigMinBiasNtuple/MbtsLvl2NtComponent.h"
#include "TrigMinBiasNtuple/McTruthNtComponent.h"
#include "TrigMinBiasNtuple/MuonNtComponent.h"
#include "TrigMinBiasNtuple/PixelClusNtComponent.h"
#include "TrigMinBiasNtuple/SctClusNtComponent.h"
#include "TrigMinBiasNtuple/PixelSpNtComponent.h"
#include "TrigMinBiasNtuple/SctSpNtComponent.h"
#include "TrigMinBiasNtuple/SpLvl2NtComponent.h"
#include "TrigMinBiasNtuple/TrackNtComponent.h"
#include "TrigMinBiasNtuple/TrkCntsEfNtComponent.h"
#include "TrigMinBiasNtuple/VxCntsEfNtComponent.h"
#include "TrigMinBiasNtuple/TrtLvl2NtComponent.h"
#include "TrigMinBiasNtuple/TrtRawNtComponent.h"
#include "TrigMinBiasNtuple/TruthLucidLvl1NtComponent.h"
#include "TrigMinBiasNtuple/RawLucidNtComponent.h"
#include "TrigMinBiasNtuple/TruthMbtsLvl1NtComponent.h"
#include "TrigMinBiasNtuple/ZdcDigitsNtComponent.h"
#include "TrigMinBiasNtuple/ZdcLvl1NtComponent.h"
#include "TrigMinBiasNtuple/TrackletNtComponent.h"

// Default names for Lvl1 simulation objects
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

//---------------------------------------------------------------------

TrigMinBiasNtupleAlg::TrigMinBiasNtupleAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  NtupleAlgorithm(name, pSvcLocator) {

  // Ntuple properties
  declareProperty("ntupleFile", m_ntupleFile = "trigmb");
  declareProperty("ntupleDir", m_ntupleDir = "MinBias");
  declareProperty("ntupleTreeName", m_ntupleTreeName = "TrigMinBias");
  declareProperty("ntupleTreeTitle", m_ntupleTreeTitle = "Minbias Trigger Validation");
  declareProperty("ntupleComponents", m_ntupleComponentCommands, "A definition ntuple components and their parameters");

  // Setup the list of available NtComponent classes compiled with this algorithm
  const int num = 30;
  std::string availableComponents[num] = {"BcmLvl1",
                                         "BcmRdo",
                                         "CaloCell",
                                         "CtpDecision",
                                         "CtpRdo",
                                         "EventInfo",
					 "TriggerDecision",
                                         "LucidLvl1",
                                         "Lvl1Config",
                                         "MbtsContainer",
                                         "MbtsLvl1",
                                         "MbtsLvl2",
                                         "McTruth",
                                         "Muon",
                                         "PixelClus",
                                         "SctClus",
					 "PixelSp",
                                         "SctSp",
                                         "SpLvl2",
                                         "Track",
                                         "TrkCntsEf",
                                         "VxCntsEf",
					 "TrtLvl2",
                                         "TrtRaw",
                                         "TruthLucidLvl1",
					 "RawLucid",
                                         "TruthMbtsLvl1",
					 "ZdcDigits",
                                         "ZdcLvl1",
					 "Tracklet"};
  std::vector<std::string> availableComponentsVec(availableComponents, availableComponents+num);
  m_availableComponents = availableComponentsVec;
}

//---------------------------------------------------------------------


StatusCode TrigMinBiasNtupleAlg::initialize(){

  msg(MSG::INFO) << "entering intialize()" << endreq;
  
  if(NtupleAlgorithm::parseNtupleComponentStrings().isFailure()) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't initialize NtupleAlgorithm base class." << endreq;
    return StatusCode::FAILURE;
  }

  // Loop over the NtComponentParameters and fill the vector with
  // requested ntuple components.
  std::vector<NtComponent::NtComponentParameters>::iterator para_itr;
  std::vector<NtComponent::NtComponentParameters>::iterator para_itr_end;
  para_itr = m_ntupleComponentParameters.begin();
  para_itr_end = m_ntupleComponentParameters.end();
  for(;para_itr!=para_itr_end;para_itr++) {
    // special component only filled for runs 
    if((*para_itr).name == "Lvl1Config") m_ntupleRunComponents.push_back(new NtComponent::Lvl1ConfigNtComponent(this,(*para_itr)));
    // the rest of the components are for every event
    else if((*para_itr).name == "BcmLvl1") m_ntupleComponents.push_back(new NtComponent::BcmLvl1NtComponent(this,(*para_itr)));
    else if((*para_itr).name == "BcmRdo") m_ntupleComponents.push_back(new NtComponent::BcmRdoNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "CaloCell") m_ntupleComponents.push_back(new NtComponent::CaloCellNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "CtpDecision") m_ntupleComponents.push_back(new NtComponent::CtpDecisionNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "CtpRdo") m_ntupleComponents.push_back(new NtComponent::CtpRdoNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "EventInfo") m_ntupleComponents.push_back(new NtComponent::EventInfoNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "TriggerDecision") m_ntupleComponents.push_back(new NtComponent::TriggerDecisionNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "MbtsContainer") m_ntupleComponents.push_back(new NtComponent::MbtsContainerNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "MbtsLvl1") m_ntupleComponents.push_back(new NtComponent::MbtsLvl1NtComponent(this,(*para_itr)));
    else if((*para_itr).name == "MbtsLvl2") m_ntupleComponents.push_back(new NtComponent::MbtsLvl2NtComponent(this,(*para_itr)));
    else if((*para_itr).name == "McTruth") m_ntupleComponents.push_back(new NtComponent::McTruthNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "Muon") m_ntupleComponents.push_back(new NtComponent::MuonNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "PixelClus") m_ntupleComponents.push_back(new NtComponent::PixelClusNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "SctClus") m_ntupleComponents.push_back(new NtComponent::SctClusNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "PixelSp") m_ntupleComponents.push_back(new NtComponent::PixelSpNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "SctSp") m_ntupleComponents.push_back(new NtComponent::SctSpNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "SpLvl2") m_ntupleComponents.push_back(new NtComponent::SpLvl2NtComponent(this,(*para_itr)));
    else if((*para_itr).name == "Track") m_ntupleComponents.push_back(new NtComponent::TrackNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "TrkCntsEf") m_ntupleComponents.push_back(new NtComponent::TrkCntsEfNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "VxCntsEf") m_ntupleComponents.push_back(new NtComponent::VxCntsEfNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "TrtLvl2") m_ntupleComponents.push_back(new NtComponent::TrtLvl2NtComponent(this,(*para_itr)));
    else if((*para_itr).name == "TrtRaw") m_ntupleComponents.push_back(new NtComponent::TrtRawNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "TruthLucidLvl1") m_ntupleComponents.push_back(new NtComponent::TruthLucidLvl1NtComponent(this,(*para_itr)));
    else if((*para_itr).name == "RawLucid") m_ntupleComponents.push_back(new NtComponent::RawLucidNtComponent(this,(*para_itr)));
    else if((*para_itr).name == "TruthMbtsLvl1") m_ntupleComponents.push_back(new NtComponent::TruthMbtsLvl1NtComponent(this,(*para_itr)));
    else if((*para_itr).name == "ZdcDigits") m_ntupleComponents.push_back(new NtComponent::ZdcDigitsNtComponent(this,(*para_itr)));  
    else if((*para_itr).name == "ZdcLvl1") m_ntupleComponents.push_back(new NtComponent::ZdcLvl1NtComponent(this,(*para_itr)));   
    else  if((*para_itr).name == "Tracklet") m_ntupleComponents.push_back(new NtComponent::TrackletNtComponent(this,(*para_itr)));
    else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "The NtComponent " << (*para_itr).name << " has not been implemented." << endreq;
  }

  // Initialize base class.
  if(NtupleAlgorithm::initialize().isFailure()) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't initialize NtupleAlgorithm base class." << endreq;
    return StatusCode::FAILURE;
  }  

  msg(MSG::INFO) << "leaving intialize()" << endreq;
  
  return StatusCode::SUCCESS;
}
 
//---------------------------------------------------------------------

 
