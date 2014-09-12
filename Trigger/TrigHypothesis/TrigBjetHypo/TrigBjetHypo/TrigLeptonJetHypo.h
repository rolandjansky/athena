/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C++  
// :-)
/**************************************************************************
 **
 **   FILE NAME:   TrigLeptonJetHypo.h
 **   PACKAGE:     Trigger/TrigHypothesis/TrigBjetHypo
 **
 **   DESCRIPTION: Hyphotesis algorithm header file
 **            
 **
 **   AUTHOR: Cristiano Alpigiani 
 **   EMAIL:  Cristiano.Alpigiani@ge.infn.it
 **
 **   CREATED:  April, 2008
 **   MODIFIED: September, 2009 (Andrea Coccaro)
 **              
 **************************************************************************/ 

#ifndef TRIGLEPTONJETHYPO_H
#define TRIGLEPTONJETHYPO_H

#include "TrigInterfaces/HypoAlgo.h"


class TrigTimerSvc;
class TriggerElement;



class TrigLeptonJetHypo : public HLT::HypoAlgo {

 public:

  /** @brief Constructor. */
  TrigLeptonJetHypo(const std::string&, ISvcLocator*);
  /** @brief Destructor. */
  ~TrigLeptonJetHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement*, bool&);
  HLT::ErrorCode hltFinalize(); 

 private:

  /** @brief Cut counter. */
  int m_cutCounter;

  /** @brief DeclareProperty: if acceptAll flag is set to true, every event is taken. */ 
  bool m_acceptAll;
  /** @brief DeclareProperty: maximum value to have a successful matching between the mu and the jet. */
  double m_deltaRCut;
  /** @brief DeclareProperty: string corresponding to the trigger level in which the algorithm is running. */
  std::string m_instance;
  /** @brief DeclareProperty: if LVLrefining is set to true (false), muComb (LVL1) output is considered to get mu direction. */
  bool m_LVL2refining;

  /** @brief Total execution time of TrigLeptonJetHypo class. */
  TrigTimer *m_totTimer;
  
};

#endif

