/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPCPTOOLS_TOPISOLATIONCPTOOLS_H_
#define TOPCPTOOLS_TOPISOLATIONCPTOOLS_H_

// Include what you use
#include <vector>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/AnaToolHandle.h"

// Isolation include(s):
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "IsolationCorrections/IIsolationCorrectionTool.h"

namespace top {

class TopConfig;

class IsolationCPTools final : public asg::AsgTool {
 public:
  explicit IsolationCPTools(const std::string& name);
  virtual ~IsolationCPTools() {}

  StatusCode initialize();

 private:
  std::shared_ptr<top::TopConfig> m_config;

  int m_release_series = 24;  // Default to 2.4

  std::string m_isolationCalibFile;

  ToolHandle<CP::IIsolationCorrectionTool> m_isolationCorr;
  ToolHandleArray<CP::IIsolationSelectionTool> m_isolationTools;

  StatusCode setupIsolation();
};
}  // namespace top

#endif  // TOPCPTOOLS_TOPISOLATIONCPTOOLS_H_
