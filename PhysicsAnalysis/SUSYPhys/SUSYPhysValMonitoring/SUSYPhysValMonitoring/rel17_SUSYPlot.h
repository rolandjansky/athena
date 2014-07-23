/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************
SUSYPlot.h: Algorithm to make plots from EventView. It calls
	- a selector tool
	- a list of histogramming/analysis tools
all inheriting from SUSYPlotTool. All histograms from an instance of
SUSYPlot are saved in a common Root folder with the instance name.

Modified from SusyPlot.
***********************************************************************/

#ifndef SUSYPLOT_SUSYPLOT_H
#define SUSYPLOT_SUSYPLOT_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include <string>

/** Forward declarations */

class ISUSYPlotTool;

class SUSYPlot : public AthAlgorithm {
 public:
  SUSYPlot(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SUSYPlot();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 ToolHandle<Trig::TrigDecisionTool> trigDecTool() {return m_trigDecAlg;}
  bool doTrigger() {return m_doTrigAlg;}
 private:

 protected:
  std::string m_collectionKey;
  std::string m_eventTruthKey;
  // std::string m_selectToolKey;
  //std::vector<std::string> m_histToolKeys;
  //  ISUSYPlotTool* m_selectTool;
  // std::vector<ISUSYPlotTool*> m_histTools;

  ToolHandle<Trig::TrigDecisionTool> m_trigDecAlg;
  
  ToolHandleArray< ISUSYPlotTool > m_histTools;
  ToolHandleArray< ISUSYPlotTool > m_selectTools;

  bool m_doTrigAlg;
  bool m_doTruthAlg;
};

#endif
