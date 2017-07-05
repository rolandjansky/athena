/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSAVERFLATNTUPLE_H_
#define EVENTSAVERFLATNTUPLE_H_

#include "TopAnalysis/EventSaverBase.h"
#include "TopCorrections/ScaleFactorRetriever.h"
#include "TopEventSelectionTools/TreeManager.h"
#include "TopObjectSelectionTools/RCJetMC15.h"

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"
#include <unordered_map>

namespace top {

// fwd declare upgrade class
class UpgradeEvent;

/**
 * @brief Some code that writes out a 'standard' flat ntuple.  This can be
 * extended if the user wants to inherit from this and implement the functions
 * themselves.
 */
class EventSaverFlatNtuple : public top::EventSaverBase , public asg::AsgTool  {

public:
    /**
     * @brief A default constructor so that this class can be created at
     * run-time by ROOT.
     */
    EventSaverFlatNtuple();

    /**
     * @brief Does nothing.
     */
    virtual ~EventSaverFlatNtuple();

    /**
     * @brief initialise is run before the event loop.
     *
     * @param config The configuration object (not used by the default flat
     * ntuple).
     * @param file The output file is used to put the TTrees in.
     * @param extraBranches Extra info requested (like which selections are
     * passed).
     */
    virtual void initialize(std::shared_ptr<top::TopConfig> config, TFile* file, const std::vector<std::string>& extraBranches);

    //Keep the asg::AsgTool happy
    virtual StatusCode initialize(){return StatusCode::SUCCESS;}

    /**
     * @brief Run for every event (actually every systematic for every event).
     *
     * This does the tree filling work.
     *
     * @param event The top::Event which has had object selection and overlap
     * removal (if requested) applied to it.
     */
    virtual void saveEvent(const top::Event& event);

    /**
     * @breif Run for every event
     *
     * This fills, if requested, the:
     *   MC truth record
     *   PDF info
     *   TopPartons
     */
    virtual void saveTruthEvent();

    /*!
     * @brief Store the particle level event's content in the particle level
     * TTree as flat n-tuple.
     *
     * This function shall be executed once for each event which is intended for
     * storage. It writes out the pass/fail results of all EventSelection chains
     * that have been applied to that specific event object.
     *
     * @param plEvent The particle level event whose data content will be
     * written to the output.
     */
    virtual void saveParticleLevelEvent(const top::ParticleLevelEvent& plEvent);

    /*!
     * @brief Store the upgrade event's content in the upgrade
     * TTree as flat n-tuple.
     *
     * This function shall be executed once for each event which is intended for
     * storage. It writes out the pass/fail results of all EventSelection chains
     * that have been applied to that specific event object.
     *
     * @param upgradeEvent The upgrade event whose data content will be
     * written to the output.
     */
    virtual void saveUpgradeEvent(const top::ParticleLevelEvent& plEvent);

    /**
     * @brief Not used by the flat ntuple code yet, but needed by the xAOD code.
     */
    virtual void finalize();

    /**
     * @brief shorten name of b-tagging working point (FixedCutBEff_*)
     */
    std::string shortBtagWP(std::string const & WP) const;

protected:
    /**
     * @brief Allow child classes to access the configuration object.
     */
    std::shared_ptr<top::TopConfig> topConfig();

    /**
     * @brief Allow classes that build on top of this to access the tree managers.
     *
     * Why not just make the variable that contains them protected? Because of
     * http://programmers.stackexchange.com/questions/162643/why-is-clean-code-suggesting-avoiding-protected-variables
     *
     * @return A vector of the treeManagers (one for each systematic).
     */
    std::vector<std::shared_ptr<top::TreeManager>> treeManagers();
    std::shared_ptr<top::TreeManager> truthTreeManager();

    /*!
     * @brief Return a shared pointer to the top::TreeManager object that is
     * used for the particle level output tree.
     * @returns A shared pointer to the top::TreeManager object that is used to
     * write out the particle level event data.
     */
    std::shared_ptr<top::TreeManager> particleLevelTreeManager();


   /**
    * @brief Return a shared pointer to the top::ScaleFactorRetriever object
    * which allows us to easily access the SF decorations
    * @returns A shared pointer to the top::ScaleFactorRetriever object
    */
    std::shared_ptr<top::ScaleFactorRetriever> scaleFactorRetriever();

    /*!
     * @brief Function to access the branch filters - cf ANALYSISTO-61
     * @returns A shared pointer to the m_branchFilters object
     */
    std::vector<top::TreeManager::BranchFilter> & branchFilters();

    /*!
     * @brief Internal function which configures the particle level tree
     * manager. It does branch setup etc.
     */
    void setupParticleLevelTreeManager(const top::ParticleLevelEvent& plEvent);

    /*!
     * @brief Internal function which configures the upgrade tree manager.
     * It does branch setup etc.
     */
    void setupUpgradeTreeManager(/*const top::UpgradeEvent& upgradeEvent*/);
private:
    /**
     * @brief For each selection that we run in the top-xaod code, we "decorate"
     * event info with a variable saying if this event passed or failed it.
     *
     * This code takes the decoration from event info
     *
     * @param event The top event, need this so we can access the decorators on EventInfo
     * @param extraBranches The names of the branches we added to EventInfo (one per event)
     */
    void recordSelectionDecision(const top::Event& event);
    void recordTriggerDecision(const top::Event& event);

    void registerObjectIntoTruthTree(const SG::AuxElement& obj);
    void saveObjectIntoTruthTree(const SG::AuxElement& obj);

    /*!
     * @brief Helper function to load the PDF info from the current truth event
     *  and write it into the branch output variable(s).
     *
     * After calling this function, the `m_PDFinfo*` variables will contain the
     * current truth event's PDF info data (given that the truth event is
     * valid).
     */
    void loadPdfInfo();

    /*!
     * @brief Helper function to load the PDF weights from the current truth
     * event and write them into the branch output variable.
     *
     * After calling this function, the `m_PDF_eventWeights` variable will
     * contain the current truth event's LHA PDF weights for the requested PDF
     * sets (given that the truth event is valid).
     */
    void loadPdfWeights();

    /*!
     * @brief Helper function to load the OTF-computed MC generator weights.
     */
    void loadMCGeneratorWeights();

    ///Configuration
    std::shared_ptr<top::TopConfig> m_config;

    ///Scale factors
    std::shared_ptr<top::ScaleFactorRetriever> m_sfRetriever;

    ///The file where everything goes
    TFile* m_outputFile;

    ///A simple way to write out branches, without having to worry about the type.
    std::vector<std::shared_ptr<top::TreeManager>> m_treeManagers;
    std::shared_ptr<top::TreeManager> m_truthTreeManager;

    /// The top::TreeManager used for the particle level data output.
    std::shared_ptr<top::TreeManager> m_particleLevelTreeManager;
    /// The array of variables used for storing the particle level selection
    /// decisions. The std::pair contains:
    ///    .first  --- The name of the selection
    ///    .second --- The variable used for storing into the TTree
    std::vector< std::pair<std::string,int> > m_particleLevel_SelectionDecisions;

    /// TreeManager for upgrade analysis data output
    std::shared_ptr<top::TreeManager> m_upgradeTreeManager;

    ///names of the passed / failed branches.
    std::vector<std::string> m_extraBranches;

    /// branch filters - cf ANALYSISTO-61
    std::vector<top::TreeManager::BranchFilter> m_branchFilters;

    ///Decisions on if the event passed / failed a particular selection.
    std::vector<int> m_selectionDecisions;

    ///Decisions on if the event passed / failed a particular trigger.
    std::unordered_map<std::string, char> m_triggerDecisions;

    ///Pre-scale of the trigger menu for each event.
    std::unordered_map<std::string, float> m_triggerPrescales;

    //Store output PDF weights from LHAPDF
    std::unordered_map<std::string, std::vector<float> > m_PDF_eventWeights;

    //some event weights
    float m_weight_mc;
    float m_weight_pileup;
    ///-- Pileup SF systematics --///
    float m_weight_pileup_UP;
    float m_weight_pileup_DOWN;
    ///-- Lepton SF --///
    float m_weight_leptonSF;
    ///-- Lepton SF - electron SF systematics --///
    float m_weight_leptonSF_EL_SF_Trigger_UP;
    float m_weight_leptonSF_EL_SF_Trigger_DOWN;
    float m_weight_leptonSF_EL_SF_Reco_UP;
    float m_weight_leptonSF_EL_SF_Reco_DOWN;
    float m_weight_leptonSF_EL_SF_ID_UP;
    float m_weight_leptonSF_EL_SF_ID_DOWN;
    float m_weight_leptonSF_EL_SF_Isol_UP;
    float m_weight_leptonSF_EL_SF_Isol_DOWN;
    ///-- Lepton SF - muon SF systematics --///
    float m_weight_leptonSF_MU_SF_Trigger_STAT_UP;
    float m_weight_leptonSF_MU_SF_Trigger_STAT_DOWN;
    float m_weight_leptonSF_MU_SF_Trigger_SYST_UP;
    float m_weight_leptonSF_MU_SF_Trigger_SYST_DOWN;
    // Muon ID SF systematics (regular)
    float m_weight_leptonSF_MU_SF_ID_STAT_UP;
    float m_weight_leptonSF_MU_SF_ID_STAT_DOWN;
    float m_weight_leptonSF_MU_SF_ID_SYST_UP;
    float m_weight_leptonSF_MU_SF_ID_SYST_DOWN;
    // Muon ID SF systematics (low pT)
    float m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_UP;
    float m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_DOWN;
    float m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_UP;
    float m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_DOWN;
    // Muon isolation SF systematics
    float m_weight_leptonSF_MU_SF_Isol_STAT_UP;
    float m_weight_leptonSF_MU_SF_Isol_STAT_DOWN;
    float m_weight_leptonSF_MU_SF_Isol_SYST_UP;
    float m_weight_leptonSF_MU_SF_Isol_SYST_DOWN;
    float m_weight_leptonSF_MU_SF_TTVA_STAT_UP;
    float m_weight_leptonSF_MU_SF_TTVA_STAT_DOWN;
    float m_weight_leptonSF_MU_SF_TTVA_SYST_UP;
    float m_weight_leptonSF_MU_SF_TTVA_SYST_DOWN;

    ///-- individual components for lepton SF --///
    float m_weight_indiv_SF_EL_Trigger;
    float m_weight_indiv_SF_EL_Trigger_UP;
    float m_weight_indiv_SF_EL_Trigger_DOWN;
    float m_weight_indiv_SF_EL_Reco;
    float m_weight_indiv_SF_EL_Reco_UP;
    float m_weight_indiv_SF_EL_Reco_DOWN;
    float m_weight_indiv_SF_EL_ID;
    float m_weight_indiv_SF_EL_ID_UP;
    float m_weight_indiv_SF_EL_ID_DOWN;
    float m_weight_indiv_SF_EL_Isol;
    float m_weight_indiv_SF_EL_Isol_UP;
    float m_weight_indiv_SF_EL_Isol_DOWN;
    float m_weight_indiv_SF_EL_ChargeID;
    float m_weight_indiv_SF_EL_ChargeID_UP;
    float m_weight_indiv_SF_EL_ChargeID_DOWN;
    float m_weight_indiv_SF_EL_ChargeMisID;
    float m_weight_indiv_SF_EL_ChargeMisID_STAT_UP;
    float m_weight_indiv_SF_EL_ChargeMisID_STAT_DOWN;
    float m_weight_indiv_SF_EL_ChargeMisID_SYST_UP;
    float m_weight_indiv_SF_EL_ChargeMisID_SYST_DOWN;
    float m_weight_indiv_SF_MU_Trigger;
    float m_weight_indiv_SF_MU_Trigger_STAT_UP;
    float m_weight_indiv_SF_MU_Trigger_STAT_DOWN;
    float m_weight_indiv_SF_MU_Trigger_SYST_UP;
    float m_weight_indiv_SF_MU_Trigger_SYST_DOWN;
    float m_weight_indiv_SF_MU_ID;
    // Muon ID SF systematics (regular)
    float m_weight_indiv_SF_MU_ID_STAT_UP;
    float m_weight_indiv_SF_MU_ID_STAT_DOWN;
    float m_weight_indiv_SF_MU_ID_SYST_UP;
    float m_weight_indiv_SF_MU_ID_SYST_DOWN;
    // Muon ID SF systematics (low pt)
    float m_weight_indiv_SF_MU_ID_STAT_LOWPT_UP;
    float m_weight_indiv_SF_MU_ID_STAT_LOWPT_DOWN;
    float m_weight_indiv_SF_MU_ID_SYST_LOWPT_UP;
    float m_weight_indiv_SF_MU_ID_SYST_LOWPT_DOWN;
    // Muon isolation systematics
    float m_weight_indiv_SF_MU_Isol;
    float m_weight_indiv_SF_MU_Isol_SYST_UP;
    float m_weight_indiv_SF_MU_Isol_SYST_DOWN;
    float m_weight_indiv_SF_MU_Isol_STAT_UP;
    float m_weight_indiv_SF_MU_Isol_STAT_DOWN;
    float m_weight_indiv_SF_MU_TTVA;
    float m_weight_indiv_SF_MU_TTVA_SYST_UP;
    float m_weight_indiv_SF_MU_TTVA_SYST_DOWN;
    float m_weight_indiv_SF_MU_TTVA_STAT_UP;
    float m_weight_indiv_SF_MU_TTVA_STAT_DOWN;

    // Taus
    float m_weight_tauSF;
    float m_weight_tauSF_ELEOLR_UP;
    float m_weight_tauSF_ELEOLR_DOWN;
    float m_weight_tauSF_JETID_UP;
    float m_weight_tauSF_JETID_DOWN;
    float m_weight_tauSF_RECO_UP;
    float m_weight_tauSF_RECO_DOWN;

    // Photons
    float m_weight_photonSF = 0.;
    float m_weight_photonSF_ID_UP = 0.;
    float m_weight_photonSF_ID_DOWN = 0.;
    float m_weight_photonSF_effIso = 0.;
    float m_weight_photonSF_effLowPtIso_UP = 0.;
    float m_weight_photonSF_effLowPtIso_DOWN = 0.;
    float m_weight_photonSF_effTrkIso_UP = 0.;
    float m_weight_photonSF_effTrkIso_DOWN = 0.;
    float m_weight_photonSF_isoDDonoff_UP = 0.;
    float m_weight_photonSF_isoDDonoff_DOWN = 0.;

    // nominal b-tagging SF [WP]
    std::unordered_map<std::string, float> m_weight_bTagSF;
    std::unordered_map<std::string, float> m_weight_trackjet_bTagSF;

    // JVT (c++11 initialization for fun)
    float m_weight_jvt = 0.0;
    float m_weight_jvt_up = 0.0;
    float m_weight_jvt_down = 0.0;

    // Sherpa 2.2 weight 
    float m_weight_sherpa_22_vjets = 0.;

    // eigen variations affecting b-jets [WP]
    std::unordered_map<std::string, std::vector<float>> m_weight_bTagSF_eigen_B_up;
    std::unordered_map<std::string, std::vector<float>> m_weight_bTagSF_eigen_B_down;
    std::unordered_map<std::string, std::vector<float>> m_weight_trackjet_bTagSF_eigen_B_up;
    std::unordered_map<std::string, std::vector<float>> m_weight_trackjet_bTagSF_eigen_B_down;

    // eigen variations affecting c-jets [WP]
    std::unordered_map<std::string, std::vector<float>> m_weight_bTagSF_eigen_C_up;
    std::unordered_map<std::string, std::vector<float>> m_weight_bTagSF_eigen_C_down;
    std::unordered_map<std::string, std::vector<float>> m_weight_trackjet_bTagSF_eigen_C_up;
    std::unordered_map<std::string, std::vector<float>> m_weight_trackjet_bTagSF_eigen_C_down;

    // eigen variations affecting light jets [WP]
    std::unordered_map<std::string, std::vector<float>> m_weight_bTagSF_eigen_Light_up;
    std::unordered_map<std::string, std::vector<float>> m_weight_bTagSF_eigen_Light_down;
    std::unordered_map<std::string, std::vector<float>> m_weight_trackjet_bTagSF_eigen_Light_up;
    std::unordered_map<std::string, std::vector<float>> m_weight_trackjet_bTagSF_eigen_Light_down;

    // named systematics [WP][name]
    std::unordered_map<std::string, std::unordered_map<std::string, float>> m_weight_bTagSF_named_up;
    std::unordered_map<std::string, std::unordered_map<std::string, float>> m_weight_bTagSF_named_down;
    std::unordered_map<std::string, std::unordered_map<std::string, float>> m_weight_trackjet_bTagSF_named_up;
    std::unordered_map<std::string, std::unordered_map<std::string, float>> m_weight_trackjet_bTagSF_named_down;

    // remove "FT_EFF_" and spaces in named systematics
    std::string betterBtagNamedSyst (const std::string WP);

    ///-- weights for matrix-method fakes estimate, for each selection and configuration --///
    /// m_fakesMM_weights[selection][configuration]
    std::unordered_map<std::string,std::unordered_map<std::string, float>> m_fakesMM_weights;


    //event info
    unsigned long long m_eventNumber;
    unsigned int m_runNumber;
    unsigned int m_randomRunNumber;
    unsigned int m_mcChannelNumber;
    float m_mu_original;
    float m_mu;
    // non-collision background flag - usage: https://twiki.cern.ch/twiki/bin/view/Atlas/NonCollisionBackgroundsRunTwo#Recommend_cuts_tools_and_procedu
    unsigned int m_backgroundFlags;
    // hasBadMuon flag - see: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/MuonSelectionTool#is_BadMuon_Flag_Event_Veto
    unsigned int m_hasBadMuon;

    //electrons
    std::vector<float> m_el_pt;
    std::vector<float> m_el_eta;
    std::vector<float> m_el_cl_eta;
    std::vector<float> m_el_phi;
    std::vector<float> m_el_e;
    std::vector<float> m_el_charge;
    std::vector<float> m_el_topoetcone20;
    std::vector<float> m_el_ptvarcone20;
    std::vector<char>  m_el_isTight;
    std::vector<char>  m_el_CF; // pass charge ID selector (has no charge flip)
    std::unordered_map<std::string, std::vector<char> > m_el_trigMatched;
    std::vector<float> m_el_d0sig;
    std::vector<float> m_el_delta_z0_sintheta;
    std::vector<int>   m_el_true_type;
    std::vector<int>   m_el_true_origin;
    std::vector<int>   m_el_true_typebkg;
    std::vector<int>   m_el_true_originbkg;

    //muons
    std::vector<float> m_mu_pt;
    std::vector<float> m_mu_eta;
    std::vector<float> m_mu_phi;
    std::vector<float> m_mu_e;
    std::vector<float> m_mu_charge;
    std::vector<float> m_mu_topoetcone20;
    std::vector<float> m_mu_ptvarcone30;
    std::vector<char>  m_mu_isTight;
    std::unordered_map<std::string, std::vector<char> > m_mu_trigMatched;
    std::vector<float> m_mu_d0sig;
    std::vector<float> m_mu_delta_z0_sintheta;
    std::vector<int>   m_mu_true_type;
    std::vector<int>   m_mu_true_origin;
    //photons
    std::vector<float> m_ph_pt;
    std::vector<float> m_ph_eta;
    std::vector<float> m_ph_phi;
    std::vector<float> m_ph_e;
    std::vector<float> m_ph_iso;

    //taus
    std::vector<float> m_tau_pt;
    std::vector<float> m_tau_eta;
    std::vector<float> m_tau_phi;
    std::vector<float> m_tau_charge;

    //jets
    std::vector<float> m_jet_pt;
    std::vector<float> m_jet_eta;
    std::vector<float> m_jet_phi;
    std::vector<float> m_jet_e;
    std::vector<float> m_jet_mv2c00;
    std::vector<float> m_jet_mv2c10;
    std::vector<float> m_jet_mv2c20;
    std::vector<float> m_jet_jvt;
    std::vector<char> m_jet_passfjvt;
    std::vector<float> m_jet_ip3dsv1;
    std::vector<int>   m_jet_truthflav;
    std::vector<int>   m_jet_truthPartonLabel;
    std::vector<char>  m_jet_isTrueHS;
    std::unordered_map<std::string, std::vector<char>>  m_jet_isbtagged;//one vector per jet per WP
    std::vector<int>  m_jet_tagWeightBin;//tag-weight bin in case Continuous WP is used

    // for upgrade, we store the tagging efficiency per jet & whether it is from pileup
    std::vector<float> m_jet_mv1eff;
    std::vector<float> m_jet_isPileup;

    //large-R jets
    std::vector<float> m_ljet_pt;
    std::vector<float> m_ljet_eta;
    std::vector<float> m_ljet_phi;
    std::vector<float> m_ljet_e;
    std::vector<float> m_ljet_m;
    std::vector<float> m_ljet_sd12;

    //track jets
    std::vector<float> m_tjet_pt;
    std::vector<float> m_tjet_eta;
    std::vector<float> m_tjet_phi;
    std::vector<float> m_tjet_e;
    std::vector<float> m_tjet_mv2c00;
    std::vector<float> m_tjet_mv2c10;
    std::vector<float> m_tjet_mv2c20;
    std::unordered_map<std::string, std::vector<char>>  m_tjet_isbtagged;//one vector per jet per WP
    std::vector<int>  m_tjet_tagWeightBin;//tag-weight bin in case Continuous WP is used

    //re-clustered jets
    //  -> need unordered map for systematics
    bool m_makeRCJets; // making re-clustered jets
    bool m_makeVarRCJets; // making VarRC jets
    std::string m_RCJetContainer;       // name for RC jets container in TStore
    std::vector<std::string> m_VarRCJetRho;
    std::vector<std::string> m_VarRCJetMassScale;
    std::unique_ptr<RCJetMC15> m_rc;
    std::map<std::string,std::unique_ptr<RCJetMC15> > m_VarRC;
    std::string m_egamma;      // egamma systematic naming scheme
    std::string m_muonsyst;    // muon systematic naming scheme
    std::string m_jetsyst;     // jet systematic naming scheme
    std::unordered_map<std::size_t, JetReclusteringTool*> m_jetReclusteringTool;
    std::map<std::string,std::vector<float>> m_VarRCjetBranches;
    std::map<std::string,std::vector<std::vector<float>>> m_VarRCjetsubBranches;
    std::vector<int> m_rcjet_nsub;
    std::vector<float> m_rcjet_pt;
    std::vector<float> m_rcjet_eta;
    std::vector<float> m_rcjet_phi;
    std::vector<float> m_rcjet_e;
    std::vector<float> m_rcjet_d12;
    std::vector<float> m_rcjet_d23;
    std::vector<std::vector<float> > m_rcjetsub_pt;
    std::vector<std::vector<float> > m_rcjetsub_eta;
    std::vector<std::vector<float> > m_rcjetsub_phi;
    std::vector<std::vector<float> > m_rcjetsub_e;
    std::vector<std::vector<float> > m_rcjetsub_mv2c10;

    //met
    float m_met_met;
    float m_met_phi;

    //KLFitter
    short m_klfitter_selected;
    /// Error flags
    std::vector<short> m_klfitter_minuitDidNotConverge;
    std::vector<short> m_klfitter_fitAbortedDueToNaN;
    std::vector<short> m_klfitter_atLeastOneFitParameterAtItsLimit;
    std::vector<short> m_klfitter_invalidTransferFunctionAtConvergence;

    /// Global result
    std::vector<unsigned int> m_klfitter_bestPermutation;
    std::vector<float> m_klfitter_logLikelihood;
    std::vector<float> m_klfitter_eventProbability;
    std::vector<std::vector<double>> m_klfitter_parameters;
    std::vector<std::vector<double>> m_klfitter_parameterErrors;

    /// Model
    std::vector<float> m_klfitter_model_bhad_pt;
    std::vector<float> m_klfitter_model_bhad_eta;
    std::vector<float> m_klfitter_model_bhad_phi;
    std::vector<float> m_klfitter_model_bhad_E;
    std::vector<unsigned int> m_klfitter_model_bhad_jetIndex;

    std::vector<float> m_klfitter_model_blep_pt;
    std::vector<float> m_klfitter_model_blep_eta;
    std::vector<float> m_klfitter_model_blep_phi;
    std::vector<float> m_klfitter_model_blep_E;
    std::vector<unsigned int> m_klfitter_model_blep_jetIndex;

    std::vector<float> m_klfitter_model_lq1_pt;
    std::vector<float> m_klfitter_model_lq1_eta;
    std::vector<float> m_klfitter_model_lq1_phi;
    std::vector<float> m_klfitter_model_lq1_E;
    std::vector<unsigned int> m_klfitter_model_lq1_jetIndex;

    std::vector<float> m_klfitter_model_lq2_pt;
    std::vector<float> m_klfitter_model_lq2_eta;
    std::vector<float> m_klfitter_model_lq2_phi;
    std::vector<float> m_klfitter_model_lq2_E;
    std::vector<unsigned int> m_klfitter_model_lq2_jetIndex;

    std::vector<float> m_klfitter_model_Higgs_b1_pt;
    std::vector<float> m_klfitter_model_Higgs_b1_eta;
    std::vector<float> m_klfitter_model_Higgs_b1_phi;
    std::vector<float> m_klfitter_model_Higgs_b1_E;
    std::vector<unsigned int> m_klfitter_model_Higgs_b1_jetIndex;

    std::vector<float> m_klfitter_model_Higgs_b2_pt;
    std::vector<float> m_klfitter_model_Higgs_b2_eta;
    std::vector<float> m_klfitter_model_Higgs_b2_phi;
    std::vector<float> m_klfitter_model_Higgs_b2_E;
    std::vector<unsigned int> m_klfitter_model_Higgs_b2_jetIndex;

    std::vector<float> m_klfitter_model_lep_pt;
    std::vector<float> m_klfitter_model_lep_eta;
    std::vector<float> m_klfitter_model_lep_phi;
    std::vector<float> m_klfitter_model_lep_E;

    std::vector<float> m_klfitter_model_nu_pt;
    std::vector<float> m_klfitter_model_nu_eta;
    std::vector<float> m_klfitter_model_nu_phi;
    std::vector<float> m_klfitter_model_nu_E;

    // calculated KLFitter variables for best perm
    float m_klfitter_bestPerm_topLep_pt;
    float m_klfitter_bestPerm_topLep_eta;
    float m_klfitter_bestPerm_topLep_phi;
    float m_klfitter_bestPerm_topLep_E;
    float m_klfitter_bestPerm_topLep_m;

    float m_klfitter_bestPerm_topHad_pt;
    float m_klfitter_bestPerm_topHad_eta;
    float m_klfitter_bestPerm_topHad_phi;
    float m_klfitter_bestPerm_topHad_E;
    float m_klfitter_bestPerm_topHad_m;

    float m_klfitter_bestPerm_ttbar_pt;
    float m_klfitter_bestPerm_ttbar_eta;
    float m_klfitter_bestPerm_ttbar_phi;
    float m_klfitter_bestPerm_ttbar_E;
    float m_klfitter_bestPerm_ttbar_m;

    // PseudoTop variables

    float m_PseudoTop_Reco_ttbar_pt;
    float m_PseudoTop_Reco_ttbar_eta;
    float m_PseudoTop_Reco_ttbar_phi;
    float m_PseudoTop_Reco_ttbar_m;
    float m_PseudoTop_Reco_top_had_pt;
    float m_PseudoTop_Reco_top_had_eta;
    float m_PseudoTop_Reco_top_had_phi;
    float m_PseudoTop_Reco_top_had_m;
    float m_PseudoTop_Reco_top_lep_pt;
    float m_PseudoTop_Reco_top_lep_eta;
    float m_PseudoTop_Reco_top_lep_phi;
    float m_PseudoTop_Reco_top_lep_m;
    float m_PseudoTop_Particle_ttbar_pt;
    float m_PseudoTop_Particle_ttbar_eta;
    float m_PseudoTop_Particle_ttbar_phi;
    float m_PseudoTop_Particle_ttbar_m;
    float m_PseudoTop_Particle_top_had_pt;
    float m_PseudoTop_Particle_top_had_eta;
    float m_PseudoTop_Particle_top_had_phi;
    float m_PseudoTop_Particle_top_had_m;
    float m_PseudoTop_Particle_top_lep_pt;
    float m_PseudoTop_Particle_top_lep_eta;
    float m_PseudoTop_Particle_top_lep_phi;
    float m_PseudoTop_Particle_top_lep_m;


    //MC
    std::vector<float> m_mc_pt;
    std::vector<float> m_mc_eta;
    std::vector<float> m_mc_phi;
    std::vector<float> m_mc_e;
    std::vector<int> m_mc_pdgId;

    //PDFInfo
    std::vector<float> m_PDFinfo_X1;
    std::vector<float> m_PDFinfo_X2;
    std::vector<int> m_PDFinfo_PDGID1;
    std::vector<int> m_PDFinfo_PDGID2;
    std::vector<float> m_PDFinfo_Q;
    std::vector<float> m_PDFinfo_XF1;
    std::vector<float> m_PDFinfo_XF2;
    
    //the on-the-fly computed generator weights
    //there is one vector of float per entry in the trutheventcontainer (which should have only 1 entry)
    std::vector<float> m_mc_generator_weights;

    //Extra variables for Particle Level (bare lepton kinematics and b-Hadron
    //tagging information).
    std::vector<float> m_el_pt_bare;
    std::vector<float> m_el_eta_bare;
    std::vector<float> m_el_phi_bare;
    std::vector<float> m_el_e_bare;

    std::vector<float> m_mu_pt_bare;
    std::vector<float> m_mu_eta_bare;
    std::vector<float> m_mu_phi_bare;
    std::vector<float> m_mu_e_bare;

    std::vector<int> m_jet_Ghosts_BHadron_Final_Count;
    std::vector<int> m_jet_Ghosts_CHadron_Final_Count;
    std::vector<int> m_ljet_Ghosts_BHadron_Final_Count;
    std::vector<int> m_ljet_Ghosts_CHadron_Final_Count;


    // Truth tree inserted variables
    // This can be expanded as required
    // This is just a first pass at doing this sort of thing
    std::unordered_map<std::string,int*> m_extraTruthVars_int;
    std::unordered_map<std::string,float*> m_extraTruthVars_float;

    ClassDef(top::EventSaverFlatNtuple, 0);
};

}

#endif
