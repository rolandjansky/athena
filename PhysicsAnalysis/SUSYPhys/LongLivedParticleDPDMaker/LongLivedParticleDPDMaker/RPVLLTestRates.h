/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPVLL_TESTRATES_H
#define RPVLL_TESTRATES_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : RPVLLTestRates.h
/// Package : offline/PhysicsAnalysis/SUSYPhys/RPVLLTestRates

///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TH1.h"
#include "TH2.h"
#include "TTree.h"


#include <string>
 
class RPVLLTestRates : public AthAlgorithm  {

 public:

   RPVLLTestRates(const std::string& name, ISvcLocator* pSvcLocator);
   ~RPVLLTestRates();


   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual StatusCode execute();

 private:

   int m_EventCounter;
   ServiceHandle<ITHistSvc> m_tHistSvc;

   std::vector<std::string> m_DecisionLabel;
   std::vector<int>         m_EventNumber;
   std::vector<double>      m_WeightedEventNumber;
   TTree* m_myTree;
   TH1F* m_skimPassHist;
   TH2F* m_twoDskimHist;

   int m_runNum;
   int m_lumiBlock;
   int m_evtNum;
   std::vector<int>         m_filterPassed;   
};

#endif 

