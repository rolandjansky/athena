/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef SCALEFACTORRETRIEVER_H_
#define SCALEFACTORRETRIEVER_H_

#include <memory>
#include <string>
#include <vector>

#include "AsgTools/AsgTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "AthContainers/AuxElement.h"

// Forward declare:
namespace top {
  class Event;
  class TopConfig;
}


namespace top {
///-- Top SF enmu, as CP::SystematicSet won't do the job --///
  enum topSFSyst {
    nominal = 0,
    // Electron Trigger SFs
    EL_SF_Trigger_UP, EL_SF_Trigger_DOWN,
    // Electron reconstruction SFs
    EL_SF_Reco_UP, EL_SF_Reco_DOWN,
    // Electron ID SFs
    EL_SF_ID_UP, EL_SF_ID_DOWN,
    // Electron isolation SFs
    EL_SF_Isol_UP, EL_SF_Isol_DOWN,
    // Electron Charge flip SFs
    EL_SF_ChargeID_UP, EL_SF_ChargeID_DOWN,
    EL_SF_ChargeMisID_STAT_UP, EL_SF_ChargeMisID_STAT_DOWN,
    EL_SF_ChargeMisID_SYST_UP, EL_SF_ChargeMisID_SYST_DOWN,
    // Muon Trigger SFs
    MU_SF_Trigger_UP, MU_SF_Trigger_DOWN,
    MU_SF_Trigger_STAT_UP, MU_SF_Trigger_STAT_DOWN,
    MU_SF_Trigger_SYST_UP, MU_SF_Trigger_SYST_DOWN,
    // Muon ID SFs (regular)
    MU_SF_ID_STAT_UP, MU_SF_ID_STAT_DOWN,
    MU_SF_ID_SYST_UP, MU_SF_ID_SYST_DOWN,
    // Muon ID SFs (low pT)
    MU_SF_ID_STAT_LOWPT_UP, MU_SF_ID_STAT_LOWPT_DOWN,
    MU_SF_ID_SYST_LOWPT_UP, MU_SF_ID_SYST_LOWPT_DOWN,
    // Muon isolation SFs
    MU_SF_Isol_STAT_UP, MU_SF_Isol_STAT_DOWN,
    MU_SF_Isol_SYST_UP, MU_SF_Isol_SYST_DOWN,
    // Muon TTVA SFs
    MU_SF_TTVA_STAT_UP, MU_SF_TTVA_STAT_DOWN,
    MU_SF_TTVA_SYST_UP, MU_SF_TTVA_SYST_DOWN,

    TAU_SF_NOMINAL,
    // Tau EleOLR SFs
    TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL__1up, TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL__1down,
    TAUS_TRUEELECTRON_EFF_ELEOLR_STAT__1up, TAUS_TRUEELECTRON_EFF_ELEOLR_STAT__1down,
    TAUS_TRUEELECTRON_EFF_ELEOLR_SYST__1up, TAUS_TRUEELECTRON_EFF_ELEOLR_SYST__1down,
    // Tau RNNID SFs
    TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2025__1up, TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2025__1down,
    TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2530__1up, TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2530__1down,
    TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT3040__1up, TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT3040__1down,
    TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPTGE40__1up, TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPTGE40__1down,
	  TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2025__1up, TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2025__1down,
	  TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2530__1up, TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2530__1down,
	  TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT3040__1up, TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT3040__1down,
	  TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPTGE40__1up, TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPTGE40__1down,
    TAUS_TRUEHADTAU_EFF_RNNID_HIGHPT__1up, TAUS_TRUEHADTAU_EFF_RNNID_HIGHPT__1down,
    TAUS_TRUEHADTAU_EFF_RNNID_SYST__1up, TAUS_TRUEHADTAU_EFF_RNNID_SYST__1down,

    // Tau BDTID SFs
    TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETAHIGH__1up, TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETAHIGH__1down,
    TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETALOW__1up, TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETALOW__1down,
    TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETAHIGH__1up, TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETAHIGH__1down,
    TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETALOW__1up, TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETALOW__1down,
    TAUS_TRUEHADTAU_EFF_JETID_SYST__1up,
	  TAUS_TRUEHADTAU_EFF_JETID_SYST__1down,
    TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up,
	  TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1down,
    // Tau Reconstruction SFs
    TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up, TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1down,
   TAU_SF_END,
    // Photon SFs
    PHOTON_IDSF_UP, PHOTON_IDSF_DOWN,
    PHOTON_EFF_ISO_UP, PHOTON_EFF_ISO_DOWN,
    // B-tagging SFs
    BTAG_SF_EIGEN_B, BTAG_SF_EIGEN_C,
    BTAG_SF_EIGEN_LIGHT,
    BTAG_SF_NAMED_UP, BTAG_SF_NAMED_DOWN,
    // JVT SFs
    JVT_UP, JVT_DOWN,
    // fJVT SFs
    FJVT_UP, FJVT_DOWN,
    // FWD Electron ID SFs
    FWDEL_SF_ID_UP, FWDEL_SF_ID_DOWN,

    lastsys
  };
  const std::map<topSFSyst, TString> tauSF_name = {
    {TAU_SF_NOMINAL, "TAU_SF_NOMINAL"},
    {TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up, "TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up"},
    {TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1down, "TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1down"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2025__1up,
     "TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2025__1up"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2025__1down,
     "TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2025__1down"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2530__1up,
     "TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2530__1up"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2530__1down,
     "TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2530__1down"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT3040__1up,
     "TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT3040__1up"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT3040__1down,
      "TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT3040__1down"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPTGE40__1up,
       "TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPTGE40__1up"},
	  {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPTGE40__1down,
	   "TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPTGE40__1down"},
	  {TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETAHIGH__1up, "TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETAHIGH__1up"},
    {TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETAHIGH__1down, "TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETAHIGH__1down"},
    {TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETALOW__1up, "TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETALOW__1up"},
    {TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETALOW__1down, "TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETALOW__1down"},
    {TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETAHIGH__1up, "TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETAHIGH__1up"},
    {TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETAHIGH__1down, "TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETAHIGH__1down"},
    {TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETALOW__1up, "TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETALOW__1up"},
    {TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETALOW__1down, "TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETALOW__1down"},
    {TAUS_TRUEHADTAU_EFF_JETID_SYST__1up, "TAUS_TRUEHADTAU_EFF_JETID_SYST__1up"},
	  {TAUS_TRUEHADTAU_EFF_JETID_SYST__1down, "TAUS_TRUEHADTAU_EFF_JETID_SYST__1down"},
    {TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up, "TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up"},
	  {TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1down, "TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1down"},
    {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2025__1up,
     "TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2025__1up"},
    {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2025__1down,
     "TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2025__1down"},
    {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2530__1up,
     "TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2530__1up"},
    {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2530__1down,
     "TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2530__1down"},
    {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT3040__1up,
     "TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT3040__1up"},
    {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT3040__1down,
      "TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT3040__1down"},
    {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPTGE40__1up,
       "TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPTGE40__1up"},
	  {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPTGE40__1down,
	   "TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPTGE40__1down"},
    {TAUS_TRUEHADTAU_EFF_RNNID_HIGHPT__1up, "TAUS_TRUEHADTAU_EFF_RNNID_HIGHPT__1up"},
    {TAUS_TRUEHADTAU_EFF_RNNID_HIGHPT__1down, "TAUS_TRUEHADTAU_EFF_RNNID_HIGHPT__1down"},
    {TAUS_TRUEHADTAU_EFF_RNNID_SYST__1up, "TAUS_TRUEHADTAU_EFF_RNNID_SYST__1up"},
    {TAUS_TRUEHADTAU_EFF_RNNID_SYST__1down, "TAUS_TRUEHADTAU_EFF_RNNID_SYST__1down"},
    {TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL__1up, "TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL__1up"},
    {TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL__1down, "TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL__1down"},
    {TAUS_TRUEELECTRON_EFF_ELEOLR_STAT__1up, "TAUS_TRUEELECTRON_EFF_ELEOLR_STAT__1up"},
    {TAUS_TRUEELECTRON_EFF_ELEOLR_STAT__1down, "TAUS_TRUEELECTRON_EFF_ELEOLR_STAT__1down"},
    {TAUS_TRUEELECTRON_EFF_ELEOLR_SYST__1up, "TAUS_TRUEELECTRON_EFF_ELEOLR_SYST__1up"},
    {TAUS_TRUEELECTRON_EFF_ELEOLR_SYST__1down, "TAUS_TRUEELECTRON_EFF_ELEOLR_SYST__1down"},
  };

  const std::map<topSFSyst, TString> tauSF_alias = {
    {TAU_SF_NOMINAL, "TAU_SF_NOMINAL"},
    {TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up, "EFF_RECO_TOTAL_UP"},
    {TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1down, "EFF_RECO_TOTAL_DOWN"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2025__1up, "EFF_RNNID_1PRONGSTATSYSTPT2025_UP"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2025__1down, "EFF_RNNID_1PRONGSTATSYSTPT2025_DOWN"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2530__1up, "EFF_RNNID_1PRONGSTATSYSTPT2530_UP"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT2530__1down, "EFF_RNNID_1PRONGSTATSYSTPT2530_DOWN"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT3040__1up, "EFF_RNNID_1PRONGSTATSYSTPT3040_UP"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPT3040__1down, "EFF_RNNID_1PRONGSTATSYSTPT3040_DOWN"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPTGE40__1up, "EFF_RNNID_1PRONGSTATSYSTPTGE40_UP"},
    {TAUS_TRUEHADTAU_EFF_RNNID_1PRONGSTATSYSTPTGE40__1down, "EFF_RNNID_1PRONGSTATSYSTPTGE40_DOWN"},
	  {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2025__1up, "EFF_RNNID_1PRONGSTATSYSTPT2025_UP"},
	  {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2025__1down, "EFF_RNNID_3PRONGSTATSYSTPT2025_DOWN"},
	  {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2530__1up, "EFF_RNNID_3PRONGSTATSYSTPT2530_UP"},
	  {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT2530__1down, "EFF_RNNID_3PRONGSTATSYSTPT2530_DOWN"},
	  {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT3040__1up, "EFF_RNNID_3PRONGSTATSYSTPT3040_UP"},
	  {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPT3040__1down, "EFF_RNNID_3PRONGSTATSYSTPT3040_DOWN"},
	  {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPTGE40__1up, "EFF_RNNID_3PRONGSTATSYSTPTGE40_UP"},
	  {TAUS_TRUEHADTAU_EFF_RNNID_3PRONGSTATSYSTPTGE40__1down, "EFF_RNNID_3PRONGSTATSYSTPTGE40_DOWN"},
    {TAUS_TRUEHADTAU_EFF_RNNID_HIGHPT__1up, "EFF_RNNID_HIGHPT_UP"},
    {TAUS_TRUEHADTAU_EFF_RNNID_HIGHPT__1down, "EFF_RNNID_HIGHPT_DOWN"},
    {TAUS_TRUEHADTAU_EFF_RNNID_SYST__1up, "EFF_RNNID_SYST_UP"},
    {TAUS_TRUEHADTAU_EFF_RNNID_SYST__1down, "EFF_RNNID_SYST_DOWN"},
    {TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETAHIGH__1up, "EFF_JETID_1PETAHIGH_UP"},
    {TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETAHIGH__1down, "EFF_JETID_1PETAHIGH_DOWN"},
    {TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETALOW__1up, "EFF_JETID_1PETALOW_UP"},
    {TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTETALOW__1down, "EFF_JETID_1PETALOW_DOWN"},
    {TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETAHIGH__1up, "EFF_JETID_3PETAHIGH_UP"},
    {TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETAHIGH__1down, "EFF_JETID_3PETAHIGH_DOWN"},
    {TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETALOW__1up, "EFF_JETID_3PETALOW_UP"},
    {TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTETALOW__1down, "EFF_JETID_3PETALOW_DOWN"},
    {TAUS_TRUEHADTAU_EFF_JETID_SYST__1up, "EFF_JETID_SYST_UP"},
	  {TAUS_TRUEHADTAU_EFF_JETID_SYST__1down, "EFF_JETID_SYST_DOWN"},
    {TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up, "EFF_JETID_HIGHPT_UP"},
    {TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1down, "EFF_JETID_HIGHPT_DOWN"},
    {TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL__1up, "EFF_ELEOLR_TOTAL_UP"},
    {TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL__1down, "EFF_ELEOLR_TOTAL_DOWN"},
    {TAUS_TRUEELECTRON_EFF_ELEOLR_STAT__1up, "ELE_EFF_ELEOLR_STAT_UP"},
    {TAUS_TRUEELECTRON_EFF_ELEOLR_STAT__1down, "ELE_EFF_ELEOLR_STAT_DOWN"},
    {TAUS_TRUEELECTRON_EFF_ELEOLR_SYST__1up, "ELE_EFF_ELEOLR_SYST_UP"},
    {TAUS_TRUEELECTRON_EFF_ELEOLR_SYST__1down, "ELE_EFF_ELEOLR_SYST_DOWN"},
  };


  enum topSFComp {
    ALL = 0, TRIGGER, RECO, ID, ISOLATION, CHARGEID, CHARGEMISID, TTVA
  };

  class ScaleFactorRetriever final: public asg::AsgTool {
    // Specify that we will overload print with top::Event later on
    using asg::AsgTool::print;
  public:
    explicit ScaleFactorRetriever(const std::string& name);
    virtual ~ScaleFactorRetriever() {}

    // Delete Standard constructors
    ScaleFactorRetriever(const ScaleFactorRetriever& rhs) = delete;
    ScaleFactorRetriever(ScaleFactorRetriever&& rhs) = delete;
    ScaleFactorRetriever& operator = (const ScaleFactorRetriever& rhs) = delete;

    StatusCode initialize();

    // Pile up SF
    static bool hasPileupSF(const top::Event& event);
    static float pileupSF(const top::Event& event, int var = 0);

    // Obtain the lepton SF
    float leptonSF(const top::Event& event, const top::topSFSyst SFSyst) const;

    // Obtain the trigger SF
    float triggerSF(const top::Event& event, const top::topSFSyst SFSyst) const;

    // Obtain trigger SF via global tool
    float globalTriggerSF(const top::Event& event, const top::topSFSyst SFSyst) const;

    // Obtain the trigger SF via approximate formula
    float oldTriggerSF(const top::Event& event, const top::topSFSyst SFSyst) const;

    // Obtain the electron SF
    float electronSF(const top::Event& event,
                     const top::topSFSyst SFSyst,
                     const top::topSFComp SFComp) const;

    //get vector of systematic variations of electron SFs, var=1 for up, var=-1 for down
    std::vector<float> electronSFSystVariationVector(const top::Event& event, const top::topSFComp SFComp, int var = 1) const;

    // Obtain the fwd electron SF
    float fwdElectronSF(const top::Event& event,
                        const top::topSFSyst SFSyst,
                        const top::topSFComp SFComp = top::topSFComp::ALL) const;

    // Obtain the muon SF
    float muonSF(const top::Event& event,
                 const top::topSFSyst SFSyst,
                 const top::topSFComp SFComp) const;

    // Obtain the soft muon SF
    float softmuonSF(const top::Event& event,
                     const top::topSFSyst SFSyst,
                     const top::topSFComp SFComp) const;

    /**
     * @brief: get the tauSF for all taus in event
     * @param: event, a top::Event object
     * @param: SFSyst, systematic variation enum
     */
    float tauSF(const top::Event& event,
                const top::topSFSyst SFSyst) const;

    /**
     * @brief: get the photonSF for all photons in event
     * @param: event, a top::Event object
     * @param: SFSyst, systematic variation enum
     */
    float photonSF(const top::Event& event,
                   const top::topSFSyst SFSyst) const;

    /**
     * @brief Missing important functionality, but testing...
     */
    float btagSF(const top::Event& event,
                 const top::topSFSyst SFSyst = top::topSFSyst::nominal,
                 std::string WP = "FixedCutBEff_77",
                 bool do_trackjets = false,
                 std::string uncert_name = "") const;

    void btagSF_eigen_vars(const top::Event& event,
                           const top::topSFSyst SFSyst,
                           std::vector<float>& btagSF_up,
                           std::vector<float>& btagSF_down,
                           std::string WP = "FixedCutBEff_77",
                           bool do_trackjets = false) const;

    /**
     *
     * JVT Efficiency SFs
     * @brief: Get the total JVT SF by taking the product of SFs for all jets
     *
     * @param: event, a top::Event object
     * @param: SFSyst, systematic variation enum
     *
     * @return: prod_jvt, the product of the jvt SFs for selected jets
     *
     **/
    float jvtSF(const top::Event& event,
                const top::topSFSyst SFsyst) const;

    /**
     *
     * fJVT Efficiency SFs
     * @brief: Get the total fJVT SF by taking the product of SFs for all jets, as of AnalysisBase124 by default central jets get a SF value of 1
     *
     * @param: event, a top::Event object
     * @param: SFSyst, systematic variation enum
     *
     * @return: prod_fjvt, the product of the fjvt SFs for selected jets
     *
     **/
    float fjvtSF(const top::Event& event,
                const top::topSFSyst SFsyst) const;

    /**
     * @brief Print all the SF values to msg stream
     */
    void print(const top::Event& event);

    float electronSF_Reco(const xAOD::Electron& x,
                          const top::topSFSyst SFSyst,
                          bool isLoose) const;

    float electronSF_Trigger(const xAOD::Electron& x, const top::topSFSyst SFSyst, bool isLoose) const;

    float electronEff_Trigger(const xAOD::Electron& x, const top::topSFSyst SFSyst, bool isLoose) const;

    float electronSF_ID(const xAOD::Electron& x,
                        const top::topSFSyst SFSyst,
                        bool isLoose) const;

    float electronSF_Isol(const xAOD::Electron& x,
                          const top::topSFSyst SFSyst,
                          bool isLoose) const;

    float electronSF_ChargeID(const xAOD::Electron& x,
                              const top::topSFSyst SFSyst,
                              bool isLoose) const;

    float electronSF_ChargeMisID(const xAOD::Electron& x,
                                 const top::topSFSyst SFSyst,
                                 bool isLoose) const;

    float fwdElectronSF_ID(const xAOD::Electron& x,
                           const top::topSFSyst SFSyst,
                           bool isLoose) const;

    float muonSF_Trigger(const xAOD::Muon& x,
                         const top::topSFSyst SFSyst,
                         bool isLoose) const;

    float muonEff_Trigger(const xAOD::Muon& x,
                          const top::topSFSyst SFSyst,
                          bool isLoose) const;

    float muonSF_ID(const xAOD::Muon& x,
                    const top::topSFSyst SFSyst,
                    bool isLoose) const;

    float muonSF_Isol(const xAOD::Muon& x,
                      const top::topSFSyst SFSyst,
                      bool isLoose) const;

    float muonSF_TTVA(const xAOD::Muon& x,
                      const top::topSFSyst SFSyst) const;

    float softmuonSF_ID(const xAOD::Muon& x,
                        const top::topSFSyst SFSyst) const;

    /**
     * @brief: get the tauSF for a single tau
     * @param: x, an xAOD::TauJet object
     * @param: SFSyst, systematic variation enum
     * @param: whether you want the SF for loose event or not
     */
    float tauSF(const xAOD::TauJet& x,
                const top::topSFSyst SFSyst,
                bool isLoose) const;

    /**
     * @brief: get the photonSF for a single photon
     * @param: x, an xAOD::Photon object
     * @param: SFSyst, systematic variation enum
     */
    float photonSF_Reco(const xAOD::Photon& x,
			const top::topSFSyst SFSyst) const;

    float photonSF_Isol(const xAOD::Photon& x,
                        const top::topSFSyst SFSyst,
                        bool isLoose) const;
  private:
    float electronSF_Trigger(const xAOD::Electron& x,
                             const std::string& id,
                             const top::topSFSyst SFSyst,
                             bool isLoose) const;

    float electronEff_Trigger(const xAOD::Electron& x,
                              const std::string& id,
                              const top::topSFSyst SFSyst,
                              bool isLoose) const;

    float electronSF_ID(const xAOD::Electron& x,
                        const std::string& id,
                        const top::topSFSyst SFSyst,
                        bool isLoose) const;

    float electronSF_Isol(const xAOD::Electron& x,
                          const std::string& id,
                          const top::topSFSyst SFSyst,
                          bool isLoose) const;

    float electronSF_ChargeID(const xAOD::Electron& x,
                              const std::string& id, const std::string& iso,
                              const top::topSFSyst SFSyst,
                              bool isLoose) const;

    float electronSF_ChargeMisID(const xAOD::Electron& x,
                                 const std::string& id, const std::string& iso,
                                 const top::topSFSyst SFSyst,
                                 bool isLoose) const;

    float fwdElectronSF_ID(const xAOD::Electron& x,
                           const std::string& id,
                           const top::topSFSyst SFSyst,
                           bool isLoose) const;

    float photonSF_Isol(const xAOD::Photon& x,
                        const std::string& iso,
                        const top::topSFSyst SFSyst,
                        bool isLoose) const;

    float muonSF_Trigger(const xAOD::Muon& x,
                         const std::string& id,
                         const top::topSFSyst SFSyst,
                         bool isLoose) const;
    float muonEff_Trigger(const xAOD::Muon& x,
                          const std::string& id,
                          const top::topSFSyst SFSyst,
                          bool isLoose) const;

    float muonSF_ID(const xAOD::Muon& x,
                    const std::string& id,
                    const top::topSFSyst SFSyst,
                    bool isLoose) const;

    float softmuonSF_ID(const xAOD::Muon& x,
                        const std::string& id,
                        const top::topSFSyst SFSyst) const;

    float muonSF_Isol(const xAOD::Muon& x,
                      const std::string& iso,
                      const top::topSFSyst SFSyst,
                      bool isLoose) const;

    // List of triggers to 'or' together for each event.
    // If any one passes, the event passes
    std::vector<std::string> m_electronTriggers_Tight;
    std::vector<std::string> m_muonTriggers_Tight;
    std::vector<std::string> m_electronTriggers_Loose;
    std::vector<std::string> m_muonTriggers_Loose;
    // Do we need to add tau triggers?

    // Configuration
    std::shared_ptr<top::TopConfig> m_config;
    bool m_preferGlobalTriggerSF;
  };
}  // namespace top

#endif  // SCALEFACTORRETRIEVER_H_
