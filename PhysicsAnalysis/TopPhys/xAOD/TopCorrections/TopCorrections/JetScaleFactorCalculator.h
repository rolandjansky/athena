/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetScaleFactorCalculator.h 724681 2016-02-17 18:20:27Z tneep $
#ifndef ANALYSISTOP_TOPCORRECTIONS_JETSCALEFACTORCALCULATOR_H
#define ANALYSISTOP_TOPCORRECTIONS_JETSCALEFACTORCALCULATOR_H

/**
  * @author John Morris <john.morris@cern.ch>
  *
  * @brief JetScaleFactorCalculator
  *   Calculate all jet scale factors and decorate
  *
  * $Revision: 724681 $
  * $Date: 2016-02-17 19:20:27 +0100 (Wed, 17 Feb 2016) $
  *
  **/

// system include(s):
#include <memory>
#include <set>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

#include "JetJvtEfficiency/IJetJvtEfficiency.h"

// CP Tool include(s):
// #include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"

// Forward declaration(s):
namespace top {
class TopConfig;
}

namespace top {

class JetScaleFactorCalculator final : public asg::AsgTool {
 public:
  explicit JetScaleFactorCalculator(const std::string& name);
  virtual ~JetScaleFactorCalculator() {}

  // Delete Standard constructors
  JetScaleFactorCalculator(const JetScaleFactorCalculator& rhs) = delete;
  JetScaleFactorCalculator(JetScaleFactorCalculator&& rhs) = delete;
  JetScaleFactorCalculator& operator=(const JetScaleFactorCalculator& rhs) = delete;

  StatusCode initialize();
  StatusCode execute();

 private:
  std::shared_ptr<top::TopConfig> m_config;

  CP::SystematicSet m_systNominal;
  CP::SystematicSet m_systUP;
  CP::SystematicSet m_systDOWN;

  ToolHandle<CP::IJetJvtEfficiency> m_jvt_tool;
};
}  // namespace top
#endif  // ANALYSISTOP_TOPCORRECTIONS_JETSCALEFACTORCALCULATOR_H
