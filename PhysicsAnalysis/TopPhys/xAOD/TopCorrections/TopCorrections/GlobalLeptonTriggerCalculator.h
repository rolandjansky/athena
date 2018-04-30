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
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"

// Forward declaration(s):
namespace top{
  class TopConfig;
}

namespace top{

  class GlobalLeptonTriggerCalculator final : public asg::AsgTool {
    public:
      explicit GlobalLeptonTriggerCalculator( const std::string& name );
      virtual ~GlobalLeptonTriggerCalculator(){}

      // Delete Standard constructors
      GlobalLeptonTriggerCalculator(const GlobalLeptonTriggerCalculator& rhs) = delete;
      GlobalLeptonTriggerCalculator(GlobalLeptonTriggerCalculator&& rhs) = delete;
      GlobalLeptonTriggerCalculator& operator=(const GlobalLeptonTriggerCalculator& rhs) = delete;

      StatusCode initialize();
      StatusCode execute();

    private:
      std::shared_ptr<top::TopConfig> m_config;

      CP::SystematicSet m_systNominal;
      CP::SystematicSet m_systTrigger_ELECTRON_UP;
      CP::SystematicSet m_systTrigger_ELECTRON_DOWN;
      CP::SystematicSet m_systTrigger_MUON_STAT_UP;
      CP::SystematicSet m_systTrigger_MUON_STAT_DOWN;
      CP::SystematicSet m_systTrigger_MUON_SYST_UP;
      CP::SystematicSet m_systTrigger_MUON_SYST_DOWN;

      ToolHandle<ITrigGlobalEfficiencyCorrectionTool> m_globalTriggerSF;
      ToolHandle<ITrigGlobalEfficiencyCorrectionTool> m_globalTriggerSFLoose;

      std::string m_decor_triggerSF;
      std::string m_decor_triggerSF_loose;
      std::string m_decor_triggerEffMC;
      std::string m_decor_triggerEffMC_loose;
      std::string m_decor_triggerEffData;
      std::string m_decor_triggerEffData_loose;

      // Accessor method to centralise the checks to leptons across functions
      SG::AuxElement::Accessor<char> m_selectedLepton;
      SG::AuxElement::Accessor<char> m_selectedLeptonLoose;

      ///-- Functions to handle different configurations --///
      StatusCode executeNominalVariations();
      StatusCode executeElectronSystematics();
      StatusCode executeMuonSystematics();
      StatusCode decorateEventInfo(std::string, double);
      void Print(double, double, double);

  };
} // namespace
#endif
