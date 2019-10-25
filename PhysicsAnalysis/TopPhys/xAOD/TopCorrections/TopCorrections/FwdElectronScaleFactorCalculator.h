/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPCORRECTIONS_FWDELECTRONSCALEFACTORCALCULATOR_H
#define ANALYSISTOP_TOPCORRECTIONS_FWDELECTRONSCALEFACTORCALCULATOR_H

/**
 * @author Marco Vanadia <marco.vanadia@cern.ch>
 *
 * @brief FwdElectronScaleFactorCalculator
 *   Calculate all forward electron scale factors and decorate
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
#include "ElectronEfficiencyCorrection/ElectronChargeEfficiencyCorrectionTool.h"

// Forward declaration(s):
namespace top {
  class TopConfig;
}

namespace top {
  class FwdElectronScaleFactorCalculator final: public asg::AsgTool {
  public:
    explicit FwdElectronScaleFactorCalculator(const std::string& name);
    virtual ~FwdElectronScaleFactorCalculator() {}

    // Delete Standard constructors
    FwdElectronScaleFactorCalculator(const FwdElectronScaleFactorCalculator& rhs) = delete;
    FwdElectronScaleFactorCalculator(FwdElectronScaleFactorCalculator&& rhs) = delete;
    FwdElectronScaleFactorCalculator& operator = (const FwdElectronScaleFactorCalculator& rhs) = delete;

    StatusCode initialize();
    StatusCode execute();
  private:
    std::shared_ptr<top::TopConfig> m_config;

    CP::SystematicSet m_systNominal;
    CP::SystematicSet m_systID_UP;
    CP::SystematicSet m_systID_DOWN;

    ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronEffSFID;
    ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronEffSFIDLoose;

    std::string m_decor_idSF;
    std::string m_decor_idSF_loose;
  };
} // namespace
#endif
