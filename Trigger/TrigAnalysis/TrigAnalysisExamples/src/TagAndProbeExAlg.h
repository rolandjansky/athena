// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TagAndProbeExAlg.h 778537 2016-10-14 15:34:31Z ssnyder $
#ifndef TRIGANALYSISEXAMPLES_TAG_AND_PROBE_EX_ALG_H
#define TRIGANALYSISEXAMPLES_TAG_AND_PROBE_EX_ALG_H

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

// xAOD include
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
/**
 * @brief The TagAndProbeExAlg is an example of how to use
 *        the TrigMatchTool to do a tag and probe measurement with
 *        muons.
 *
 * @author Ben Smith <bcsmith@fas.harvard.edu> - Harvard University
 *
 * Please see the .cxx file for detailed comments
 *
 */
class TagAndProbeExAlg : public AthAlgorithm {

public:
   /// Regular algorithm constructor
   TagAndProbeExAlg( const std::string& name, ISvcLocator *pSvcLocator );

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
   std::string m_flavor; // Analysis type
   xAOD::Type::ObjectType m_objType; // Analysis type
   std::vector<std::string> m_l1chainList; // L1 chain of interest, e.g., L1_MU10
   std::vector<std::string> m_hltchainList; // HLT chains of interest, e.g., HLT_mu10
   
   //std::vector< std::string > m_cfg_hltchainList;
   float m_etaMax; // eta max for reco'd leptons
   float m_ptCut; // pt cut on reco'd leptons
   float m_chi2Cut; // X^2 cut on reco'd leptons
   float m_massMin; // min z mass
   float m_massMax; // max z mass
   float m_dRMax; // max dR matching threshold

   // Methods to steer the tutorial
   StatusCode collectTriggerStatistics();
   StatusCode TriggerAnalysis(const xAOD::IParticleContainer *cont);
   StatusCode EmulationAnalysis();
   // Tools
   ToolHandle< Trig::TrigDecisionTool > m_trigDec;
   ToolHandle<Trig::IMatchingTool> m_matchTool;
   //! Helper class for tutorial, provides an additional layer to illustrate TDT functionality
   ToolHandle< Trig::TriggerAnalysisHelper > m_tah; 
   // The THistSvc
   ServiceHandle< ITHistSvc > m_histSvc;
   
   // private data
   std::vector<std::pair<const xAOD::IParticle *,const xAOD::IParticle *> > m_Zcandidates;
   std::map<std::string,int> m_numSelectedEvents; // events passing selection requirements
   int m_numTaggedEvents;   // events passing tag requirements
   std::map<std::string,int> m_numL1PassedEvents; // events with a probe muon passing L1
   std::map<std::string,int> m_numHLTPassedEvents; // events with a probe muon passing EF


   // Histograms common for each analysis

   // Trigger Decision
   TH1* m_h_triggerAccepts;
   TH1* m_h_triggerAcceptsRaw;
   TH1* m_h_triggerPrescaled;
   TH1* m_h_emulationAccepts;
   
   // Kinematic plots
   //Tag and Probe analysis
   TH1* m_h_zMass;
   // Efficiency TProfiles
   std::map<std::string,TProfile*> m_h_eff_et;
   std::map<std::string,TProfile*> m_h_eff_eta;
   
   // private functions
   bool passL1(const xAOD::IParticle &recoObj, const std::string chain); // did the object match L1
   bool passHLT(const xAOD::IParticle &recoObj, const std::string chain); // did the object match HLT
   bool passHLT(const xAOD::IParticle &recoObj, std::vector<std::string> chainList); // did the object match HLT
   bool passObjectSelection(const xAOD::IParticle &trigObj);
   bool passMuon(const xAOD::Muon &muon); // does the muon pass selection
   bool passElectron( const xAOD::Electron &electron); // tag electron selection
   bool passEvent(const std::pair<const xAOD::IParticle*,const xAOD::IParticle*> ); //TP Pair Selection
   float DiobjectMass(const std::pair<const xAOD::IParticle*,const xAOD::IParticle*> ); //TP Mass 

}; // end of class TagAndProbeExAlg

#endif // TRIGANALYSISEXAMPLES_TAG_AND_PROBE_EX_ALG_H
