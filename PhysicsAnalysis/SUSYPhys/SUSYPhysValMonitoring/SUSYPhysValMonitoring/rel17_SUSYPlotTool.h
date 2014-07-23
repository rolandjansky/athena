/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************
SUSYPlotTool.h: Base tool to make plots from EventView. It provides
	- pointers to StoreGate and AnalysisTools
	- pointers to EventView reco and truth (the latter may be null)
	- #include's relevant to these
	- a virtual takeAction() method which does the actual work.
All tools called by SUSYPlot should inherit from this base class and must 
define takeAction().

Modified from SusyPlot.
***********************************************************************/
#ifndef SUSYPLOT_SUSYPLOTTOOL_H
#define SUSYPLOT_SUSYPLOTTOOL_H

// Any generally useful include files should be added here
#include "SUSYPhysValMonitoring/rel17_ISUSYPlotTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AnalysisTools/IAnalysisTools.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include <GaudiKernel/ITHistSvc.h>
#include <GaudiKernel/AlgTool.h>
#include <GaudiKernel/MsgStream.h>
#include <CLHEP/Units/SystemOfUnits.h>

using namespace CLHEP;

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include <vector>
#include <map>
#include <string>

class SUSYPlotTool : virtual public ISUSYPlotTool, public AthAlgTool {

 public:
  SUSYPlotTool(const std::string& type,
             const std::string& name,
             const IInterface* parent);
  virtual ~SUSYPlotTool();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode doIt(bool, bool, ToolHandle<Trig::TrigDecisionTool>);
  virtual StatusCode takeAction()=0;

 protected:
  // common variables to all the tools:
  IAnalysisTools* m_pPAT;
  ITHistSvc*      m_thistSvc;
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;
  bool m_doTrigger;
  bool m_doTruth;

  std::string m_foldername;
  std::string m_prefix;

  int m_runNumber;
  int m_eventNumber;
  double m_eventWeight;

  bool m_trigger;

 private:
  std::string m_McEventName;

};

#endif
