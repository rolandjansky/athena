// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetMetExAlg.h 770492 2016-08-28 16:52:40Z rwhite $
#ifndef TRIGANALYSISEXAMPLES_JET_MET_EX_ALG_H
#define TRIGANALYSISEXAMPLES_JET_MET_EX_ALG_H

// STL include(s):
#include <string>

// Root
#include "TProfile.h"
#include "TH1.h"
#include "TEfficiency.h" // Used for turn on curves
// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// Trigger include(s):
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"
#include "TriggerAnalysisHelper.h"
/**
 * @brief 
 *        
 * @author 
 *
 * Please see the .cxx file for detailed comments
 *
 */
class JetMetExAlg : public AthAlgorithm {

public:
   /// Regular algorithm constructor
   JetMetExAlg( const std::string& name, ISvcLocator *pSvcLocator );

   /// Function called at the beginning of the job
   virtual StatusCode initialize();
   /// Function called at each event
   virtual StatusCode execute();
   /// Function called at the end of the job
   virtual StatusCode finalize();

private:
   // event info
   unsigned int m_eventNr;
   
   // job options configurable
   std::vector<std::string> m_l1chainList; // L1 chain of interest, e.g., L1_MU10
   std::vector<std::string> m_hltchainList; // HLT chains of interest, e.g., HLT_mu10
   std::vector<std::string> m_hltjetList;
   std::vector<std::string> m_hltmetList;
   float m_etaMax; // eta max for reco'd leptons
   float m_ptCut; // pt cut on reco'd leptons
   float m_dRMax; // max dR matching threshold

   // Methods to steer the tutorial
   StatusCode collectTriggerStatistics();
   StatusCode TriggerAnalysis(const xAOD::IParticleContainer *);
   StatusCode EmulationAnalysis();
   StatusCode FillEfficiency();
   // Tools
   ToolHandle< Trig::TrigDecisionTool > m_trigDec;
   ToolHandle<Trig::IMatchingTool> m_matchTool;
   //! Helper class for tutorial, provides an additional layer to illustrate TDT functionality
   ToolHandle< Trig::TriggerAnalysisHelper > m_tah; 
   // The THistSvc
   ServiceHandle< ITHistSvc > m_histSvc;
   
   // private data
   std::map<std::string,int> m_numSelectedEvents; // events passing selection requirements
   int m_numTaggedEvents;   // events passing tag requirements
   std::map<std::string,int> m_numL1PassedEvents; // events with a probe muon passing L1
   std::map<std::string,int> m_numHLTPassedEvents; // events with a probe muon passing EF


   // Histograms common for each analysis

   // Trigger Decision
   TH1* h_triggerAccepts;
   TH1* h_triggerAcceptsRaw;
   TH1* h_triggerPrescaled;
   TH1* h_emulationAccepts;
   
   // Kinematic plots
   // Efficiency TProfiles
   std::map<std::string,TProfile*> h_eff_et;
   std::map<std::string,TProfile*> h_eff_eta;

   std::map<std::string, TProfile*> h_eff_xe;
   std::map<std::string, TProfile*> h_eff_jpt;
   //std::map<std::string, TH1*> h_eff_xe_pass;
   //std::map<std::string, TH1*> h_eff_jpt_pass;
   //std::map<std::string, TH1*> h_eff_xe_total;
   //std::map<std::string, TH1*> h_eff_jpt_total;
   
   // private functions
   bool passL1(const xAOD::IParticle &recoObj, const std::string &chain); // did the object match L1
   bool passHLT(const xAOD::IParticle &recoObj, const std::string &chain); // did the object match HLT
   bool passObjectSelection(const xAOD::IParticle *);

}; // end of class JetMetExAlg

#endif // TRIGANALYSISEXAMPLES_JET_MET_EX_ALG_H
