// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEmulatorExAlg.h 770492 2016-08-28 16:52:40Z rwhite $
#ifndef TRIGANALYSISEXAMPLES_TRIGEMULATOR_EX_ALG_H
#define TRIGANALYSISEXAMPLES_TRIGEMULATOR_EX_ALG_H

// STL include(s):
#include <string>

// Root
#include "TProfile.h"
#include "TH1.h"
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
 * Example code for emulating the trigger decision 
 * from reconstructed objects, e.g. L1 Objects
 *        
 *
 * @author Ryan White <ryan.white@cern.ch>
 *
 * Please see the .cxx file for detailed comments
 *
 */
class TrigEmulatorExAlg : public AthAlgorithm {

public:
   /// Regular algorithm constructor
   TrigEmulatorExAlg( const std::string& name, ISvcLocator *pSvcLocator );

   /// Function called at the beginning of the job
   virtual StatusCode initialize();
   /// Function called at each event
   virtual StatusCode execute();
   /// Function called at the end of the job
   virtual StatusCode finalize();

private:
   // event info
   unsigned int m_eventNr;
   //bool m_doEg;
   
   // job options configurable
   std::vector<std::string> m_l1chainList; // L1 chain of interest, e.g., L1_MU10
   std::vector<std::string> m_hltchainList; // HLT chains of interest, e.g., HLT_mu10

   // Methods to steer the tutorial
   StatusCode collectTriggerStatistics();
   StatusCode EmulationAnalysis();
   
   // Tools
   ToolHandle< Trig::TrigDecisionTool > m_triggerDecisionTool;
   ToolHandle<Trig::IMatchingTool> m_triggerMatchingTool;
   
   //! Helper class for tutorial, provides an additional layer to illustrate TDT functionality
   ToolHandle< Trig::TriggerAnalysisHelper > m_triggerAnalysisHelper; 
   
   // The THistSvc
   ServiceHandle< ITHistSvc > m_histSvc;
   
   // private data
   std::map<std::string,int> m_numSelectedEvents; // events passing selection requirements
   std::map<std::string,int> m_numL1PassedEvents; // events passing L1
   std::map<std::string,int> m_numHLTPassedEvents; // events passing HLT 
   std::map<std::string,int> m_numL1EmulatedEvents; // events emulated at L1 
   std::map<std::string,int> m_numHLTEmulatedEvents; // events emualted at HLT 

   // Histograms common for each analysis

   // Trigger Decision
   TH1* m_h_L1TriggerAccepts;
   TH1* m_h_L1EmulationAccepts;
   TH1* m_h_HLTTriggerAccepts;
   TH1* m_h_HLTEmulationAccepts;

   // Internal settings for isolation and leakage methods
   double m_roi_pt_cut;
   double m_roi_eta_cut;

   double m_iso_slope;
   double m_iso_offset;
   double m_iso_thresh;
   double m_iso_min;

   double m_had_leak_slope;
   double m_had_leak_offset;
   double m_had_leak_thresh;
   double m_had_leak_min;

   bool m_use_had_core;
   bool m_use_emclus;

   bool EmulateTauDecision (const xAOD::EmTauRoI& l1tau) const;
   bool pass_isolation(const xAOD::EmTauRoI& l1tau) const;
   bool pass_hadronic_leakage(const xAOD::EmTauRoI& l1tau) const;

}; // end of class TrigEmulatorExAlg

#endif // TRIGANALYSISEXAMPLES_TRIGEMULATOR_EX_ALG_H
