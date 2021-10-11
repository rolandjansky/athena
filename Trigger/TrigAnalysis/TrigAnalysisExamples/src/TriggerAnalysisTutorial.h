// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
#include "TriggerMatchingTool/IMatchingTool.h"
#include "TriggerAnalysisHelper.h"

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
 * Updated August 2016 for ATLAS Trigger for Physics Workshop
 * Some basic trigger analysis code -- see other examples
 * Can we make one common algorithm to put everything together?
 * @author Ryan White <rwhite@cern.ch> - UTFSM
 */
class TriggerAnalysisTutorial : public AthAlgorithm {

public:
   /// Regular algorithm constructor
   TriggerAnalysisTutorial( const std::string& name, ISvcLocator *pSvcLocator );

   ~TriggerAnalysisTutorial();

   /// Function called at the beginning of the job
   virtual StatusCode initialize();
   /// Function called at the end of the job
   virtual StatusCode finalize();
   /// Function called at each event
   virtual StatusCode execute();

private:
   int m_eventNr;
   //int m_eventsPrinted;
   
   std::vector< std::string > m_chain_names;
   std::vector< std::string > m_cfg_chains;
   
   // histograms
   TH1 *m_h_triggerAccepts;
   // TrigDecisionTool
   ToolHandle< Trig::TrigDecisionTool > m_trigDec;

   // TrigMatchTool handle
   ToolHandle< Trig::IMatchingTool > m_matchTool;

   //! Helper class for tutorial, provides an additional layer to illustrate TDT functionality
   ToolHandle< Trig::TriggerAnalysisHelper > m_tah; 
   
   // The THistSvc
   ServiceHandle< ITHistSvc > m_histSvc;

}; // end of class TriggerAnalysisTutorial

#endif // TRIGANALYSISEXAMPLES_TRIGGERANALYSISTUTORIAL_H
