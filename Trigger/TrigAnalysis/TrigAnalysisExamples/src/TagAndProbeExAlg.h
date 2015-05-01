// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TagAndProbeExAlg.h 297842 2010-05-07 13:47:24Z krasznaa $
#ifndef TRIGANALYSISEXAMPLES_TAG_AND_PROBE_EX_ALG_H
#define TRIGANALYSISEXAMPLES_TAG_AND_PROBE_EX_ALG_H

// STL include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Trigger include(s):
#include "TrigObjectMatching/TrigMatchTool.h"

// Forward declaration(s):
namespace Analysis {
   class Muon;
}

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
   // job options configurable
   std::string m_muonContainerKey; // The StoreGate key for reco'd muons
   std::string m_l1chainName; // L1 chain of interest, e.g., L1_MU10
   std::string m_l2chainName; // L2 chain of interest, e.g., L2_mu10
   std::string m_efchainName; // EF chain of interest, e.g., EF_mu10
   float m_etaMax; // eta max for reco'd muons
   float m_ptCut; // pt cut on reco'd muons
   float m_chi2Cut; // X^2 cut on reco'd muons
   float m_massMin; // min z mass
   float m_massMax; // max z mass

   // needed tools
   ToolHandle< TrigMatchTool > m_matchTool;

   // private data
   int m_numSelectedEvents; // events passing selection requirements
   int m_numTaggedEvents;   // events passing tag requirements
   int m_numL1PassedEvents; // events with a probe muon passing L1
   int m_numL2PassedEvents; // events with a probe muon passing L2
   int m_numEFPassedEvents; // events with a probe muon passing EF

   // private functions
   bool passL1(const Analysis::Muon *m); // did the muon pass L1
   bool passL2(const Analysis::Muon *m); // did the muon pass L2
   bool passEF(const Analysis::Muon *m); // did the muon pass EF

   bool passMuon(const Analysis::Muon *m); // does the muon pass selection
   bool passEvent(std::vector<const Analysis::Muon*> &muons);
   // does the event pass selection

   bool tagMuon(const Analysis::Muon *m); // does the muon pass tag cuts
   void recordProbe(const Analysis::Muon *m); // record info about the probe

}; // end of class TagAndProbeExAlg

#endif // TRIGANALYSISEXAMPLES_TAG_AND_PROBE_EX_ALG_H
