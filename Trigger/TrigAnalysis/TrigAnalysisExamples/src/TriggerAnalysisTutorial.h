// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerAnalysisTutorial.h 297842 2010-05-07 13:47:24Z krasznaa $
#ifndef TRIGANALYSISEXAMPLES_TRIGGERANALYSISTUTORIAL_H
#define TRIGANALYSISEXAMPLES_TRIGGERANALYSISTUTORIAL_H

// STL include(s):
#include <string>
#include <map>
#include <vector>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// Trigger include(s):
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigObjectMatching/TrigMatchTool.h"

// Forward declaration(s):
class TTree;
class TH1;

/**
 * @brief The TriggerAnalysisTutorial goes together with the trigger
 * part of the ATLAS Software Tutorial
 *
 * @author Joerg Stelzer <stelzer@cern.ch> - DESY
 * @author Ricardo Goncalo <goncalo@cern.ch> - University 
 *
 */
class TriggerAnalysisTutorial : public AthAlgorithm {

public:
   /// Regular algorithm constructor
   TriggerAnalysisTutorial( const std::string& name, ISvcLocator *pSvcLocator );

   /// Function called at the beginning of the job
   virtual StatusCode initialize();
   /// Function called at the end of the job
   virtual StatusCode finalize();
   /// Function called at each event
   virtual StatusCode execute();

private:
   // private functions for structuring the tutorial
   StatusCode printTriggerChainsAllLevels();
   StatusCode printPrescales();
   StatusCode printSimplePassInfo();
   StatusCode collectTriggerStatistics();
   StatusCode jetTriggerStudy(const std::string& chain);
   StatusCode zeeTriggerAnalysis(const std::string& chain);

   // private data

   // job properties
   std::string m_jetTriggerChain;   
   float m_zeeMaxMatchDistance;
   std::string m_zeeTriggerChain;

   // event info
   unsigned int m_eventNr;
   unsigned int m_eventsPrinted;
   unsigned int m_runNr;
   unsigned int m_lbNr;

   // histogram
   TH1* m_triggerAccepts;

   // tree and variables to fill
   TTree* m_tree;

   // variables for trigger jet study
   float m_efJet_et;
   float m_efJet_eta;
   float m_efJet_phi;
   float m_l2Jet_et;
   float m_l2Jet_eta;
   float m_l2Jet_phi;
   float m_l1Jet_et88;
   float m_l1Jet_eta;
   float m_l1Jet_phi;
   float m_l1roi_eta;
   float m_l1roi_phi;

   // variables for Z->ee study
   std::vector<float>* m_MZ;
   std::vector<float>* m_pE1;
   std::vector<float>* m_pE2;
   std::vector<float>* m_matchDistance;
   std::vector<float>* m_pEOther;
   std::vector<int>*   m_matchOther;

   std::vector<std::string> m_chain_names;
   std::vector<std::string> m_triggerChains;
   std::map<std::string,int> m_triggersPassed;

   // TrigDecisionTool
   ToolHandle< Trig::TrigDecisionTool > m_trigDec;

   // TrigMatchTool handle
   ToolHandle< TrigMatchTool > m_matchTool;

   // The THistSvc
   ServiceHandle< ITHistSvc > m_histSvc;

}; // end of class TriggerAnalysisTutorial

#endif // TRIGANALYSISEXAMPLES_TRIGGERANALYSISTUTORIAL_H
