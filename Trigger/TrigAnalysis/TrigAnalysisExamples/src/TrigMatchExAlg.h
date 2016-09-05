// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMatchExAlg.h 770492 2016-08-28 16:52:40Z rwhite $
#ifndef TRIGANALYSISEXAMPLES_TRIG_MATCH_EX_ALG_H
#define TRIGANALYSISEXAMPLES_TRIG_MATCH_EX_ALG_H

// STL include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"


/**
 * @brief The TrigMatchExAlg is an example of how to use the
 *        TriggerMatchingTool
 *
 *        Please see TrigMatchExAlg.cxx for detailed comments
 *
 * @author Ben Smith <bcsmith@fas.harvard.edu> - Harvard University
 *
 */
class TrigMatchExAlg : public AthAlgorithm {

public:
   /// Regular algorithm constructor
   TrigMatchExAlg( const std::string& name, ISvcLocator *pSvcLocator );

   /// Function called at the beginning of the job
   virtual StatusCode initialize();
   /// Function called at each event
   virtual StatusCode execute();

private:
   // job options configurable
   std::string m_muonContainerKey;
   std::string m_electronContainerKey;
   std::string m_tauContainerKey;
   std::string m_jetContainerKey;
   std::string m_photonContainerKey;

   // user flags
   bool m_matchMuons;
   bool m_matchElectrons;
   bool m_matchPhotons;
   bool m_matchTaus;
   bool m_matchJets;
   bool m_matchClusterElectrons;
   bool m_matchL1;
   bool m_matchL2;
   bool m_matchEF;

   // needed tools
   ToolHandle< Trig::IMatchingTool > m_matchTool;

}; // end of class TrigMatchExAlg

#endif // TRIGANALYSISEXAMPLES_TRIG_MATCH_EX_ALG_H
