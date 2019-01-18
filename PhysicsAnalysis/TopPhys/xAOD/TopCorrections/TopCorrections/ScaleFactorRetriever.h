/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
enum topSFSyst{nominal = 0,
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
               // Tau EleOLR SFs
               TAU_SF_ELEOLR_TOTAL_DOWN, TAU_SF_ELEOLR_TOTAL_UP,
               TAU_SF_TRUEELECTRON_ELEOLR_HIGHMU_DOWN, TAU_SF_TRUEELECTRON_ELEOLR_HIGHMU_UP,
               TAU_SF_TRUEELECTRON_ELEOLR_LOWMU_DOWN, TAU_SF_TRUEELECTRON_ELEOLR_LOWMU_UP,
               TAU_SF_TRUEELECTRON_ELEOLR_SYST_DOWN, TAU_SF_TRUEELECTRON_ELEOLR_SYST_UP,
               TAU_SF_TRUEELECTRON_ELEBDT_MC16A_DOWN, TAU_SF_TRUEELECTRON_ELEBDT_MC16A_UP,
               TAU_SF_TRUEELECTRON_ELEBDT_MC16D_DOWN, TAU_SF_TRUEELECTRON_ELEBDT_MC16D_UP,
               TAU_SF_TRUEELECTRON_ELEBDT_SYST_DOWN, TAU_SF_TRUEELECTRON_ELEBDT_SYST_UP,
               // Tau JetID SFs
               TAU_SF_JETID_1P2025_DOWN, TAU_SF_JETID_1P2025_UP,
               TAU_SF_JETID_1P2530_DOWN, TAU_SF_JETID_1P2530_UP,
               TAU_SF_JETID_1P3040_DOWN, TAU_SF_JETID_1P3040_UP,
               TAU_SF_JETID_1PGE40_DOWN, TAU_SF_JETID_1PGE40_UP,
               TAU_SF_JETID_3P2030_DOWN, TAU_SF_JETID_3P2030_UP,
               TAU_SF_JETID_3PGE30_DOWN, TAU_SF_JETID_3PGE30_UP,
               TAU_SF_JETID_HIGHPT_DOWN, TAU_SF_JETID_HIGHPT_UP,
               TAU_SF_JETID_SYST_DOWN, TAU_SF_JETID_SYST_UP,
               TAU_SF_JETID_AF2_DOWN, TAU_SF_JETID_AF2_UP,
               // Tau Reconstruction SFs
               TAU_SF_RECO_TOTAL_DOWN, TAU_SF_RECO_TOTAL_UP,
               TAU_SF_RECO_HIGHPT_DOWN, TAU_SF_RECO_HIGHPT_UP,
               TAU_SF_RECO_AF2_DOWN, TAU_SF_RECO_AF2_UP,
               // Photon SFs
               PHOTON_IDSF_UP, PHOTON_IDSF_DOWN, PHOTON_EFF_ISO,
               PHOTON_EFF_LOWPTISO_UP, PHOTON_EFF_TRKISO_UP,
               PHOTON_EFF_LOWPTISO_DOWN, PHOTON_EFF_TRKISO_DOWN,
               // B-tagging SFs
               BTAG_SF_EIGEN_B, BTAG_SF_EIGEN_C,
               BTAG_SF_EIGEN_LIGHT,
               BTAG_SF_NAMED_UP, BTAG_SF_NAMED_DOWN,
               // JVT SFs
               JVT_UP, JVT_DOWN,

               lastsys
};


enum topSFComp{ALL = 0, TRIGGER, RECO, ID, ISOLATION, CHARGEID, CHARGEMISID, TTVA};

class ScaleFactorRetriever final : public asg::AsgTool{
  // Specify that we will overload print with top::Event later on
  using asg::AsgTool::print;
 public:
  explicit ScaleFactorRetriever(const std::string& name);
  virtual ~ScaleFactorRetriever() {}

  // Delete Standard constructors
  ScaleFactorRetriever(const ScaleFactorRetriever& rhs) = delete;
  ScaleFactorRetriever(ScaleFactorRetriever&& rhs) = delete;
  ScaleFactorRetriever& operator=(const ScaleFactorRetriever& rhs) = delete;

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

  // Obtain the muon SF
  float muonSF(const top::Event& event,
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
   * @brief Print all the SF values to cout
   */
  void print(const top::Event& event);

  float electronSF_Reco(const xAOD::Electron& x,
                        const top::topSFSyst SFSyst) const;

  float electronSF_Trigger(const xAOD::Electron& x, const top::topSFSyst SFSyst, bool isLoose) const;

  float electronEff_Trigger(const xAOD::Electron& x, const top::topSFSyst SFSyst,  bool isLoose) const ;

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
   * @param: whether you want the SF for loose event or not
   */
  float photonSF(const xAOD::Photon& x,
                 const top::topSFSyst SFSyst,
                 bool isLoose) const;

  float photonSF_Isol(const xAOD::Photon& x,
                      const top::topSFSyst SFSyst,
                      bool isLoose) const;

 private:
  float electronSF_Trigger(const xAOD::Electron& x,
                           const std::string& id,
                           const top::topSFSyst SFSyst) const;

  float electronEff_Trigger(const xAOD::Electron& x,
			    const std::string& id,
			    const top::topSFSyst SFSyst) const;

  float electronSF_ID(const xAOD::Electron& x,
                      const std::string& id,
                      const top::topSFSyst SFSyst) const;

  float electronSF_Isol(const xAOD::Electron& x,
                        const std::string& id,
                        const top::topSFSyst SFSyst) const;

  float electronSF_ChargeID(const xAOD::Electron& x,
			    const std::string& id, const std::string& iso,
			    const top::topSFSyst SFSyst) const;

  float electronSF_ChargeMisID(const xAOD::Electron& x,
			       const std::string& id, const std::string& iso,
			       const top::topSFSyst SFSyst) const;

  float photonSF_Isol(const xAOD::Photon& x,
		      const std::string& iso,
		      const top::topSFSyst SFSyst) const;

  float muonSF_Trigger(const xAOD::Muon& x,
                       const std::string& id,
                       const top::topSFSyst SFSyst) const;
  float muonEff_Trigger(const xAOD::Muon& x,
                       const std::string& id,
                       const top::topSFSyst SFSyst) const;

  float muonSF_ID(const xAOD::Muon& x,
                  const std::string& id,
                  const top::topSFSyst SFSyst) const;

  float muonSF_Isol(const xAOD::Muon& x,
                    const std::string& iso,
                    const top::topSFSyst SFSyst) const;

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
