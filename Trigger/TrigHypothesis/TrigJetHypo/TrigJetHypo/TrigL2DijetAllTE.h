/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2DijetAllTE.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:    Simon Owen, Ignacio Aracena, Teresa Fonseca Martin, Tomasz Bold
//
// Description: Level2 Jet algorithm.  
// 
// ********************************************************************

#ifndef TRIGJETHYPO_TRIGL2DIJETALLTE_H
#define TRIGJETHYPO_TRIGL2DIJETALLTE_H

#include <string>

/** @class TrigL2DijetAllTE
    
This algorithm selects the two leading pt L2 jets in an event.
e.g. For an item like L2_2j15_j60 it stores the leading jet in
j60 and the leading jet in j15 (does not store the same jet).

@author  Simon Owen <simon.owen@shef.ac.uk>
*/

/// general athena stuff
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// some trigger stuff
#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigCaloEvent/TrigT2Jet.h"


class StoreGateSvc;


class TrigL2DijetAllTE: public HLT::AllTEAlgo {


 public:
  TrigL2DijetAllTE(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigL2DijetAllTE();

  //structure to store association information between a jet and its corresponding TE
  struct JetToTEMap {
    JetToTEMap(const TrigT2Jet * j, HLT::TriggerElement * t ) : jet(j), te(t) {}
    const TrigT2Jet * jet;
    HLT::TriggerElement * te;
    
  };
  
  
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE,
			    unsigned int outputTE);


  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();


 private:

  // Function to store the all jets and corresponding TEs to JetToTEMap.
  HLT::ErrorCode pull_jets( HLT::TEVec& tes,  std::vector<JetToTEMap>& jm);
  // Function to sort a JetToTEMap in pt
  void sort_jets( std::vector<JetToTEMap>& jm);
  //Function to print info of jets and check all jets from high pt chain also exist in low pt chain
  HLT::ErrorCode check( const std::vector<JetToTEMap>& jHigh, const std::vector<JetToTEMap>& jLow);
  //Function to store the correct TEs from the maps into a TEVec.
  HLT::ErrorCode store_jets(HLT::TEVec& outputTE, const std::vector<JetToTEMap>& jmHigh, const std::vector<JetToTEMap>& jmLow);
  
  //Monitoring variables
  bool m_doMonitoring;
  int m_nInputTEsAllTE, m_nTEinTE0, m_nTEinTE1, m_nLowChainHigh;
  double m_ptHigh, m_ptLow, m_ptHighChain_0, m_ptLowChain_0, m_ptLowChain_1;
  
  //check that algorithm is behaving as expected
  bool m_checkJetVectors;
  
  //protected:
  
  
  // public:
  
  
};


#endif
