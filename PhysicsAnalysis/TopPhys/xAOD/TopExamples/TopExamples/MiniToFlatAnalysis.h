/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MiniToFlatAnalysis.h 733950 2016-04-05 11:21:48Z tneep $
#ifndef TOPEXAMPLES_MINITOFLATANALYSIS_H_
#define TOPEXAMPLES_MINITOFLATANALYSIS_H_

/**
 * @author Tom Neep <tom.neep@cern.ch>
 *
 * @brief MiniToFlatAnalysis
 *
 * $Revision: 683667 $
 * $Date: 2015-07-17 04:24:59 +0200 (Fri, 17 Jul 2015) $
 *
 *
 **/

#include <memory>
#include <unordered_map>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Forward declaration(s):
namespace top {
class TopConfig;
class PlotManager;
}

class TFile;

#include "TopCPTools/TopToolStore.h"
#include "TopCorrections/ScaleFactorCalculator.h"
#include "TopEvent/TopEventMaker.h"
#include "TopAnalysis/EventSaverFlatNtuple.h"

namespace top {
class MiniToFlatAnalysis : public asg::AsgTool {
 public:
  explicit MiniToFlatAnalysis(const std::string& name);
  virtual ~MiniToFlatAnalysis() {}

  // Delete Standard constructors
  MiniToFlatAnalysis(const MiniToFlatAnalysis& rhs) = delete;
  MiniToFlatAnalysis(MiniToFlatAnalysis&& rhs) = delete;
  MiniToFlatAnalysis& operator=(const MiniToFlatAnalysis& rhs) = delete;

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
  std::shared_ptr<top::TopConfig> m_config;

  std::unique_ptr<top::TopToolStore> m_topTools;
  std::unique_ptr<top::ScaleFactorCalculator> m_sfCalc;
  std::unique_ptr<top::TopEventMaker> m_evtMaker;
  std::unique_ptr<top::EventSaverFlatNtuple> m_event_saver;

  std::string m_outputFileName;
  ::TFile* m_outputFile;
};
}  // namespace top
#endif  // TOPEXAMPLES_MINITOFLATANALYSIS_H_
