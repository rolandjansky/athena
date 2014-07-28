/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   FILE NAME:   TrigMuonJetFex.h
 **   PACKAGE:     Trigger/TrigHypothesis/TrigJetHypo
 **
 **   DESCRIPTION: Features Extractor algorithm header file
 **            
 **
 **   AUTHORS: Stefano.Giagu@cern.ch
 **            Antonio.Policicchio@cern.ch
 **
 **   CREATED:  November, 2008
 **   MODIFIED: ---  
 **              
 **************************************************************************/ 


#ifndef TRIGMUONJETFEX_H
#define TRIGMUONJETFEX_H

//* Trigger includes *//
#include "TrigInterfaces/FexAlgo.h"
#include "TrigInterfaces/ComboAlgo.h"
#include "TrigInterfaces/AlgoConfig.h"


class TriggerElement;


class TrigMuonJetFex: public HLT::ComboAlgo{

 public:

  TrigMuonJetFex(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigMuonJetFex();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);

 private:
  
  std::string m_instance;



};


#endif


