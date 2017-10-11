/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonScaleFactorCalculator.h 786766 2016-11-28 03:21:13Z tpelzer $
#ifndef ANALYSISTOP_TOPCORRECTIONS_MUONSCALEFACTORCALCULATOR_H
#define ANALYSISTOP_TOPCORRECTIONS_MUONSCALEFACTORCALCULATOR_H

/**
  * @author John Morris <john.morris@cern.ch>
  *
  * @brief MuonScaleFactorCalculator
  *   Calculate all muon scale factors and decorate
  *
  * $Revision: 786766 $
  * $Date: 2016-11-28 03:21:13 +0000 (Mon, 28 Nov 2016) $
  *
  **/

// system include(s):
#include <memory>
#include <set>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

// CP Tool include(s):
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"
#include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"

// Forward declaration(s):
namespace top {
class TopConfig;
}

namespace top {

class MuonScaleFactorCalculator final : public asg::AsgTool {
 public:
  explicit MuonScaleFactorCalculator(const std::string& name);
  virtual ~MuonScaleFactorCalculator() {}

  // Delete Standard constructors
  MuonScaleFactorCalculator(const MuonScaleFactorCalculator& rhs) = delete;
  MuonScaleFactorCalculator(MuonScaleFactorCalculator&& rhs) = delete;
  MuonScaleFactorCalculator& operator=(const MuonScaleFactorCalculator& rhs) = delete;

  StatusCode initialize();
  StatusCode execute();

 private:
  // Helper function to retrieve a toolhandle and
  // get the recommended systematics of that tool
  // into a std::set
  template<typename T>
    void retrieveSystematicTool(const ToolHandle<T>& tool,
                                std::set<std::string>& recommended_systematics);
  // We have (had) a lot of lines like ::
  //     top::check(tool->applySystematicVariation(...),
  //     "Failed to set systematic variation");
  // This function will essentially do that but try to give a more
  // sensible and specific error message.
  // If the SystematicSet is empty then it sets to the nominal,
  // so edit error message accordingly.
  // (This might be expensive).
  //
  // Since this is general, we should maybe move somewhere
  // more common.
  template<typename T>
    void applySystematicVariation(ToolHandle<T>& tool,
                                  const CP::SystematicSet& systematic);

  // Check that all recommended systematics are implemented
  // The arguments are two c++ std::set, for recommended & implemented
  // systematics. Recommended systematics can be automatically extracted
  // from tools (see retrieveSystematicTool above) while the implemented
  // systematics must be hard-coded by the maintainer of this class.
  StatusCode checkSystematicsImplemented(const std::set<std::string>& recommended,
                                         const std::set<std::string>& implemented);

  // Use this function to decorate the trigger efficiency
  // onto the muon. Try not to use directly, instead use
  // decorateTrigSFandEff(), see below.
  void decorateTriggerEfficiency(ToolHandle<CP::IMuonTriggerScaleFactors>& tool,
                                 const xAOD::Muon& muon,
                                 const SG::AuxElement::Decorator<float>& decor);

  // Use this function to decorate the trigger SF
  // onto the muon. Try not to use directly, instead use
  // decorateTrigSFandEff(), see below.
  void decorateTriggerSF(ToolHandle<CP::IMuonTriggerScaleFactors>& tool,
                         const ConstDataVector<xAOD::MuonContainer>& muon_cont,
                         const SG::AuxElement::Decorator<float>& decor);

  // This function is a nice wrapper of::
  //     applySystematicVariation(...);
  //     decorateTriggerEfficiency(...);
  // and::
  //     decorateTriggerSF(...);
  // We have this code block ~10 times so this hopefully
  // keeps it simple and ensures each block is the same.
  void decorateTrigSFandEff(ToolHandle<CP::IMuonTriggerScaleFactors>& tool,
                            const CP::SystematicSet& systematic,
                            const ConstDataVector<xAOD::MuonContainer>& muon_cont,
                            const SG::AuxElement::Decorator<float>& sf_decor,
                            const SG::AuxElement::Decorator<float>& eff_decor);

  // To keep things as readable as possible this function
  // decorates the muon with the loose and tight trigger SFs and effs
  void decorateTriggerSystematics(const ConstDataVector<xAOD::MuonContainer>& trigger_SF_muon);

  // As with ``applySystematicVariation``, we have (had) a
  // lot of lines::
  //     top::check(tool->getEfficiencyScaleFactor(muon, syst),
  //                "Failed to get efficiency SF");
  // This helper function will replace that, as well as calling
  // applySystematicVariation try and avoid errors
  void decorateEfficiencySF(ToolHandle<CP::IMuonEfficiencyScaleFactors>& tool,
                            const CP::SystematicSet& systematic,
                            const xAOD::Muon& muon,
                            const SG::AuxElement::Decorator<float>& decor);

  // Helper function to decorate isolation systematics
  // if we have set isolation to None for either the
  // tight or loose muon selections then this will still
  // decorate the SFs all set to 1.0
  void decorateIsolationSystematics(const xAOD::Muon& muon);

  // Helper function to decorate all TTVA SF systematics onto muon
  void decorateTTVASystematics(const xAOD::Muon& muon);

  // For a single systematic, decorate the ID SF onto muon
  // no reco Eff since 2.4.22
  void decorateIDSFandRecoEff(ToolHandle<CP::IMuonEfficiencyScaleFactors>& tool,
                              const CP::SystematicSet& systematic,
                              const xAOD::Muon& muon,
                              const SG::AuxElement::Decorator<float>& id_sf_decor);

  // Helper funciton to do above for all systematics
  void decorateIDSFandRecoEffSystematics(const xAOD::Muon& muon);

  std::shared_ptr<top::TopConfig> m_config;

  CP::SystematicSet m_systNominal;

  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactors;
  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactorsLoose;

  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactors_2015;
  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactorsLoose_2015;
  
  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactors_2016;
  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactorsLoose_2016;

  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactors_R21;
  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactorsLoose_R21;

  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEfficiencyCorrectionsTool;
  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEfficiencyCorrectionsToolLoose;
  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEfficiencyCorrectionsToolIso;
  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEfficiencyCorrectionsToolLooseIso;
  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEfficiencyCorrectionsToolTTVA;

  std::string m_decor_triggerEff;
  std::string m_decor_triggerEff_loose;
  std::string m_decor_triggerSF;
  std::string m_decor_triggerSF_loose;
  std::string m_decor_idSF;
  std::string m_decor_idSF_loose;
  std::string m_decor_isoSF;
  std::string m_decor_isoSF_loose;
  std::string m_decor_TTVA;

  bool m_do_muon_isolation_SFs;
  bool m_do_muon_isolation_SFs_loose;

  CP::SystematicSet m_trig_sf_stat_UP;
  CP::SystematicSet m_trig_sf_stat_DOWN;
  CP::SystematicSet m_trig_sf_syst_UP;
  CP::SystematicSet m_trig_sf_syst_DOWN;

  CP::SystematicSet m_reco_stat_UP;
  CP::SystematicSet m_reco_stat_DOWN;
  CP::SystematicSet m_reco_syst_UP;
  CP::SystematicSet m_reco_syst_DOWN;

  CP::SystematicSet m_reco_stat_lowpt_UP;
  CP::SystematicSet m_reco_stat_lowpt_DOWN;
  CP::SystematicSet m_reco_syst_lowpt_UP;
  CP::SystematicSet m_reco_syst_lowpt_DOWN;

  CP::SystematicSet m_iso_stat_UP;
  CP::SystematicSet m_iso_stat_DOWN;
  CP::SystematicSet m_iso_syst_UP;
  CP::SystematicSet m_iso_syst_DOWN;

  CP::SystematicSet m_TTVA_stat_UP;
  CP::SystematicSet m_TTVA_stat_DOWN;
  CP::SystematicSet m_TTVA_syst_UP;
  CP::SystematicSet m_TTVA_syst_DOWN;

  std::string m_muon_trigger_sf_config = "";
};
}  // namespace top
#endif  // ANALYSISTOP_TOPCORRECTIONS_MUONSCALEFACTORCALCULATOR_H
