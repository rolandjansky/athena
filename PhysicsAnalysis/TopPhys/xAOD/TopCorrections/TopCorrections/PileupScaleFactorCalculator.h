/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOP_TOPCORRECTIONS_PILEUPSCALEFACTORCALCULATOR_H
#define ANALYSISTOP_TOPCORRECTIONS_PILEUPSCALEFACTORCALCULATOR_H


// Framework include(s):
#include "AsgTools/AsgTool.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"

// Forward declaration(s):
namespace top{
  class TopConfig;
}

namespace top{

  class PileupScaleFactorCalculator final : public asg::AsgTool {
    public:
      explicit PileupScaleFactorCalculator( const std::string& name );
      virtual ~PileupScaleFactorCalculator(){}

      // Delete Standard constructors
      PileupScaleFactorCalculator(const PileupScaleFactorCalculator& rhs) = delete;
      PileupScaleFactorCalculator(PileupScaleFactorCalculator&& rhs) = delete;
      PileupScaleFactorCalculator& operator=(const PileupScaleFactorCalculator& rhs) = delete;

      StatusCode initialize();
      StatusCode execute();

    private:
      std::shared_ptr<top::TopConfig> m_config;
      
      ///Pileup Reweighting Tool
      ToolHandle<CP::IPileupReweightingTool> m_pileupReweightingTool;

      // systematics
      CP::SystematicSet m_systNominal;
      CP::SystematicSet m_systDataSFUp;
      CP::SystematicSet m_systDataSFDown;

      bool m_mu_dependent_PRW = true;
  };
} // namespace
#endif
