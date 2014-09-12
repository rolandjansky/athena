/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C++  
// :-)
/**************************************************************************
 **
 **   FILE NAME:   TrigLeptonJetFex.h
 **   PACKAGE:     Trigger/TrigHypothesis/TrigBjetHypo
 **
 **   DESCRIPTION: Features Extractor algorithm header file
 **            
 **
 **   AUTHOR: Cristiano Alpigiani 
 **   EMAIL:  Cristiano.Alpigiani@ge.infn.it
 **
 **   CREATED:  April, 2008
 **   MODIFIED: September, 2009 (Andrea Coccaro)
 **              
 **************************************************************************/ 

#ifndef TRIGLEPTONJETFEX_H
#define TRIGLEPTONJETFEX_H

#include "TrigInterfaces/FexAlgo.h"
#include "TrigInterfaces/ComboAlgo.h"
#include "TrigInterfaces/AlgoConfig.h"


class TriggerElement;



class TrigLeptonJetFex: public HLT::ComboAlgo{

 public:

  /** @brief Constructor. */
  TrigLeptonJetFex(const std::string& name, ISvcLocator* pSvcLocator);
  /** @brief Destructor. */
  ~TrigLeptonJetFex();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);

 private:

  /** @brief DeclareProperty: string corresponding to the trigger level in which the algorithm is running. */  
  std::string m_instance;

};

#endif


