/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOP_TOPCORRECTIONS_TAUSCALEFACTORCALCULATOR_H
#define ANALYSISTOP_TOPCORRECTIONS_TAUSCALEFACTORCALCULATOR_H

// system include(s):
#include <memory>
#include <set>
#include <map>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

// CP Tool include(s):
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"

// Forward declaration(s):
namespace top {
  class TopConfig;
}

namespace top {
  class TauScaleFactorCalculator final: public asg::AsgTool {
  public:
    explicit TauScaleFactorCalculator(const std::string& name);
    virtual ~TauScaleFactorCalculator() {}

    // Delete Standard constructors
    TauScaleFactorCalculator(const TauScaleFactorCalculator& rhs) = delete;
    TauScaleFactorCalculator(TauScaleFactorCalculator&& rhs) = delete;
    TauScaleFactorCalculator& operator = (const TauScaleFactorCalculator& rhs) = delete;

    StatusCode initialize();
    StatusCode execute();
  private:
    std::shared_ptr<top::TopConfig> m_config;

    ToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> m_tauEffCorrTool;
    ToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> m_tauEffCorrToolLoose;

    CP::SystematicSet m_systNominal;
    std::map< std::string, CP::SystematicSet > m_syst_map;
  };
}  // namespace top
#endif  // ANALYSISTOP_TOPCORRECTIONS_TAUSCALEFACTORCALCULATOR_H
