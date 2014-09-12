/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C++  
// :-)
/**************************************************************************
 **
 **   FILE NAME:   TrigLeptonJetFex.cxx
 **   PACKAGE:     Trigger/TrigHypothesis/TrigBjetHypo
 **
 **   DESCRIPTION: Features Extractor algorithm developped for the 
 **                muon-jet selection 
 **            
 **
 **   AUTHOR: Cristiano Alpigiani 
 **   EMAIL:  Cristiano.Alpigiani@ge.infn.it
 **           
 **
 **   CREATED:  April, 2008
 **   MODIFIED: September, 2009 (Andrea Coccaro)
 **              
 **************************************************************************/ 

#include "TrigBjetHypo/TrigLeptonJetFex.h"



//*********----------------------------------------------------------------------------------*********//


TrigLeptonJetFex::TrigLeptonJetFex(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::ComboAlgo(name, pSvcLocator)
{
  declareProperty("Instance", m_instance);
}


//*********----------------------------------------------------------------------------------*********//


TrigLeptonJetFex::~TrigLeptonJetFex() {}


//*********----------------------------------------------------------------------------------*********//


HLT::ErrorCode TrigLeptonJetFex::hltInitialize() {

  if (m_instance == "L2"){
    if (msgLvl() <= MSG::INFO)
      msg() << MSG::INFO << "TrigLeptonJetFex initialization at L2" << endreq;

    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Instance = " << m_instance << endreq;
  }  

  if (m_instance == "EF"){
    if (msgLvl() <= MSG::INFO)
      msg() << MSG::INFO << "TrigLeptonJetFex initialization at EF" << endreq;

    if(msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << " Instance = " << m_instance << endreq;
  }

  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " Initialization completed successfully" << endreq;
  
  return HLT::OK;
}


//*********----------------------------------------------------------------------------------*********//


HLT::ErrorCode TrigLeptonJetFex::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement*) {
  
  if (m_instance == "L2") {
    
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "TrigLeptonJetFex at L2" << endreq;
    
    if (inputTE.size() != 2) {
      msg() << MSG::ERROR << "Number of input TEs is " <<  inputTE.size() << " and not 2. Configuration problem." << endreq;
      
      return HLT::BAD_JOB_SETUP;
    }
  }
            
  if (m_instance == "EF") {
    
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "TrigLeptonJetFex at EF" << endreq;
  }
  
  return HLT::OK;  
}


//*********----------------------------------------------------------------------------------*********//


HLT::ErrorCode TrigLeptonJetFex::hltFinalize() {

  msg() << MSG::INFO << "Finalizing TrigLeptonJetFex" << endreq;

  return HLT::OK;
}


//*********----------------------------------------------------------------------------------*********//
