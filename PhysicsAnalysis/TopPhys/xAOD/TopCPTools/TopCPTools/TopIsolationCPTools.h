/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

  class IsolationCPTools final: public asg::AsgTool {
  public:
    explicit IsolationCPTools(const std::string& name);
    virtual ~IsolationCPTools() {}

    StatusCode initialize();
  private:
    std::shared_ptr<top::TopConfig> m_config;

    std::string m_isolationCalibFile;

    ToolHandle<CP::IIsolationCorrectionTool> m_isolationCorr;
    ToolHandleArray<CP::IIsolationSelectionTool> m_isolationTools;

    StatusCode setupIsolation();

    /**
     * @brief Setup IsolationSelectonTools for all WPs specified for one object type (electron, muon, photon)
     *
     * @param WPs std::set of the WP names
     * @param objectWPtype what object WPs are these: ElectronWP, MuonWP or PhotonWP
     *
     * @return return
    */
    StatusCode setupPerObjectWPs(const std::vector<std::string>& WPs, const std::string& objectWPtype);
  };
}  // namespace top

#endif  // TOPCPTOOLS_TOPISOLATIONCPTOOLS_H_
