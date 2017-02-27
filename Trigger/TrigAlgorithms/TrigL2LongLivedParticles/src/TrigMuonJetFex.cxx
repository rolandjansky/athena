/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   FILE NAME:   TrigMuonJetFex.cxx
 **
 **   DESCRIPTION: Features Extractor algorithm developped for the 
 **                muon-trackless_jet selection
 **  
 **   AUTHORS: stefano.giagu@cern.ch
 **            antonio.policicchio@cern.ch 
 **              
 **************************************************************************/ 

#include "TrigL2LongLivedParticles/TrigMuonJetFex.h"


//*********----------------------------------------------------------------------------------*********//


TrigMuonJetFex::TrigMuonJetFex(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::ComboAlgo(name, pSvcLocator)
{
  declareProperty("Instance",             m_instance);
}


//*********----------------------------------------------------------------------------------*********//


TrigMuonJetFex::~TrigMuonJetFex(){ }


//*********----------------------------------------------------------------------------------*********//


HLT::ErrorCode TrigMuonJetFex::hltInitialize()
{
 
  if (m_instance == "L2"){
    if (msgLvl() <= MSG::INFO) {
      msg() << MSG::INFO << " ***** TrigL2MuonJetFex initialization on L2 ***** " << endmsg;
    }
  }  

  if (m_instance == "EF"){
    if (msgLvl() <= MSG::INFO) {
      msg() << MSG::INFO << " ***** TrigMuonJetFex initialization on EF ***** " << endmsg;
    }
  }
 
  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " Initialization completed successfully" << endmsg;
 
  return HLT::OK;
}


//*********----------------------------------------------------------------------------------*********//


HLT::ErrorCode TrigMuonJetFex::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* /*outputTE*/){

  if (m_instance == "L2"){
    
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << " ***** Executing TrigMuonJetFex on L2 ***** " << endmsg; 
 

    if (inputTE.size() != 2) {
      msg() << MSG::ERROR << "Number of input TEs is " <<  inputTE.size() << " and not 2. Configuration problem." << endmsg;
     
      return HLT::BAD_JOB_SETUP;
    }
 
  }
  
  if (m_instance == "EF"){
    
    if ( msgLvl() <= MSG::DEBUG ){
      msg() << MSG::DEBUG << " ***** Executing TrigMuonJetFex on EF ***** " << endmsg;
    }
    
  }
  
  return HLT::OK;
  
}


//*********----------------------------------------------------------------------------------*********//


HLT::ErrorCode TrigMuonJetFex::hltFinalize()
{
  msg() << MSG::INFO << " ***** Finalizing TrigMuonJetFex ***** " << endmsg;

  return HLT::OK;
}


//*********----------------------------------------------------------------------------------*********//
