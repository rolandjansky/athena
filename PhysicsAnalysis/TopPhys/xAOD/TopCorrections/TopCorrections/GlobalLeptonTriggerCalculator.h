/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

// $Id: GlobalLeptonTriggerCalculator.h 799556 2017-03-05 19:46:03Z tpelzer $
#ifndef ANALYSISTOP_TOPCORRECTIONS_GLOBALLEPTONTRIGGERCALCULATOR_H
#define ANALYSISTOP_TOPCORRECTIONS_GLOBALLEPTONTRIGGERCALCULATOR_H

/**
 * @author Ian Connelly <ian.connelly@cern.ch>
 *
 * @brief Global lepton trigger calculator
 *   Interface to retrieve the global lepton trigger scale factors
 *
 **/

// system include(s):
#include <memory>
#include <set>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

// CP Tool include(s):
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonEfficiencyCorrectionTool.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"

// Forward declaration(s):
namespace xAOD {
  class SystematicEvent;
}
namespace top {
  class TopConfig;
}

namespace top {
  class GlobalLeptonTriggerCalculator final: public asg::AsgTool {
  public:
    explicit GlobalLeptonTriggerCalculator(const std::string& name);
    virtual ~GlobalLeptonTriggerCalculator() {}

    // Delete Standard constructors
    GlobalLeptonTriggerCalculator(const GlobalLeptonTriggerCalculator& rhs) = delete;
    GlobalLeptonTriggerCalculator(GlobalLeptonTriggerCalculator&& rhs) = delete;
    GlobalLeptonTriggerCalculator& operator = (const GlobalLeptonTriggerCalculator& rhs) = delete;

    StatusCode initialize();
    StatusCode execute();
  private:
    void processEvent(xAOD::SystematicEvent* systEvent, bool withScaleFactorVariations);

    std::shared_ptr<top::TopConfig> m_config;

    ToolHandle<ITrigGlobalEfficiencyCorrectionTool> m_globalTriggerSF;
    ToolHandle<ITrigGlobalEfficiencyCorrectionTool> m_globalTriggerSFLoose;

    std::vector<ToolHandle<IAsgElectronEfficiencyCorrectionTool> > m_electronTools;
    std::vector<ToolHandle<CP::IMuonTriggerScaleFactors> > m_muonTools;
    std::vector<ToolHandle<IAsgPhotonEfficiencyCorrectionTool> > m_photonTools;

    std::string m_decor_triggerSF;
  };
} // namespace
#endif
