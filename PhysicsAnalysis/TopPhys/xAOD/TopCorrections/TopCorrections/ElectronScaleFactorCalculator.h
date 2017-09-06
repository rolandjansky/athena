/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronScaleFactorCalculator.h 799556 2017-03-05 19:46:03Z tpelzer $
#ifndef ANALYSISTOP_TOPCORRECTIONS_ELECTRONSCALEFACTORCALCULATOR_H
#define ANALYSISTOP_TOPCORRECTIONS_ELECTRONSCALEFACTORCALCULATOR_H

/**
  * @author John Morris <john.morris@cern.ch>
  * 
  * @brief ElectronScaleFactorCalculator
  *   Calculate all electron scale factors and decorate
  * 
  * $Revision: 799556 $
  * $Date: 2017-03-05 20:46:03 +0100 (Sun, 05 Mar 2017) $
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
#include "ElectronEfficiencyCorrection/IAsgElectronEfficiencyCorrectionTool.h"
#include "ElectronEfficiencyCorrection/ElectronChargeEfficiencyCorrectionTool.h"

// Forward declaration(s):
namespace top{
  class TopConfig;
}

namespace top{

  class ElectronScaleFactorCalculator final : public asg::AsgTool {
    public:
      explicit ElectronScaleFactorCalculator( const std::string& name );
      virtual ~ElectronScaleFactorCalculator(){}

      // Delete Standard constructors
      ElectronScaleFactorCalculator(const ElectronScaleFactorCalculator& rhs) = delete;
      ElectronScaleFactorCalculator(ElectronScaleFactorCalculator&& rhs) = delete;
      ElectronScaleFactorCalculator& operator=(const ElectronScaleFactorCalculator& rhs) = delete;

      StatusCode initialize();
      StatusCode execute();

    private:
      std::shared_ptr<top::TopConfig> m_config;

      CP::SystematicSet m_systNominal;
      CP::SystematicSet m_systTrigger_UP;
      CP::SystematicSet m_systTrigger_DOWN;
      CP::SystematicSet m_systReco_UP;
      CP::SystematicSet m_systReco_DOWN;
      CP::SystematicSet m_systID_UP;
      CP::SystematicSet m_systID_DOWN;
      CP::SystematicSet m_systIso_UP;
      CP::SystematicSet m_systIso_DOWN;
      CP::SystematicSet m_systChargeID_UP;
      CP::SystematicSet m_systChargeID_DOWN;
      CP::SystematicSet m_systChargeMisID_STAT_UP;
      CP::SystematicSet m_systChargeMisID_STAT_DOWN;
      CP::SystematicSet m_systChargeMisID_SYST_UP;
      CP::SystematicSet m_systChargeMisID_SYST_DOWN;
     
      ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronEffSFTrigger;
      ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronEffSFTriggerLoose;
      ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronEffTrigger;
      ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronEffTriggerLoose;
      ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronEffSFReco;
      ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronEffSFID;
      ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronEffSFIDLoose;

      bool m_electronEffIso_exists;
      bool m_electronEffIsoLoose_exists;
      ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronEffSFIso;
      ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronEffSFIsoLoose;
      bool m_electronEffChargeID_exists;
      bool m_electronEffChargeIDLoose_exists;
      bool m_electronEffChargeMisID_exists;
      bool m_electronEffChargeMisIDLoose_exists;
      ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronEffSFChargeID;
      ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronEffSFChargeIDLoose;
      ToolHandle<CP::ElectronChargeEfficiencyCorrectionTool> m_electronEffSFChargeMisID;
      ToolHandle<CP::ElectronChargeEfficiencyCorrectionTool> m_electronEffSFChargeMisIDLoose;
     
      std::string m_decor_triggerEff;
      std::string m_decor_triggerEff_loose;
      std::string m_decor_triggerSF;
      std::string m_decor_triggerSF_loose;
      std::string m_decor_recoSF;
      std::string m_decor_idSF;
      std::string m_decor_idSF_loose;  
      std::string m_decor_isoSF;
      std::string m_decor_isoSF_loose;        
      std::string m_decor_chargeidSF;        
      std::string m_decor_chargeidSF_loose;
      std::string m_decor_chargemisidSF;
      std::string m_decor_chargemisidSF_loose;

  };
} // namespace
#endif
