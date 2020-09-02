/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopCorrections/ScaleFactorRetriever.h"
#include "TopCorrections/TopCorrectionsTools.h"

#include <vector>
#include <string>

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopEvent/SystematicEvent.h"
#include "TopConfiguration/ConfigurationSettings.h"
#include "TopConfiguration/TopConfig.h"
#include "AthContainers/AuxElement.h"
#include "xAODRootAccess/TStore.h"
#include "xAODEventInfo/EventInfo.h"

namespace top {
  ScaleFactorRetriever::ScaleFactorRetriever(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),
    m_preferGlobalTriggerSF(ConfigurationSettings::get()->feature("PreferGlobalTriggerSF")) {
    declareProperty("config", m_config);
  }

  StatusCode ScaleFactorRetriever::initialize() {
    ATH_MSG_INFO("Initialising " << this->name());

    std::shared_ptr<std::vector<std::string> > selectors = m_config->allSelectionNames();

    for (std::string selPtr : *selectors) {
      std::vector<std::string> muonTrig_Tight = m_config->muonTriggers_Tight(selPtr);
      std::vector<std::string> electronTrig_Tight = m_config->electronTriggers_Tight(selPtr);
      std::vector<std::string> muonTrig_Loose = m_config->muonTriggers_Loose(selPtr);
      std::vector<std::string> electronTrig_Loose = m_config->electronTriggers_Loose(selPtr);

      for (auto trig : muonTrig_Tight)
        m_muonTriggers_Tight.push_back(trig);
      for (auto trig : muonTrig_Loose)
        m_muonTriggers_Loose.push_back(trig);

      for (auto trig : electronTrig_Tight)
        m_electronTriggers_Tight.push_back(trig);
      for (auto trig : electronTrig_Loose)
        m_electronTriggers_Loose.push_back(trig);
    }

    return StatusCode::SUCCESS;
  }

  // Pile up SF
  bool ScaleFactorRetriever::hasPileupSF(const top::Event& event) {
    // Probably don't need this function...
    return event.m_info->isAvailable<float>("PileupWeight");
  }

  float ScaleFactorRetriever::pileupSF(const top::Event& event, int var) {
    float sf(1.);

    if (var == 0) {  // nominal
      if (event.m_info->isAvailable<float>("PileupWeight")) sf = event.m_info->auxdataConst<float>("PileupWeight");
    } else if (var == 1) {  // dataSF up
      if (event.m_info->isAvailable<float>("PileupWeight_UP")) sf =
          event.m_info->auxdataConst<float>("PileupWeight_UP");
    } else if (var == -1) {  // dataSF down
      if (event.m_info->isAvailable<float>("PileupWeight_DOWN")) sf = event.m_info->auxdataConst<float>(
          "PileupWeight_DOWN");
    }

    return sf;
  }

  // Obtain the lepton SF
  float ScaleFactorRetriever::leptonSF(const top::Event& event, const top::topSFSyst SFSyst) const {
    return
      electronSF(event, SFSyst, top::topSFComp::ALL)
      * muonSF(event, SFSyst, top::topSFComp::ALL)
      * triggerSF(event, SFSyst);
  }

  float ScaleFactorRetriever::globalTriggerSF(const top::Event& event, const top::topSFSyst SFSyst) const {
    float sf(1.0);

    static const std::string prefix = "AnalysisTop_Trigger_SF_";

    xAOD::SystematicEvent const* eventInfo = event.m_systematicEvent;
    top::check(eventInfo, "Failed to retrieve SystematicEvent");
    const bool electronTriggerIsEmpty = event.m_isLoose ? m_electronTriggers_Loose.empty() : m_electronTriggers_Tight.empty();
    const bool muonTriggerIsEmpty     = event.m_isLoose ? m_muonTriggers_Loose.empty()     : m_muonTriggers_Tight.empty();

    // Create a hard-coded map linking top::topSFSyst <-> EventInfo decoration
    switch (SFSyst) {
    case top::topSFSyst::EL_SF_Trigger_UP:
      if (electronTriggerIsEmpty) {
        sf = 1;
      } else {
        sf = eventInfo->auxdataConst<float>(prefix + "EL_EFF_Trigger_TOTAL_1NPCOR_PLUS_UNCOR__1up");
      }
      break;

    case top::topSFSyst::EL_SF_Trigger_DOWN:
      if (electronTriggerIsEmpty) {
        sf = 1;
      } else {
        sf = eventInfo->auxdataConst<float>(prefix + "EL_EFF_Trigger_TOTAL_1NPCOR_PLUS_UNCOR__1down");
      }
      break;

    case top::topSFSyst::MU_SF_Trigger_STAT_UP:
      if (muonTriggerIsEmpty) {
        sf = 1;
      } else {
        sf = eventInfo->auxdataConst<float>(prefix + "MUON_EFF_TrigStatUncertainty__1up");
      }
      break;

    case top::topSFSyst::MU_SF_Trigger_STAT_DOWN:
      if (muonTriggerIsEmpty) {
        sf = 1;
      } else {
        sf = eventInfo->auxdataConst<float>(prefix + "MUON_EFF_TrigStatUncertainty__1down");
      }
      break;

    case top::topSFSyst::MU_SF_Trigger_SYST_UP:
      if (muonTriggerIsEmpty) {
        sf = 1;
      } else {
        sf = eventInfo->auxdataConst<float>(prefix + "MUON_EFF_TrigSystUncertainty__1up");
      }
      break;

    case top::topSFSyst::MU_SF_Trigger_SYST_DOWN:
      if (muonTriggerIsEmpty) {
        sf = 1;
      } else {
        sf = eventInfo->auxdataConst<float>(prefix + "MUON_EFF_TrigSystUncertainty__1down");
      }
      break;

    default:
      // Nominal weight
      sf = eventInfo->auxdataConst<float>(prefix);
      break;
    }
    return sf;
  }

  float ScaleFactorRetriever::triggerSF(const top::Event& event,
                                        const top::topSFSyst SFSyst) const {
    return(m_preferGlobalTriggerSF &&
           m_config->useGlobalTrigger() ? globalTriggerSF(event, SFSyst) : oldTriggerSF(event, SFSyst));
  }

  float ScaleFactorRetriever::oldTriggerSF(const top::Event& event,
                                           const top::topSFSyst SFSyst) const {
    std::string electronID = m_config->electronID();
    if (event.m_isLoose) {
      electronID = m_config->electronIDLoose();
    }

    std::string muonID = m_config->muonQuality();
    if (event.m_isLoose) {
      muonID = m_config->muonQualityLoose();
    }

    std::vector<float> triggerSFvec;

    // Loop over electrons
    for (auto elPtr : event.m_electrons) {
      bool trigMatch = false;

      for (const auto& trigger : event.m_isLoose ? m_electronTriggers_Loose : m_electronTriggers_Tight) {
        std::string trig = "TRIGMATCH_" + trigger;
        if (elPtr->isAvailable<char>(trig)) {
          if (elPtr->auxdataConst<char>(trig) == 1) trigMatch = true;
        }
      }

      if (trigMatch) triggerSFvec.push_back(electronSF_Trigger(*elPtr, electronID, SFSyst, event.m_isLoose));
    }

    // Loop over muons
    for (auto muPtr : event.m_muons) {
      bool trigMatch = false;

      for (const auto& trigger : event.m_isLoose ? m_muonTriggers_Loose : m_muonTriggers_Tight) {
        std::string trig = "TRIGMATCH_" + trigger;
        if (muPtr->isAvailable<char>(trig)) {
          if (muPtr->auxdataConst<char>(trig) == 1) trigMatch = true;
        }
      }

      if (trigMatch) triggerSFvec.push_back(muonSF_Trigger(*muPtr, muonID, SFSyst, event.m_isLoose));
    }

    // for the cutflow histograms, in case the lepton triggers have not been checked yet
    if (triggerSFvec.empty()) return 1.0;

    float trigSF = 1.0;
    for (float SF : triggerSFvec)
      trigSF *= (1.0 - SF);

    return 1.0 - trigSF;
  }

  std::vector<float> ScaleFactorRetriever::electronSFSystVariationVector(const top::Event& event,
                                                                         const top::topSFComp SFComp, int var) const {
    std::vector<float> sf;
    if (abs(var) != 1) {
      ATH_MSG_ERROR("ScaleFactorRetriever::electronSFSystVariationVector must be called with var=+1 (up) or -1 (down)");
      return sf;
    }
    if (SFComp != top::topSFComp::RECO && SFComp != top::topSFComp::ID && SFComp != top::topSFComp::ISOLATION) {
      ATH_MSG_ERROR(
        "ScaleFactorRetriever::electronSFSystVariationVector is currently implemented only for SFComp=RECO, ID, ISOLATION");
      return sf;
    }

    std::string decorationName = "EL_SF_";
    if(event.m_isLoose && SFComp != top::topSFComp::RECO) decorationName = "EL_LOOSE_SF_";
    std::string electronID = "";
    std::string electronIso = "";

    if (SFComp == top::topSFComp::RECO) decorationName += "Reco_";
    if (SFComp == top::topSFComp::ID) {
      decorationName += "ID_";
      electronID = m_config->electronID();
      if (event.m_isLoose && !m_config->applyTightSFsInLooseTree()) {
        electronID = m_config->electronIDLoose();
      }
      decorationName += electronID;
      decorationName += "_";
    }
    if (SFComp == top::topSFComp::ISOLATION) {
      decorationName += "Iso_";
      electronIso = m_config->electronIsolationSF();
      if (event.m_isLoose && !m_config->applyTightSFsInLooseTree()) {
        electronIso = m_config->electronIsolationSFLoose();
      }
      decorationName += electronIso;
      decorationName += "_";
    }

    if (var == 1) decorationName += "CorrModel_UP";
    else decorationName += "CorrModel_DOWN";

    for (auto elPtr : event.m_electrons) {
      std::vector<float> sf_aux;
      if (elPtr->isAvailable<std::vector<float> >(decorationName)) {
        sf_aux = elPtr->auxdataConst<std::vector<float> >(decorationName);
      } else {
        ATH_MSG_ERROR(
          "ScaleFactorRetriever::electronSFSystVariationVector error in accessing decoration " << decorationName);
      }

      if (sf.size() == 0) sf = std::vector<float>(sf_aux.size(), leptonSF(event, top::topSFSyst::nominal));
      if (sf.size() != sf_aux.size()) ATH_MSG_ERROR(
          "ScaleFactorRetriever::electronSFSystVariationVector error in size of vector of electron SFs");
      double oldSF = 1.;
      if (SFComp == top::topSFComp::RECO) oldSF = electronSF_Reco(*elPtr, top::topSFSyst::nominal,event.m_isLoose);
      if (SFComp == top::topSFComp::ID) oldSF = electronSF_ID(*elPtr, electronID, top::topSFSyst::nominal,event.m_isLoose);
      if (SFComp == top::topSFComp::ISOLATION) oldSF = electronSF_Isol(*elPtr, electronIso, top::topSFSyst::nominal,event.m_isLoose);

      for (unsigned int i = 0; i < sf.size(); i++) {
        sf[i] *= (sf_aux[i] / oldSF);
      }
    }//end of loop on electrons

    return sf;
  }

  // Obtain the electron SF
  float ScaleFactorRetriever::electronSF(const top::Event& event,
                                         const top::topSFSyst SFSyst,
                                         const top::topSFComp SFComp) const {
    float sf(1.);

    std::string electronID = m_config->electronID();
    if (event.m_isLoose && !m_config->applyTightSFsInLooseTree()) {
      electronID = m_config->electronIDLoose();
    }
    std::string electronIso = m_config->electronIsolationSF();
    if (event.m_isLoose && !m_config->applyTightSFsInLooseTree()) {
      electronIso = m_config->electronIsolationSFLoose();
    }

    float reco(1.);
    float id(1.);
    float isol(1.);
    float chargeid(1.);
    float chargemisid(1.);

    // Loop over electrons
    for (auto elPtr : event.m_electrons) {
      if (event.m_isLoose && m_config->applyTightSFsInLooseTree() &&
          !elPtr->auxdataConst<char>("passPreORSelection")) continue; // in case one want the tight SFs in the loose
                                                                      // tree, need to only take the tight leptons

      reco *= electronSF_Reco(*elPtr, SFSyst,event.m_isLoose);
      id *= electronSF_ID(*elPtr, electronID, SFSyst,event.m_isLoose);
      isol *= electronSF_Isol(*elPtr, electronIso, SFSyst,event.m_isLoose);
      chargeid *= electronSF_ChargeID(*elPtr, electronID, electronIso, SFSyst,event.m_isLoose);
      chargemisid *= electronSF_ChargeMisID(*elPtr, electronID, electronIso, SFSyst,event.m_isLoose);
    }

    sf = reco * id * isol; // *chargeid*chargemisid; // let the charge id scale factors out until further tested by
                           // users

    if (SFComp == top::topSFComp::RECO) return reco;
    else if (SFComp == top::topSFComp::ID) return id;
    else if (SFComp == top::topSFComp::ISOLATION) return isol;
    else if (SFComp == top::topSFComp::CHARGEID) return chargeid;
    else if (SFComp == top::topSFComp::CHARGEMISID) return chargemisid;
    else if (SFComp == top::topSFComp::ALL) return sf;

    return sf;
  }

// Obtain the electron SF
  float ScaleFactorRetriever::fwdElectronSF(const top::Event& event,
                                            const top::topSFSyst SFSyst,
                                            const top::topSFComp SFComp) const {
    float sf(1.);

    std::string fwdElectronID = m_config->fwdElectronID();
    if (event.m_isLoose) {
      fwdElectronID = m_config->fwdElectronIDLoose();
    }

    float reco(1.);
    float id(1.);
    float isol(1.);
    float chargeid(1.);
    float chargemisid(1.);

    // Loop over electrons
    for (auto elPtr : event.m_fwdElectrons) {
      //currently on ID SFs are supported for fwd electrons
      id *= fwdElectronSF_ID(*elPtr, fwdElectronID, SFSyst, event.m_isLoose);
    }

    sf = reco * id * isol;

    if (SFComp == top::topSFComp::RECO) return reco;
    else if (SFComp == top::topSFComp::ID) return id;
    else if (SFComp == top::topSFComp::ISOLATION) return isol;
    else if (SFComp == top::topSFComp::CHARGEID) return chargeid;
    else if (SFComp == top::topSFComp::CHARGEMISID) return chargemisid;
    else if (SFComp == top::topSFComp::ALL) return sf;

    return sf;
  }

  float ScaleFactorRetriever::electronSF_Trigger(const xAOD::Electron& x,
                                                 const top::topSFSyst SFSyst,
                                                 bool isLoose) const {
    return electronSF_Trigger(x, (isLoose ? m_config->electronIDLoose() : m_config->electronID()), SFSyst, isLoose);
  }

  float ScaleFactorRetriever::electronEff_Trigger(const xAOD::Electron& x,
                                                  const top::topSFSyst SFSyst,
                                                  bool isLoose) const {
    return electronEff_Trigger(x, (isLoose ? m_config->electronIDLoose() : m_config->electronID()), SFSyst, isLoose);
  }

  float ScaleFactorRetriever::electronSF_Trigger(const xAOD::Electron& x,
                                                 const std::string& id,
                                                 const top::topSFSyst SFSyst,
                                                 bool isLoose) const {
    float sf(1.);
    
    std::string prefix="EL";
    if(isLoose) prefix+="_LOOSE";

    if (x.isAvailable<float>(prefix+"_SF_Trigger_" + id)) {
      sf = x.auxdataConst<float>(prefix+"_SF_Trigger_" + id);
    }

    if (SFSyst == top::topSFSyst::EL_SF_Trigger_UP) {
      if (x.isAvailable<float>(prefix+"_SF_Trigger_" + id + "_UP")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Trigger_" + id + "_UP");
      }
    }

    if (SFSyst == top::topSFSyst::EL_SF_Trigger_DOWN) {
      if (x.isAvailable<float>(prefix+"_SF_Trigger_" + id + "_DOWN")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Trigger_" + id + "_DOWN");
      }
    }

    return sf;
  }

  float ScaleFactorRetriever::electronEff_Trigger(const xAOD::Electron& x,
                                                  const std::string& id,
                                                  const top::topSFSyst SFSyst,
                                                  bool isLoose) const {
    float eff(1.);
    
    std::string prefix="EL";
    if(isLoose) prefix+="_LOOSE";

    if (x.isAvailable<float>(prefix+"_EFF_Trigger_" + id)) {
      eff = x.auxdataConst<float>(prefix+"_EFF_Trigger_" + id);
    }

    if (SFSyst == top::topSFSyst::EL_SF_Trigger_UP) {
      if (x.isAvailable<float>(prefix+"_EFF_Trigger_" + id + "_UP")) {
        eff = x.auxdataConst<float>(prefix+"_EFF_Trigger_" + id + "_UP");
      }
    }

    if (SFSyst == top::topSFSyst::EL_SF_Trigger_DOWN) {
      if (x.isAvailable<float>(prefix+"_EFF_Trigger_" + id + "_DOWN")) {
        eff = x.auxdataConst<float>(prefix+"_EFF_Trigger_" + id + "_DOWN");
      }
    }

    return eff;
  }

  float ScaleFactorRetriever::electronSF_Reco(const xAOD::Electron& x,
                                              const top::topSFSyst SFSyst,
                                              bool isLoose) const {
    float sf(1.);
    
    std::string prefix="EL";
    if(isLoose) prefix+="_LOOSE";

    if (x.isAvailable<float>(prefix+"_SF_Reco")) {
      sf = x.auxdataConst<float>(prefix+"_SF_Reco");
    }

    if (SFSyst == top::topSFSyst::EL_SF_Reco_UP) {
      if (x.isAvailable<float>(prefix+"_SF_Reco_UP")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Reco_UP");
      }
    }

    if (SFSyst == top::topSFSyst::EL_SF_Reco_DOWN) {
      if (x.isAvailable<float>(prefix+"_SF_Reco_DOWN")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Reco_DOWN");
      }
    }

    return sf;
  }

  float ScaleFactorRetriever::electronSF_ID(const xAOD::Electron& x,
                                            const top::topSFSyst SFSyst,
                                            bool isLoose) const {
    return electronSF_ID(x, (isLoose ? m_config->electronIDLoose() : m_config->electronID()), SFSyst, isLoose);
  }

  float ScaleFactorRetriever::fwdElectronSF_ID(const xAOD::Electron& x,
                                               const top::topSFSyst SFSyst,
                                               bool isLoose) const {
    return fwdElectronSF_ID(x, (isLoose ? m_config->fwdElectronIDLoose() : m_config->fwdElectronID()), SFSyst, isLoose);
  }

  float ScaleFactorRetriever::electronSF_ID(const xAOD::Electron& x,
                                            const std::string& id,
                                            const top::topSFSyst SFSyst,
                                            bool isLoose) const {
    float sf(1.);
    
    std::string prefix="EL";
    if(isLoose) prefix+="_LOOSE";

    if (x.isAvailable<float>(prefix+"_SF_ID_" + id)) {
      sf = x.auxdataConst<float>(prefix+"_SF_ID_" + id);
    }

    if (SFSyst == top::topSFSyst::EL_SF_ID_UP) {
      if (x.isAvailable<float>(prefix+"_SF_ID_" + id + "_UP")) {
        sf = x.auxdataConst<float>(prefix+"_SF_ID_" + id + "_UP");
      }
    }

    if (SFSyst == top::topSFSyst::EL_SF_ID_DOWN) {
      if (x.isAvailable<float>(prefix+"_SF_ID_" + id + "_DOWN")) {
        sf = x.auxdataConst<float>(prefix+"_SF_ID_" + id + "_DOWN");
      }
    }

    return sf;
  }

  float ScaleFactorRetriever::fwdElectronSF_ID(const xAOD::Electron& x,
                                               const std::string& id,
                                               const top::topSFSyst SFSyst,
                                               bool isLoose) const {
    float sf(1.);
    
    std::string prefix="FWDEL";
    if(isLoose) prefix+="_LOOSE";

    if (x.isAvailable<float>(prefix+"_SF_ID_" + id)) {
      sf = x.auxdataConst<float>(prefix+"_SF_ID_" + id);
    }

    if (SFSyst == top::topSFSyst::FWDEL_SF_ID_UP) {
      if (x.isAvailable<float>(prefix+"_SF_ID_" + id + "_UP")) {
        sf = x.auxdataConst<float>(prefix+"_SF_ID_" + id + "_UP");
      }
    }

    if (SFSyst == top::topSFSyst::FWDEL_SF_ID_DOWN) {
      if (x.isAvailable<float>(prefix+"_SF_ID_" + id + "_DOWN")) {
        sf = x.auxdataConst<float>(prefix+"_SF_ID_" + id + "_DOWN");
      }
    }

    return sf;
  }

  float ScaleFactorRetriever::electronSF_Isol(const xAOD::Electron& x,
                                              const top::topSFSyst SFSyst,
                                              bool isLoose) const {
    return electronSF_Isol(x, (isLoose ? m_config->electronIsolationSFLoose() : m_config->electronIsolationSF()),
                           SFSyst, isLoose);
  }

  float ScaleFactorRetriever::electronSF_Isol(const xAOD::Electron& x,
                                              const std::string& iso,
                                              const top::topSFSyst SFSyst,
                                              bool isLoose) const {
    float sf(1.);
    
    std::string prefix="EL";
    if(isLoose) prefix+="_LOOSE";

    if (x.isAvailable<float>(prefix+"_SF_Iso_" + iso)) {
      sf = x.auxdataConst<float>(prefix+"_SF_Iso_" + iso);
    }


    if (SFSyst == top::topSFSyst::EL_SF_Isol_UP) {
      if (x.isAvailable<float>(prefix+"_SF_Iso_" + iso + "_UP")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Iso_" + iso + "_UP");
      }
    }

    if (SFSyst == top::topSFSyst::EL_SF_Isol_DOWN) {
      if (x.isAvailable<float>(prefix+"_SF_Iso_" + iso + "_DOWN")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Iso_" + iso + "_DOWN");
      }
    }

    return sf;
  }

  float ScaleFactorRetriever::electronSF_ChargeID(const xAOD::Electron& x,
                                                  const top::topSFSyst SFSyst,
                                                  bool isLoose) const {
    return electronSF_ChargeID(x, (isLoose ? m_config->electronIDLoose() : m_config->electronID()),
                               (isLoose ? m_config->electronIsolationLoose() : m_config->electronIsolation()), SFSyst, isLoose);
  }

  float ScaleFactorRetriever::electronSF_ChargeID(const xAOD::Electron& x,
                                                  const std::string& id, const std::string& iso,
                                                  const top::topSFSyst SFSyst,
                                                  bool isLoose) const {
    float sf(1.);
    
    std::string prefix="EL";
    if(isLoose) prefix+="_LOOSE";

    if (x.isAvailable<float>(prefix+"_SF_ChargeID_" + id + "_" + iso)) {
      sf = x.auxdataConst<float>(prefix+"_SF_ChargeID_" + id + "_" + iso);
    }

    if (SFSyst == top::topSFSyst::EL_SF_ChargeID_UP) {
      if (x.isAvailable<float>(prefix+"_SF_ChargeID_" + id + "_" + iso + "_UP")) {
        sf = x.auxdataConst<float>(prefix+"_SF_ChargeID_" + id + "_" + iso + "_UP");
      }
    }

    if (SFSyst == top::topSFSyst::EL_SF_ChargeID_DOWN) {
      if (x.isAvailable<float>(prefix+"_SF_ChargeID_" + id + "_" + iso + "_DOWN")) {
        sf = x.auxdataConst<float>(prefix+"_SF_ChargeID_" + id + "_" + iso + "_DOWN");
      }
    }
    return sf;
  }

  float ScaleFactorRetriever::electronSF_ChargeMisID(const xAOD::Electron& x,
                                                     const top::topSFSyst SFSyst,
                                                     bool isLoose) const {
    return electronSF_ChargeMisID(x, (isLoose ? m_config->electronIDLoose() : m_config->electronID()),
                                  (isLoose ? m_config->electronIsolationLoose() : m_config->electronIsolation()),
                                  SFSyst, isLoose);
  }

  float ScaleFactorRetriever::electronSF_ChargeMisID(const xAOD::Electron& x,
                                                     const std::string& id, const std::string& iso,
                                                     const top::topSFSyst SFSyst,
                                                     bool isLoose) const {
    float sf(1.);
    
    std::string prefix="EL";
    if(isLoose) prefix+="_LOOSE";

    if (x.isAvailable<float>(prefix+"_SF_ChargeMisID_" + id + "_" + iso)) {
      sf = x.auxdataConst<float>(prefix+"_SF_ChargeMisID_" + id + "_" + iso);
    }

    if (SFSyst == top::topSFSyst::EL_SF_ChargeMisID_STAT_UP) {
      if (x.isAvailable<float>(prefix+"_SF_ChargeMisID_" + id + "_" + iso + "_STAT_UP")) {
        sf = x.auxdataConst<float>(prefix+"_SF_ChargeMisID_" + id + "_" + iso + "_STAT_UP");
      }
    }

    if (SFSyst == top::topSFSyst::EL_SF_ChargeMisID_STAT_DOWN) {
      if (x.isAvailable<float>(prefix+"_SF_ChargeMisID_" + id + "_" + iso + "_STAT_DOWN")) {
        sf = x.auxdataConst<float>(prefix+"_SF_ChargeMisID_" + id + "_" + iso + "_STAT_DOWN");
      }
    }

    if (SFSyst == top::topSFSyst::EL_SF_ChargeMisID_SYST_UP) {
      if (x.isAvailable<float>(prefix+"_SF_ChargeMisID_" + id + "_" + iso + "_SYST_UP")) {
        sf = x.auxdataConst<float>(prefix+"_SF_ChargeMisID_" + id + "_" + iso + "_SYST_UP");
      }
    }

    if (SFSyst == top::topSFSyst::EL_SF_ChargeMisID_SYST_DOWN) {
      if (x.isAvailable<float>(prefix+"_SF_ChargeMisID_" + id + "_" + iso + "_SYST_DOWN")) {
        sf = x.auxdataConst<float>(prefix+"_SF_ChargeMisID_" + id + "_" + iso + "_SYST_DOWN");
      }
    }

    return sf;
  }

  // Obtain the muon SF
  float ScaleFactorRetriever::muonSF(const top::Event& event,
                                     const top::topSFSyst SFSyst,
                                     const top::topSFComp SFComp) const {
    float sf(1.);

    std::string muonID = m_config->muonQuality();
    std::string muonIso = m_config->muonIsolationSF();

    if (event.m_isLoose && !m_config->applyTightSFsInLooseTree()) {
      muonID = m_config->muonQualityLoose();
      muonIso = m_config->muonIsolationSFLoose();
    }

    float reco(1.);
    float id(1.);
    float isol(1.);
    float TTVA(1.);

    // Loop over muons
    for (auto muPtr : event.m_muons) {
      if (event.m_isLoose && m_config->applyTightSFsInLooseTree() &&
          !muPtr->auxdataConst<char>("passPreORSelection")) continue; // in case one want the tight SFs in the loose
                                                                      // tree, need to only take the tight leptons

      id *= muonSF_ID(*muPtr, muonID, SFSyst, event.m_isLoose);
      isol *= muonSF_Isol(*muPtr, muonIso, SFSyst, event.m_isLoose);

      if (m_config->applyTTVACut())                                       // if not using TTVA cut, leave SF set to 1.0
        TTVA *= muonSF_TTVA(*muPtr, SFSyst);
    }

    sf = id * isol * TTVA;

    if (SFComp == top::topSFComp::RECO) return reco;
    else if (SFComp == top::topSFComp::ID) return id;
    else if (SFComp == top::topSFComp::ISOLATION) return isol;
    else if (SFComp == top::topSFComp::TTVA) return TTVA;
    else if (SFComp == top::topSFComp::ALL) return sf;

    return sf;
  }

  float ScaleFactorRetriever::muonSF_Trigger(const xAOD::Muon& x,
                                             const top::topSFSyst SFSyst,
                                             bool isLoose) const {
    return muonSF_Trigger(x, (isLoose ? m_config->muonQualityLoose() : m_config->muonQuality()), SFSyst, isLoose);
  }

  float ScaleFactorRetriever::muonSF_Trigger(const xAOD::Muon& x,
                                             const std::string& id,
                                             const top::topSFSyst SFSyst,
                                             bool isLoose) const {
    float sf(1.);
    
    std::string prefix="MU";
    if(isLoose) prefix+="_LOOSE";

    if (x.isAvailable<float>(prefix+"_SF_Trigger_" + id)) {
      sf = x.auxdataConst<float>(prefix+"_SF_Trigger_" + id);
    }

    if (SFSyst == top::topSFSyst::MU_SF_Trigger_STAT_UP) {
      if (x.isAvailable<float>(prefix+"_SF_Trigger_" + id + "_STAT_UP")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Trigger_" + id + "_STAT_UP");
      }
    }

    if (SFSyst == top::topSFSyst::MU_SF_Trigger_STAT_DOWN) {
      if (x.isAvailable<float>(prefix+"_SF_Trigger_" + id + "_STAT_DOWN")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Trigger_" + id + "_STAT_DOWN");
      }
    }

    if (SFSyst == top::topSFSyst::MU_SF_Trigger_SYST_UP) {
      if (x.isAvailable<float>(prefix+"_SF_Trigger_" + id + "_SYST_UP")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Trigger_" + id + "_SYST_UP");
      }
    }

    if (SFSyst == top::topSFSyst::MU_SF_Trigger_SYST_DOWN) {
      if (x.isAvailable<float>(prefix+"_SF_Trigger_" + id + "_SYST_DOWN")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Trigger_" + id + "_SYST_DOWN");
      }
    }



    return sf;
  }

  float ScaleFactorRetriever::muonEff_Trigger(const xAOD::Muon& x,
                                              const top::topSFSyst SFSyst,
                                              bool isLoose) const {
    return muonEff_Trigger(x, (isLoose ? m_config->muonQualityLoose() : m_config->muonQuality()), SFSyst, isLoose);
  }

  float ScaleFactorRetriever::muonEff_Trigger(const xAOD::Muon& x,
                                              const std::string& id,
                                              const top::topSFSyst SFSyst,
                                              bool isLoose) const {
    float eff(1.);
    
    std::string prefix="MU";
    if(isLoose) prefix+="_LOOSE";

    if (x.isAvailable<float>(prefix+"_EFF_Trigger_" + id)) {
      eff = x.auxdataConst<float>(prefix+"_EFF_Trigger_" + id);
    }

    if (SFSyst == top::topSFSyst::MU_SF_Trigger_STAT_UP) {
      if (x.isAvailable<float>(prefix+"_EFF_Trigger_" + id + "_STAT_UP")) {
        eff = x.auxdataConst<float>(prefix+"_EFF_Trigger_" + id + "_STAT_UP");
      }
    }

    if (SFSyst == top::topSFSyst::MU_SF_Trigger_STAT_DOWN) {
      if (x.isAvailable<float>(prefix+"_EFF_Trigger_" + id + "_STAT_DOWN")) {
        eff = x.auxdataConst<float>(prefix+"_EFF_Trigger_" + id + "_STAT_DOWN");
      }
    }

    if (SFSyst == top::topSFSyst::MU_SF_Trigger_SYST_UP) {
      if (x.isAvailable<float>(prefix+"_EFF_Trigger_" + id + "_SYST_UP")) {
        eff = x.auxdataConst<float>(prefix+"_EFF_Trigger_" + id + "_SYST_UP");
      }
    }

    if (SFSyst == top::topSFSyst::MU_SF_Trigger_SYST_DOWN) {
      if (x.isAvailable<float>(prefix+"_EFF_Trigger_" + id + "_SYST_DOWN")) {
        eff = x.auxdataConst<float>(prefix+"_EFF_Trigger_" + id + "_SYST_DOWN");
      }
    }



    return eff;
  }

  float ScaleFactorRetriever::muonSF_ID(const xAOD::Muon& x,
                                        const top::topSFSyst SFSyst,
                                        bool isLoose) const {
    return muonSF_ID(x, (isLoose ? m_config->muonQualityLoose() : m_config->muonQuality()), SFSyst, isLoose);
  }

  float ScaleFactorRetriever::muonSF_ID(const xAOD::Muon& x,
                                        const std::string& id,
                                        const top::topSFSyst SFSyst,
                                        bool isLoose) const {
    std::string decoration = "MU_SF_ID_";
    if(isLoose) decoration = "MU_LOOSE_SF_ID_";
    decoration+=id;
    
    switch (SFSyst) {
    case top::topSFSyst::MU_SF_ID_STAT_UP:
      decoration += "_STAT_UP";
      break;

    case top::topSFSyst::MU_SF_ID_STAT_DOWN:
      decoration += "_STAT_DOWN";
      break;

    case top::topSFSyst::MU_SF_ID_SYST_UP:
      decoration += "_SYST_UP";
      break;

    case top::topSFSyst::MU_SF_ID_SYST_DOWN:
      decoration += "_SYST_DOWN";
      break;

    case top::topSFSyst::MU_SF_ID_STAT_LOWPT_UP:
      decoration += "_STAT_LOWPT_UP";
      break;

    case top::topSFSyst::MU_SF_ID_STAT_LOWPT_DOWN:
      decoration += "_STAT_LOWPT_DOWN";
      break;

    case top::topSFSyst::MU_SF_ID_SYST_LOWPT_UP:
      decoration += "_SYST_LOWPT_UP";
      break;

    case top::topSFSyst::MU_SF_ID_SYST_LOWPT_DOWN:
      decoration += "_SYST_LOWPT_DOWN";
      break;

    default:
      // Do nothing, we have the decoration already
      break;
    }

    if (!x.isAvailable<float>(decoration)) {
      return 1.0;
    } else {
      return x.auxdataConst<float>(decoration);
    }

    // This should never happen
    throw std::runtime_error("Something has gone wrong in mu ID SF retrieval");
  }

  float ScaleFactorRetriever::softmuonSF_ID(const xAOD::Muon& x,
                                            const top::topSFSyst SFSyst) const {
    return softmuonSF_ID(x, m_config->softmuonQuality(), SFSyst);
  }

  float ScaleFactorRetriever::softmuonSF_ID(const xAOD::Muon& x,
                                            const std::string& id,
                                            const top::topSFSyst SFSyst) const {
    std::string decoration = "SOFTMU_SF_ID_" + id;
    switch (SFSyst) {
    case top::topSFSyst::MU_SF_ID_STAT_UP:
      decoration += "_STAT_UP";
      break;

    case top::topSFSyst::MU_SF_ID_STAT_DOWN:
      decoration += "_STAT_DOWN";
      break;

    case top::topSFSyst::MU_SF_ID_SYST_UP:
      decoration += "_SYST_UP";
      break;

    case top::topSFSyst::MU_SF_ID_SYST_DOWN:
      decoration += "_SYST_DOWN";
      break;

    case top::topSFSyst::MU_SF_ID_STAT_LOWPT_UP:
      decoration += "_STAT_LOWPT_UP";
      break;

    case top::topSFSyst::MU_SF_ID_STAT_LOWPT_DOWN:
      decoration += "_STAT_LOWPT_DOWN";
      break;

    case top::topSFSyst::MU_SF_ID_SYST_LOWPT_UP:
      decoration += "_SYST_LOWPT_UP";
      break;

    case top::topSFSyst::MU_SF_ID_SYST_LOWPT_DOWN:
      decoration += "_SYST_LOWPT_DOWN";
      break;

    default:
      // Do nothing, we have the decoration already
      break;
    }

    return x.auxdataConst<float>(decoration);

    // This should never happen
    throw std::runtime_error("Something has gone wrong in mu ID SF retrieval");
  }

  float ScaleFactorRetriever::muonSF_Isol(const xAOD::Muon& x,
                                          const top::topSFSyst SFSyst,
                                          bool isLoose) const {
    return muonSF_Isol(x, (isLoose ? m_config->muonIsolationSFLoose() : m_config->muonIsolationSF()), SFSyst, isLoose);
  }

  float ScaleFactorRetriever::muonSF_Isol(const xAOD::Muon& x,
                                          const std::string& iso,
                                          const top::topSFSyst SFSyst,
                                          bool isLoose) const {
    float sf(1.);
    
    std::string prefix="MU";
    if(isLoose) prefix+="_LOOSE";

    if (x.isAvailable<float>(prefix+"_SF_Isol_" + iso)) {
      sf = x.auxdataConst<float>(prefix+"_SF_Isol_" + iso);
    }

    if (SFSyst == top::topSFSyst::MU_SF_Isol_SYST_UP) {
      if (x.isAvailable<float>(prefix+"_SF_Isol_" + iso + "_SYST_UP")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Isol_" + iso + "_SYST_UP");
      }
    }

    if (SFSyst == top::topSFSyst::MU_SF_Isol_SYST_DOWN) {
      if (x.isAvailable<float>(prefix+"_SF_Isol_" + iso + "_SYST_DOWN")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Isol_" + iso + "_SYST_DOWN");
      }
    }

    if (SFSyst == top::topSFSyst::MU_SF_Isol_STAT_UP) {
      if (x.isAvailable<float>(prefix+"_SF_Isol_" + iso + "_STAT_UP")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Isol_" + iso + "_STAT_UP");
      }
    }

    if (SFSyst == top::topSFSyst::MU_SF_Isol_STAT_DOWN) {
      if (x.isAvailable<float>(prefix+"_SF_Isol_" + iso + "_STAT_DOWN")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Isol_" + iso + "_STAT_DOWN");
      }
    }

    return sf;
  }

  /**
   * @brief Get the muon track-to-vertex association SF
   *
   * @param x - muon to get SF for
   * @param SFSyst - systematic variation required
   *
   * @return muon TTVA SF
   **/
  float ScaleFactorRetriever::muonSF_TTVA(const xAOD::Muon& x,
                                          const top::topSFSyst SFSyst) const {
    // Nominal decoration: if not a TTVA
    // systematic then return the nominal
    std::string decoration = "MU_SF_TTVA";
    
    switch (SFSyst) {
    case top::topSFSyst::MU_SF_TTVA_STAT_UP:
      decoration += "_STAT_UP";
      break;

    case top::topSFSyst::MU_SF_TTVA_STAT_DOWN:
      decoration += "_STAT_DOWN";
      break;

    case top::topSFSyst::MU_SF_TTVA_SYST_UP:
      decoration += "_SYST_UP";
      break;

    case top::topSFSyst::MU_SF_TTVA_SYST_DOWN:
      decoration += "_SYST_DOWN";
      break;

    default:
      // Do nothing, we have the decoration already
      break;
    }

    if (!(x.isAvailable<float>(decoration))) {
      ATH_MSG_INFO("Muon is not decorated with requested TTVA SF. 1.0 will be returned.");
      return 1.0;
    }

    return x.auxdataConst<float>(decoration);

    ;
  }

  float ScaleFactorRetriever::tauSF(const top::Event& event,
                                    const top::topSFSyst SFSyst) const {
    float sf(1.0);

    for (auto tau : event.m_tauJets)
      sf *= tauSF(*tau, SFSyst, event.m_isLoose);
    return sf;
  }

  float ScaleFactorRetriever::tauSF(const xAOD::TauJet& x,
                                    const top::topSFSyst SFSyst,
                                    bool isLoose) const {
    // See TauScaleFactorCalculator.cxx for uncertainties

    static std::map<TString, const SG::AuxElement::ConstAccessor<float> > acc_tauSFs;
    static std::map<TString, const SG::AuxElement::ConstAccessor<float> > acc_tauSFs_loose;

    if (!isLoose) {
      if (tauSF_name.find(SFSyst) == tauSF_name.end()) {
        if (acc_tauSFs.find(tauSF_name.at(top::topSFSyst::TAU_SF_NOMINAL)) == acc_tauSFs.end()) {
          acc_tauSFs.insert(std::pair<TString,
                                      const SG::AuxElement::ConstAccessor<float> > (tauSF_name.at(top::topSFSyst::
												  TAU_SF_NOMINAL),
										    tauSF_name.at(top::topSFSyst::
												  TAU_SF_NOMINAL).Data()));
        }
        return acc_tauSFs.at(tauSF_name.at(top::topSFSyst::TAU_SF_NOMINAL))(x);
      }
      if (acc_tauSFs.find(tauSF_name.at(SFSyst)) == acc_tauSFs.end()) {
        acc_tauSFs.insert(std::pair<TString, const SG::AuxElement::ConstAccessor<float> > (tauSF_name.at(SFSyst),
											   tauSF_name.at(SFSyst).Data()));
      }
      return acc_tauSFs.at(tauSF_name.at(SFSyst))(x);
    } else {
      if (tauSF_name.find(SFSyst) == tauSF_name.end()) {
        if (acc_tauSFs_loose.find(tauSF_name.at(top::topSFSyst::TAU_SF_NOMINAL)) == acc_tauSFs_loose.end()) {
          acc_tauSFs_loose.insert(std::pair<TString,
                                            const SG::AuxElement::ConstAccessor<float> > (tauSF_name.at(top::topSFSyst::
													TAU_SF_NOMINAL) +
          "_loose",
											  tauSF_name.at(top::topSFSyst::
													TAU_SF_NOMINAL).Data()));
        }
        return acc_tauSFs_loose.at(tauSF_name.at(top::topSFSyst::TAU_SF_NOMINAL) + "_loose")(x);
      }
      if (acc_tauSFs_loose.find((tauSF_name.at(SFSyst) + "_loose")) == acc_tauSFs_loose.end()) {
        acc_tauSFs_loose.insert(std::pair<TString,
                                          const SG::AuxElement::ConstAccessor<float> > (tauSF_name.at(SFSyst) + "_loose",
											(tauSF_name.at(SFSyst) +
											 "_loose").Data()));
      }
      return acc_tauSFs_loose.at((tauSF_name.at(SFSyst) + "_loose"))(x);
    }
  }

  float ScaleFactorRetriever::photonSF_Isol(const xAOD::Photon& x,
                                            const top::topSFSyst SFSyst,
                                            bool isLoose) const {
    return photonSF_Isol(x, (isLoose ? m_config->photonIsolationLoose() : m_config->photonIsolation()), SFSyst, isLoose);
  }

  float ScaleFactorRetriever::photonSF_Isol(const xAOD::Photon& x,
                                            const std::string& iso,
                                            const top::topSFSyst SFSyst,
                                            bool isLoose) const {
    float sf(1.);
    
    std::string prefix="PH";
    if(isLoose) prefix+="_LOOSE";

    if (x.isAvailable<float>(prefix+"_SF_Iso_" + iso)) {
      sf = x.auxdataConst<float>(prefix+"_SF_Iso_" + iso);
    }
    if (SFSyst == top::topSFSyst::PHOTON_EFF_ISO_UP) {
      if (x.isAvailable<float>(prefix+"_SF_Iso_" + iso + "_UP")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Iso_" + iso + "_UP");
      }
    }
    if (SFSyst == top::topSFSyst::PHOTON_EFF_ISO_DOWN) {
      if (x.isAvailable<float>(prefix+"_SF_Iso_" + iso + "_DOWN")) {
        sf = x.auxdataConst<float>(prefix+"_SF_Iso_" + iso + "_DOWN");
      }
    }
    return sf;
  }

  float ScaleFactorRetriever::photonSF(const top::Event& event,
                                       const top::topSFSyst SFSyst) const {
    float sf(1.);
    float reco(1.);
    float isol(1.);

    for (auto photon : event.m_photons) {
      reco *= photonSF_Reco(*photon, SFSyst);
      isol *= photonSF_Isol(*photon, SFSyst, event.m_isLoose);
    }

    sf = reco * isol;

    return sf;
  }

  float ScaleFactorRetriever::photonSF_Reco(const xAOD::Photon& photon,
					    const top::topSFSyst SFSyst) const {
    static const SG::AuxElement::ConstAccessor<float> acc_ph_IDSF("EFF_ID_SF");
    static const SG::AuxElement::ConstAccessor<float> acc_ph_IDSFUp("EFF_ID_SF_UP");
    static const SG::AuxElement::ConstAccessor<float> acc_ph_IDSFDown("EFF_ID_SF_DOWN");

    switch (SFSyst) {
    case top::topSFSyst::nominal:
      return acc_ph_IDSF(photon);

    case top::topSFSyst::PHOTON_IDSF_UP:
      return acc_ph_IDSFUp(photon);

    case top::topSFSyst::PHOTON_IDSF_DOWN:
      return acc_ph_IDSFDown(photon);

    default:
      return acc_ph_IDSF(photon);
    }
  }

  /**
   * @brief nominal SF or named systematics
   */
  float ScaleFactorRetriever::btagSF(const top::Event& event,
                                     const top::topSFSyst SFSyst,
                                     std::string WP, bool do_trackjets, std::string uncert_name) const {
    float sf(1.);

    std::string decoration = "btag_SF_" + WP + "_nom";
    std::string systematicName, bTagSystName;
    switch (SFSyst) {
    case top::topSFSyst::nominal:
      // If this is the nominal tree, we proceed as normal
      // If not nominal tree, we need to know which systematic this event corresponds to,
      // in case the systematic is removed from EV decomposition (will enter as a nominal retrieval)
      systematicName = m_config->systematicName(event.m_hashValue);
      bTagSystName = top::bTagNamedSystCheck(m_config, systematicName, WP, do_trackjets, false);
      if (bTagSystName != "") decoration = "btag_SF_" + WP + "_" + bTagSystName; // Only change decoration if found,
                                                                                 // otherwise we will use the nominal
      break;

    case top::topSFSyst::BTAG_SF_NAMED_UP:
      if (uncert_name == "") {
        ATH_MSG_INFO("Named b-tagging systematics should have a name. Please provide one.");
        return 0;
      }
      decoration = "btag_SF_" + WP + "_" + uncert_name + "__1up";
      break;

    case top::topSFSyst::BTAG_SF_NAMED_DOWN:
      if (uncert_name == "") {
        ATH_MSG_INFO("Named b-tagging systematics should have a name. Please provide one.");
        return 0;
      }
      decoration = "btag_SF_" + WP + "_" + uncert_name + "__1down";
      break;

    case top::topSFSyst::BTAG_SF_EIGEN_B:
    case top::topSFSyst::BTAG_SF_EIGEN_C:
    case top::topSFSyst::BTAG_SF_EIGEN_LIGHT:
      ATH_MSG_INFO("For Eigenvectors please use ScaleFactorRetriever::btagSF_eigen_vars");
      return 0;

      break;

    default:
      ATH_MSG_INFO("Not the right function: " << __PRETTY_FUNCTION__);
      return 0;

      break;
    }

    // I'm testing!
    xAOD::JetContainer jets = event.m_jets;
    if (do_trackjets) jets = event.m_trackJets;
    for (auto jetPtr : jets) {
      double weight = 1.0;
      if (jetPtr->isAvailable<float>(decoration)) weight = jetPtr->auxdataConst<float>(decoration);

      sf *= weight;
    }

    // for now
    return sf;
  }

  void ScaleFactorRetriever::btagSF_eigen_vars(const top::Event& event,
                                               const top::topSFSyst SFSyst,
                                               std::vector<float>& vec_btagSF_up,
                                               std::vector<float>& vec_btagSF_down,
                                               std::string WP, bool do_trackjets) const {
    // just in case
    vec_btagSF_up.clear();
    vec_btagSF_down.clear();

    unsigned int n_eigen = 0;
    std::string prefix = "btag_SF_" + WP + "_FT_EFF_Eigen_";
    std::string flav = "";

    switch (SFSyst) {
    case top::topSFSyst::BTAG_SF_EIGEN_B:
      n_eigen = do_trackjets ? m_config->trkjet_btagging_num_B_eigenvars(WP) : m_config->btagging_num_B_eigenvars(WP);
      flav = "B_";
      break;

    case top::topSFSyst::BTAG_SF_EIGEN_C:
      n_eigen = do_trackjets ? m_config->trkjet_btagging_num_C_eigenvars(WP) : m_config->btagging_num_C_eigenvars(WP);
      flav = "C_";
      break;

    case top::topSFSyst::BTAG_SF_EIGEN_LIGHT:
      n_eigen =
        do_trackjets ? m_config->trkjet_btagging_num_Light_eigenvars(WP) : m_config->btagging_num_Light_eigenvars(WP);
      flav = "Light_";
      break;

    default:
      ATH_MSG_INFO("Not the right function: " << __PRETTY_FUNCTION__);
      return;
    }
    vec_btagSF_up.resize(n_eigen);
    vec_btagSF_down.resize(n_eigen);

    for (unsigned int i = 0; i < n_eigen; ++i) {
      float SF_up(1.0), SF_down(1.0);
      std::string num = std::to_string(i);
      std::string SF_dec_up = prefix + flav + num + "__1up";
      std::string SF_dec_down = prefix + flav + num + "__1down";

      xAOD::JetContainer jets = event.m_jets;
      if (do_trackjets) jets = event.m_trackJets;
      for (auto jetPtr : jets) {
        if (jetPtr->isAvailable<float>(SF_dec_up)) SF_up *= jetPtr->auxdataConst<float>(SF_dec_up);
        if (jetPtr->isAvailable<float>(SF_dec_down)) SF_down *= jetPtr->auxdataConst<float>(SF_dec_down);
      }
      vec_btagSF_up[i] = SF_up;
      vec_btagSF_down[i] = SF_down;
    }
    return;
  }

  float ScaleFactorRetriever::jvtSF(const top::Event& event,
                                    const top::topSFSyst SFSyst) const {
    xAOD::JetContainer jets = event.m_jets;
    switch (SFSyst) {
    case top::topSFSyst::JVT_UP:
      return event.m_jvtSF_UP;

    case top::topSFSyst::JVT_DOWN:
      return event.m_jvtSF_DOWN;

    default:
      return event.m_jvtSF;
    }
  }

  float ScaleFactorRetriever::fjvtSF(const top::Event& event,
                                    const top::topSFSyst SFSyst) const {
    xAOD::JetContainer jets = event.m_jets;
    switch (SFSyst) {
    case top::topSFSyst::FJVT_UP:
      return event.m_fjvtSF_UP;

    case top::topSFSyst::FJVT_DOWN:
      return event.m_fjvtSF_DOWN;

    default:
      return event.m_fjvtSF;
    }
  }

  /**
   * @brief Print all the SF values to msg stream
   */
  void ScaleFactorRetriever::print(const top::Event& event) {
    ATH_MSG_INFO("ScaleFactors");
    ATH_MSG_INFO("    MCEventWeight      : " <<
      std::to_string(event.m_info->auxdataConst<float>("AnalysisTop_eventWeight")));
    ATH_MSG_INFO("    Pileup             : " << std::to_string(pileupSF(event)));
    ATH_MSG_INFO("    LeptonEventWeight  : " << std::to_string(leptonSF(event, top::topSFSyst::nominal)));
    ATH_MSG_INFO("    B-TagEventWeight   : " << std::to_string(btagSF(event, top::topSFSyst::nominal)));
  }
}  // namespace top
