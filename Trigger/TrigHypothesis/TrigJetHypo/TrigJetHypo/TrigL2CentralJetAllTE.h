/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2CentralJetAllTE.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
// 
// ********************************************************************

#ifndef TRIGJETHYPO_TRIGL2CENTRALJETALLTE_H
#define TRIGJETHYPO_TRIGL2CENTRALJETALLTE_H

#include <string>

/// general athena stuff
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// some trigger stuff
#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigCaloEvent/TrigT2Jet.h"

typedef std::pair<float,float> jet_pair;

class StoreGateSvc;

class TrigL2CentralJetAllTE: public HLT::AllTEAlgo {


 public:
  TrigL2CentralJetAllTE(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigL2CentralJetAllTE();

  
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE,
			    unsigned int outputTE);


  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

  bool reset();


 private:

  bool m_executedEvent_L2CentralJet ;
  
  int m_acceptedEvts;
  int m_rejectedEvts;
  int m_errorEvts;

  double eta_range ;   

  double all_jet0_et ;
  double all_jet0_eta ;
  double passed_jet0_et ;
  double passed_jet0_eta ;

  
};


#endif
