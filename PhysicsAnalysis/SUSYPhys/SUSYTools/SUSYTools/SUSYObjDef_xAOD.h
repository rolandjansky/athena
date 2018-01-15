/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This file's extension implies that it's C, but it's really -*- C++ -*-.
// $Id$
/**
 * @file SUSYTools/SUSYTools/SUSYObjDef.h
 * @author Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
 * @date Feb, 2011
 * @brief Baseline SUSY object definitions.
 */

#ifndef SUSYTOOLS_SUSYOBJDEF_XAOD_H
#define SUSYTOOLS_SUSYOBJDEF_XAOD_H

// Framework include(s) - base class
#include "AsgTools/AsgMetadataTool.h"

// Interface class
#include "SUSYTools/ISUSYObjDef_xAODTool.h"

// Includes for systematics
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h"

// Tool handles
#include "AsgTools/AnaToolHandle.h"

// Configuration
#include "TEnv.h"

// Map for config file names
#include <map>
// Set for properties
#include <set>
// Various uses in function arguments and parameters
#include <string>
#include <vector>

// Tool interfaces
// Toolbox, which holds the tools
#include "AssociationUtils/ToolBox.h"

class IJetCalibrationTool;
class IJERTool;
class IJERSmearingTool;
class ICPJetUncertaintiesTool;
class IJetSelector;
class IJetUpdateJvt;
class IJetModifier;

class IAsgElectronLikelihoodTool;
class IAsgElectronEfficiencyCorrectionTool;
class IEGammaAmbiguityTool;

class IAsgPhotonIsEMSelector;
class IAsgPhotonEfficiencyCorrectionTool;
class IElectronPhotonShowerShapeFudgeTool;

class IBTaggingEfficiencyTool;
class IBTaggingSelectionTool;

class IMETMaker;
class IMETSystematicsTool;

namespace CP {
  class IMuonSelectionTool;
  class IMuonCalibrationAndSmearingTool;
  class IMuonEfficiencyScaleFactors;
  class IMuonTriggerScaleFactors;

  class IEgammaCalibrationAndSmearingTool;
  class IEfficiencyScaleFactorTool;
  class IIsolationSelectionTool;
  class IIsolationCloseByCorrectionTool;
  class IIsolationCorrectionTool;
  class IPileupReweightingTool;
  class IJetJvtEfficiency;
}

namespace TauAnalysisTools {
  class ITauSelectionTool;
  class ITauSmearingTool;
  class ITauEfficiencyCorrectionsTool;
  class ITauOverlappingElectronLLHDecorator;
  class ITauTruthMatchingTool;
}

class ITauToolBase;

namespace ORUtils {
  class IOverlapTool;
}

namespace TrigConf {
  class ITrigConfigTool;
}
namespace Trig {
  // Need the TrigDecisionTool directly for getChainGroup, features, and GetPreScale
  class TrigDecisionTool;
  class IMatchingTool;
  class FeatureContainer;
}

class IWeightTool;

namespace ST {


  //  class SUSYObjDef;
  class SUSYObjDef_xAOD : public virtual ISUSYObjDef_xAODTool,
			  public asg::AsgMetadataTool {

    // Create a proper constructor for Athena
    ASG_TOOL_CLASS( SUSYObjDef_xAOD, ST::ISUSYObjDef_xAODTool)

    public:
    SUSYObjDef_xAOD( const std::string& name );
    ~SUSYObjDef_xAOD();

    // Function initialising the tool
    StatusCode initialize() override final;
    // NB that there is no function for finalising the tool.
    //  An IAsgTool does not have a finalize method, so we can 
    //  only override finalize in athena.  To clean up, delete me.

    bool isData() const override final {return m_dataSource == Data;}
    bool isAtlfast() const override final {return m_dataSource == AtlfastII;}

    StatusCode setBoolProperty(const std::string& name, const bool& property) override final;

    // autoconfiguration of pileup-reweighting tool
    StatusCode autoconfigurePileupRWTool() override final;

    // Hack to make thisx configurable from python
    // 0 = data, 1 = full sim, 2 = AF-II
    void setDataSource(int source);

    // Apply the correction on a modifyable object
    StatusCode FillJet(xAOD::Jet& input, const bool doCalib = true, bool isFat = false) override final;
    StatusCode FillTau(xAOD::TauJet& input) override final;
    StatusCode FillMuon(xAOD::Muon& input, const float ptcut, const float etacut) override final;
    StatusCode FillElectron(xAOD::Electron& input, const float etcut, const float etacut) override final;
    StatusCode FillPhoton(xAOD::Photon& input, const float ptcut, const float etacut) override final;

    const xAOD::Vertex* GetPrimVtx() const override final;

    StatusCode GetJets(xAOD::JetContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, const bool recordSG = true, const std::string& jetkey = "", const xAOD::JetContainer* containerToBeCopied = 0) override final;
    StatusCode GetJetsSyst(const xAOD::JetContainer& calibjets, xAOD::JetContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, const bool recordSG = true, const std::string& jetkey = "") override final;
    StatusCode GetFatJets(xAOD::JetContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, const bool recordSG = false, const std::string& jetkey = "", const bool doLargeRdecorations = false, const xAOD::JetContainer* containerToBeCopied = 0) override final;
    StatusCode GetTaus(xAOD::TauJetContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, const bool recordSG = true, const std::string& taukey = "TauJets", const xAOD::TauJetContainer* containerToBeCopied = 0) override final;
    StatusCode GetMuons(xAOD::MuonContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, const bool recordSG = true, const std::string& muonkey = "Muons", const xAOD::MuonContainer* containerToBeCopied = 0) override final;
    StatusCode GetElectrons(xAOD::ElectronContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, const bool recordSG = true, const std::string& elekey = "Electrons", const xAOD::ElectronContainer* containerToBeCopied = 0) override final;
    StatusCode GetPhotons(xAOD::PhotonContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, const bool recordSG = true, const std::string& photonkey = "Photons", const xAOD::PhotonContainer* containerToBeCopied = 0) override final;
    StatusCode GetMET(xAOD::MissingETContainer& met,
                      const xAOD::JetContainer* jet,
                      const xAOD::ElectronContainer* elec = 0,
                      const xAOD::MuonContainer* muon = 0,
                      const xAOD::PhotonContainer* gamma = 0,
                      const xAOD::TauJetContainer* taujet = 0,
                      bool doTST = true, bool doJVTCut = true,
                      const xAOD::IParticleContainer* invis = 0) override final;

    StatusCode GetTrackMET(xAOD::MissingETContainer& met,
                           const xAOD::JetContainer* jet,
                           const xAOD::ElectronContainer* elec = 0,
                           const xAOD::MuonContainer* muon = 0
			   // const xAOD::PhotonContainer* gamma = 0,
			   // const xAOD::TauJetContainer* taujet = 0,
			   ) override final;

    bool IsSignalJet(const xAOD::Jet& input, const float ptcut, const float etacut) const override final;

    bool IsBadJet(const xAOD::Jet& input) const override final;

    bool IsBJetLoose(const xAOD::Jet& input) const override final; 
    bool JetPassJVT(xAOD::Jet& input, bool update_jvt) override final;


    bool IsSignalMuon(const xAOD::Muon& input, const float ptcut, const float d0sigcut, const float z0cut, const float etacut = DUMMYDEF) const override final;

    bool IsSignalElectron(const xAOD::Electron& input, const float etcut, const float d0sigcut, const float z0cut, const float etacut = DUMMYDEF) const override final;

    bool IsCosmicMuon(const xAOD::Muon& input, const float z0cut, const float d0cut) const override final;

    bool IsHighPtMuon(const xAOD::Muon& input) const override final;
    // See https://indico.cern.ch/event/371499/contribution/1/material/slides/0.pdf and https://indico.cern.ch/event/397325/contribution/19/material/slides/0.pdf and https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSelectionTool

    bool IsSignalTau(const xAOD::TauJet& input, const float ptcut, const float etacut) const override final;

    bool IsBadMuon(const xAOD::Muon& input, const float qopcut) const override final;

    bool IsSignalPhoton(const xAOD::Photon& input, const float ptcut, const float etacut = DUMMYDEF) const override final;

    bool IsBJet(const xAOD::Jet& input) const override final;

    bool IsTruthBJet(const xAOD::Jet& input) const override final;

    int IsBJetContinuous(const xAOD::Jet& input) const override final;

    float BtagSF(const xAOD::JetContainer* jets) const override final;

    float BtagSFsys(const xAOD::JetContainer* jets, const CP::SystematicSet& systConfig) override final;

    double JVT_SF(const xAOD::JetContainer* jets) override final;

    double JVT_SFsys(const xAOD::JetContainer* jets, const CP::SystematicSet& systConfig) override final;

    //SF helpers
    //muons
    float GetSignalMuonSF(const xAOD::Muon& mu, const bool recoSF = true, const bool isoSF = true, const bool doBadMuonHP = true, const bool warnOVR = true) override final;

    double GetMuonTriggerEfficiency(const xAOD::Muon& mu, const std::string& trigExpr, const bool isdata = false ) override final;

    double GetTotalMuonTriggerSF(const xAOD::MuonContainer& sfmuons, const std::string& trigExpr) override final;

    double GetTotalMuonSF(const xAOD::MuonContainer& muons, const bool recoSF = true, const bool isoSF = true, const std::string& trigExpr="HLT_mu20_iloose_L1MU15_OR_HLT_mu50", const bool bmhptSF = true) override final;

    double GetTotalMuonSFsys(const xAOD::MuonContainer& muons, const CP::SystematicSet& systConfig, const bool recoSF = true, const bool isoSF = true, const std::string& trigExpr = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", const bool bmhptSF = true) override final;

    //electrons
    float GetSignalElecSF(const xAOD::Electron& el, const bool recoSF = true, const bool idSF = true, const bool triggerSF = true, const bool isoSF = true, const std::string& trigExpr = "e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose", const bool chfSF = false) override final;

    double GetEleTriggerEfficiency(const xAOD::Electron& el, const std::string& trigExpr = "e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose") const override final; 
    double GetEleTriggerEfficiencySF(const xAOD::Electron& el, const std::string& trigExpr = "e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose") const override final;

    float GetTotalElectronSF(const xAOD::ElectronContainer& electrons, const bool recoSF = true, const bool idSF = true, const bool triggerSF = true, const bool isoSF = true, const std::string& trigExpr = "singleLepton", const bool chfSF = false) override final; // singleLepton == Ele.TriggerSFStringSingle value

    float GetTotalElectronSFsys(const xAOD::ElectronContainer& electrons, const CP::SystematicSet& systConfig, const bool recoSF = true, const bool idSF = true, const bool triggerSF = true, const bool isoSF = true, const std::string& trigExpr = "singleLepton", const bool chfSF = false) override final; // singleLepton == Ele.TriggerSFStringSingle value

    //taus
    double GetSignalTauSF(const xAOD::TauJet& tau, const bool idSF = true, const bool triggerSF = true, const std::string& trigExpr = "tau25_medium1_tracktwo") override final;
  
    double GetSignalTauSFsys(const xAOD::TauJet& tau, const CP::SystematicSet& systConfig, const bool idSF = true, const bool triggerSF = true, const std::string& trigExpr = "tau25_medium1_tracktwo") override final;

    double GetTauTriggerEfficiencySF(const xAOD::TauJet& tau, const std::string& trigExpr = "tau25_medium1_tracktwo") override final;

    double GetTotalTauSF(const xAOD::TauJetContainer& taus, const bool idSF = true, const bool triggerSF = true,  const std::string& trigExpr = "tau25_medium1_tracktwo") override final;

    double GetTotalTauSFsys(const xAOD::TauJetContainer& taus, const CP::SystematicSet& systConfig, const bool idSF = true, const bool triggerSF = true, const std::string& trigExpr = "tau25_medium1_tracktwo") override final;
  
    //photons
    double GetSignalPhotonSF(const xAOD::Photon& ph, const bool effSF = true, const bool isoSF = true) const override final;

    double GetSignalPhotonSFsys(const xAOD::Photon& ph, const CP::SystematicSet& systConfig, const bool effSF = true, const bool isoSF = true) override final;

    double GetTotalPhotonSF(const xAOD::PhotonContainer& photons, const bool effSF = true, const bool isoSF = true) const override final;

    double GetTotalPhotonSFsys(const xAOD::PhotonContainer& photons, const CP::SystematicSet& systConfig, const bool effSF = true, const bool isoSF = true) override final;

    //jets
    double GetTotalJetSF(const xAOD::JetContainer* jets, const bool btagSF = true, const bool jvtSF = true) override final;

    double GetTotalJetSFsys(const xAOD::JetContainer* jets, const CP::SystematicSet& systConfig, const bool btagSF = true, const bool jvtSF = true) override final;

    //Trigger
    bool IsMETTrigPassed(unsigned int runnumber = 0, bool j400_OR = false) const override final;
    bool IsMETTrigPassed(const std::string& triggerName, bool j400_OR = false) const override final;

    bool IsTrigPassed(const std::string&, unsigned int condition=TrigDefs::Physics) const override final;

    bool IsTrigMatched(const xAOD::IParticle *part, const std::string& tr_item) override final;
    bool IsTrigMatched(const xAOD::IParticle *part1, const xAOD::IParticle *part2, const std::string& tr_item) override final;
    bool IsTrigMatched(const std::vector<const xAOD::IParticle*>& v, const std::string& tr_item) override final;
    bool IsTrigMatched(const std::initializer_list<const xAOD::IParticle*> &v, const std::string& tr_item) override final;
  
    void TrigMatch(const xAOD::IParticle* p, std::initializer_list<std::string>::iterator, std::initializer_list<std::string>::iterator) override final;
    void TrigMatch(const xAOD::IParticle* p, const std::vector<std::string>& items) override final;
    void TrigMatch(const xAOD::IParticle* p, const std::initializer_list<std::string>& items) override final;
    void TrigMatch(const xAOD::IParticleContainer* v, const std::vector<std::string>& items) override final;
    void TrigMatch(const xAOD::IParticleContainer* v, const std::initializer_list<std::string>& items) override final;
    void TrigMatch(const std::initializer_list<const xAOD::IParticle*>& v, const std::vector<std::string>& items) override final;
    void TrigMatch(const std::initializer_list<const xAOD::IParticle*>& v, const std::initializer_list<std::string>& items) override final;
    void TrigMatch(const xAOD::IParticle* p, const std::string& item) override final;
    void TrigMatch(const xAOD::IParticleContainer* v,  const std::string& item) override final;
    void TrigMatch(const std::initializer_list<const xAOD::IParticle*> &v, const std::string& item) override final;

    // Trigger helpers
    float GetTrigPrescale(const std::string&) const override final;
    const Trig::ChainGroup* GetTrigChainGroup(const std::string&) const override final;
    std::vector<std::string> GetTriggerOR(std::string trigExpr) const;
    void GetTriggerTokens(std::string, std::vector<std::string>& , std::vector<std::string>& ) const;
    Trig::FeatureContainer GetTriggerFeatures(const std::string& chainName = "EF_.*", unsigned int condition = TrigDefs::Physics) const;

    float GetPileupWeight() override final;

    ULong64_t GetPileupWeightHash() override final;

    float GetDataWeight(const std::string&) override final;

    float GetCorrectedAverageInteractionsPerCrossing(bool includeDataSF=false) override final;

    double GetSumOfWeights(int channel) override final;

    unsigned int GetRandomRunNumber(bool muDependentRRN = true) override final;

    StatusCode ApplyPRWTool(bool muDependentRRN = true) override final;

    unsigned int GetRunNumber() const override final;

    int treatAsYear(const int runNumber=-1) const override final;

    StatusCode OverlapRemoval(const xAOD::ElectronContainer *electrons, const xAOD::MuonContainer *muons, const xAOD::JetContainer *jets,
                              const xAOD::PhotonContainer* gamma = 0, const xAOD::TauJetContainer* taujet = 0, const xAOD::JetContainer *fatjets = 0) override final;

    StatusCode NearbyLeptonCorrections(const xAOD::ElectronContainer *electrons = nullptr, const xAOD::MuonContainer *muons = nullptr) const override final;

    CP::SystematicCode resetSystematics() override final;

    const CP::SystematicSet& currentSystematic() const;

    CP::SystematicCode applySystematicVariation( const CP::SystematicSet& systConfig ) override final;


    //truth helpers
    bool isPrompt(const xAOD::IParticle* part) const override final;

    StatusCode FindSusyHP(int& pdgid1, int& pdgid2) const;
    StatusCode FindSusyHP(const xAOD::TruthParticleContainer *truthP, int& pdgid1, int& pdgid2, bool isTruth3=false) const override final;
    StatusCode FindSusyHP(const xAOD::TruthEvent *truthE, int& pdgid1, int& pdgid2) const override final;
    static bool FindSusyHardProc(const xAOD::TruthParticleContainer *truthP, int& pdgid1, int& pdgid2, bool isTruth3=false);
    static bool FindSusyHardProc(const xAOD::TruthEvent *truthE, int& pdgid1, int& pdgid2);

    //trigger helpers
    std::string TrigSingleLep() const override final;
    std::string TrigDiLep() const override final;
    std::string TrigMixLep() const override final;


    //systematics helpers
    bool isNominal(const CP::SystematicSet& syst) const;
    bool isWeight(const CP::SystematicSet& systSet) const;
    bool isVariation(const CP::SystematicSet& syst) const;
    bool currentSystematicIsNominal() const;
    bool currentSystematicIsVariation() const;
    bool currentSystematicIsWeight() const;

    ST::SystInfo getSystInfo(const CP::SystematicVariation& sys) const;

    std::vector<ST::SystInfo> getSystInfoList() const override final;

    // Temporary function for Sherpa 2.2 V+jets n-jets reweighting 
    // (see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/CentralMC15ProductionList#NEW_Sherpa_v2_2_V_jets_NJet_rewe)
    float getSherpaVjetsNjetsWeight() const override final;
    float getSherpaVjetsNjetsWeight(const std::string& jetContainer) const override final;

    // Helper for b-tagging weights
    int getMCShowerType(const std::string& sample_name="") const override final { return ST::getMCShowerType(sample_name); }


  private:

    StatusCode SUSYToolsInit();

    //map ConfigFile names to Property names
    std::map<std::string, std::string> m_conf_to_prop;

    //book boolean properties already set
    std::set<std::string> m_bool_prop_set;

    // These variables just cache decisions so it's OK to make them mutable
    // Store trigger emulation functions so they're only setup once
    mutable std::map<std::string, std::function<bool()>> m_metTriggerFuncs;
    // Store whether the trigger was in the TDT
    mutable std::map<std::string, bool> m_checkedTriggers;
    bool m_emulateHLT(const std::string& triggerName) const;
    bool m_isTrigInTDT(const std::string& triggerName) const;

    //book trigger chains for matching
    std::vector<std::string> v_trigs15_cache_single;
    std::vector<std::string> v_trigs16_cache_single;
    std::vector<std::string> v_trigs15_cache_dilep;
    std::vector<std::string> v_trigs16_cache_dilep;
    std::vector<std::string> v_trigs15_cache_mixlep;
    std::vector<std::string> v_trigs16_cache_mixlep; 

  protected:

    StatusCode readConfig() override final; 
    StatusCode validConfig(bool strict = false) const;

    const std::vector<std::string> split(const std::string& s, const std::string& delim) const;

    void getTauConfig(const std::string tauConfigPath, std::vector<float>& pT_window, std::vector<float>& eta_window, bool &eleOLR, bool &muVeto, bool &muOLR) const;

    void configFromFile(bool& property, const std::string& propname, TEnv& rEnv,
                        bool defaultValue);
    void configFromFile(double& property, const std::string& propname, TEnv& rEnv,
                        double defaultValue);
    void configFromFile(int& property, const std::string& propname, TEnv& rEnv,
                        int defaultValue);
    void configFromFile(std::string& property, const std::string& propname, TEnv& rEnv,
                        const std::string& defaultValue);

    //little helpers for WP configurations / handling
    bool check_isOption(const std::string& wp, const std::vector<std::string>& list) const;
    bool check_isTighter(const std::string& wp1, const std::string& wp, const std::vector<std::string>& list) const;
  
    std::string EG_WP(const std::string& wp) const; //translate our WPs to make egamma selectors happy

    std::vector<std::string> getElSFkeys(const std::string& mapFile) const;

#ifdef XAOD_STANDALONE // more convenient for property setting
    DataSource m_dataSource;
    xAOD::JetInput::Type m_jetInputType; // permit switching between LC, PFlow, EM jets
#else
    int m_dataSource;
    int m_jetInputType;
#endif
    std::string m_configFile;

    bool m_force_noElId;
    bool m_force_noMuId;
    bool m_doTTVAsf;

    int m_jesNPset;
    bool m_useBtagging;
    bool m_debug;

    bool m_strictConfigCheck;

    std::string m_badJetCut;

    std::string m_fatJetUncConfig;
    std::string m_fatJetUncVars;

    bool m_tool_init;
    bool m_subtool_init;

    std::string m_eleTerm    ;
    std::string m_gammaTerm  ;
    std::string m_tauTerm    ;
    std::string m_jetTerm    ;
    std::string m_muonTerm   ;
    std::string m_inputMETSuffix;
    std::string m_inputMETMap;
    std::string m_inputMETCore;
    std::string m_inputMETRef;
    std::string m_outMETTerm;
    bool m_metRemoveOverlappingCaloTaggedMuons;
    bool m_metDoSetMuonJetEMScale;
    bool m_metDoMuonJetOR;

    bool m_metGreedyPhotons;

    bool m_trkMETsyst;
    bool m_caloMETsyst;

    std::vector<std::string> m_prwConfFiles;
    std::vector<std::string> m_prwLcalcFiles;
    double m_muUncert;

    // bookkeep supported configurations (in increasing order of tightness)
    std::vector<std::string> el_id_support;
    std::vector<std::string> ph_id_support;
    int mu_id_support; //max value

    // strings needed for dealing with 2015+2016 electron trigger SFs
    std::string m_electronTriggerSFStringSingle;
    std::string m_electronTriggerSFStringDiLepton;
    std::string m_electronTriggerSFStringMixedLepton;

    std::vector<std::string> tau_trig_support;
 
    std::string m_eleId;
    std::string m_eleIdBaseline;
    int         m_muId;
    int         m_muIdBaseline;
    std::string m_photonId;
    std::string m_photonIdBaseline;
    std::string m_tauId;
    std::string m_tauIdBaseline;
    bool        m_tauMVACalib; //!< Use the MVA calibration for taus
    bool        m_tauIDrecalc; //!< Recalculate TauID definition (20.7.8.2 bugfix) 
    std::string m_eleIso_WP;
    std::string m_eleChID_WP;
    bool        m_runECIS; //run ChargeIDSelector if valid WP was selected
    std::string m_photonIso_WP;
    std::string m_muIso_WP;
    std::string m_BtagWP;
    std::string m_BtagTagger;
    std::string m_BtagSystStrategy;


    //configurable cuts here
    double m_eleBaselinePt;
    double m_eleBaselineEta;
    bool   m_eleBaselineCrackVeto;
    double m_elePt;
    double m_eleEta;
    bool   m_eleCrackVeto;
    double m_eled0sig;
    double m_elez0;
    double m_elebaselined0sig;
    double m_elebaselinez0;

    double m_muBaselinePt;
    double m_muBaselineEta;
    double m_muPt;
    double m_muEta;
    double m_mud0sig;
    double m_muz0;
    double m_mubaselined0sig;
    double m_mubaselinez0;
    bool m_murequirepassedHighPtCuts;

    double m_muCosmicz0;
    double m_muCosmicd0;

    double m_badmuQoverP;

    double m_photonBaselinePt;
    double m_photonBaselineEta;
    double m_photonEta;
    double m_photonPt;

    bool   m_photonBaselineCrackVeto;
    bool   m_photonCrackVeto;
    bool   m_photonAllowLate;

    double m_tauPrePtCut;
    double m_tauPt;
    double m_tauEta;
    std::string m_tauConfigPath;
    std::string m_tauConfigPathBaseline;
    bool m_tauDoTTM;
    bool m_tauRecalcOLR;

    double m_jetPt;
    double m_jetEta;
    double m_jetJvt;
    std::string m_JVT_WP;

    bool   m_doFwdJVT;
    double m_fwdjetEtaMin;
    double m_fwdjetPtMax;
    bool   m_fwdjetTightOp;

    std::string m_JMScalib;

    /// Overlap removal options
    bool m_orDoTau;
    bool m_orDoPhoton;
    bool m_orDoEleJet;
    bool m_orDoMuonJet;
    bool m_orDoBjet;
    bool m_orDoElBjet;
    bool m_orDoMuBjet;
    bool m_orDoTauBjet;
    bool m_orDoBoostedElectron;
    double m_orBoostedElectronC1;
    double m_orBoostedElectronC2;
    double m_orBoostedElectronMaxConeSize;
    bool m_orDoBoostedMuon;
    double m_orBoostedMuonC1;
    double m_orBoostedMuonC2;
    double m_orBoostedMuonMaxConeSize;
    bool   m_orApplyRelPt;
    double m_orMuJetPtRatio;
    double m_orMuJetTrkPtRatio;
    double m_orMuJetInnerDR;
    bool m_orDoMuonJetGhostAssociation;
    bool m_orRemoveCaloMuons;
    std::string m_orBtagWP;
    std::string m_orInputLabel;

    bool m_orDoFatjets;
    double m_EleFatJetDR;
    double m_JetFatJetDR;

    bool m_doIsoSignal;
    bool m_doElIsoSignal;
    bool m_doPhIsoSignal;
    bool m_doMuIsoSignal;
    bool m_doIsoCloseByOR;

    std::string m_metJetSelection;

    int  m_showerType;

    std::string m_defaultJets;
    std::string m_fatJets;

    CP::SystematicSet m_defaultSyst = CP::SystematicSet();
    CP::SystematicSet m_currentSyst;

    std::string m_EG_corrModel;
    bool m_applyJVTCut;

    std::string m_bTaggingCalibrationFilePath;
    
    asg::AnaToolHandle<IJetCalibrationTool> m_jetCalibTool;
    asg::AnaToolHandle<IJetCalibrationTool> m_jetFatCalibTool;
    asg::AnaToolHandle<IJERTool> m_jerTool;
    asg::AnaToolHandle<IJERSmearingTool> m_jerSmearingTool;
    asg::AnaToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesTool;
    asg::AnaToolHandle<ICPJetUncertaintiesTool> m_fatjetUncertaintiesTool;
    asg::AnaToolHandle<IJetSelector> m_jetCleaningTool;
    asg::AnaToolHandle<IJetUpdateJvt> m_jetJvtUpdateTool;
    asg::AnaToolHandle<IJetModifier> m_jetFwdJvtTool;
    asg::AnaToolHandle<CP::IJetJvtEfficiency> m_jetJvtEfficiencyTool;
    
    std::string m_WtagConfig;
    std::string m_ZtagConfig;
    asg::AnaToolHandle<IJetSelector> m_WTaggerTool;
    asg::AnaToolHandle<IJetSelector> m_ZTaggerTool;

    //
    asg::AnaToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
    asg::AnaToolHandle<CP::IMuonSelectionTool> m_muonSelectionHighPtTool;
    asg::AnaToolHandle<CP::IMuonSelectionTool> m_muonSelectionToolBaseline;
    asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_muonCalibrationAndSmearingTool;
    asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEfficiencySFTool;
    asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEfficiencyBMHighPtSFTool;
    asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonTTVAEfficiencySFTool;
    asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonIsolationSFTool;
    asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerSFTool;
    //
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_elecEfficiencySFTool_reco;
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_elecEfficiencySFTool_id;
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_elecEfficiencySFTool_trig_singleLep;
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_elecEfficiencySFTool_trig_diLep;
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_elecEfficiencySFTool_trig_mixLep;
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_elecEfficiencySFTool_iso;
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_elecEfficiencySFTool_chf;
    //
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_elecEfficiencySFTool_trigEff_singleLep;
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_elecEfficiencySFTool_trigEff_diLep;
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_elecEfficiencySFTool_trigEff_mixLep;
    //
    asg::AnaToolHandle<CP::IEgammaCalibrationAndSmearingTool> m_egammaCalibTool;
    asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_elecSelLikelihood;
    asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_elecSelLikelihoodBaseline;
    asg::AnaToolHandle<IAsgPhotonIsEMSelector>     m_photonSelIsEM;
    asg::AnaToolHandle<IAsgPhotonIsEMSelector>     m_photonSelIsEMBaseline;
    asg::AnaToolHandle<IAsgPhotonEfficiencyCorrectionTool> m_photonEfficiencySFTool;
    asg::AnaToolHandle<IAsgPhotonEfficiencyCorrectionTool> m_photonIsolationSFTool;
    asg::AnaToolHandle<IElectronPhotonShowerShapeFudgeTool> m_electronPhotonShowerShapeFudgeTool;
    asg::AnaToolHandle<IEGammaAmbiguityTool> m_egammaAmbiguityTool;
    asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_elecChargeIDSelectorTool;
    asg::AnaToolHandle<CP::IEfficiencyScaleFactorTool>      m_elecChargeEffCorrTool;
    //
    asg::AnaToolHandle<TauAnalysisTools::ITauSelectionTool> m_tauSelTool;
    asg::AnaToolHandle<TauAnalysisTools::ITauSelectionTool> m_tauSelToolBaseline;
    asg::AnaToolHandle<TauAnalysisTools::ITauSmearingTool> m_tauSmearingTool;
    asg::AnaToolHandle<TauAnalysisTools::ITauTruthMatchingTool> m_tauTruthMatch;                                                                                             
    asg::AnaToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> m_tauEffTool;
    asg::AnaToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> m_tauTrigEffTool0;
    asg::AnaToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> m_tauTrigEffTool1;
    asg::AnaToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> m_tauTrigEffTool2;
    asg::AnaToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> m_tauTrigEffTool3;
    asg::AnaToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> m_tauTrigEffTool4;
    asg::AnaToolHandle<TauAnalysisTools::ITauOverlappingElectronLLHDecorator> m_tauElORdecorator;
    asg::AnaToolHandle<ITauToolBase> m_tauWPdecorator;
    //
    asg::AnaToolHandle<IBTaggingEfficiencyTool> m_btagEffTool;
    asg::AnaToolHandle<IBTaggingSelectionTool> m_btagSelTool;
    asg::AnaToolHandle<IBTaggingSelectionTool> m_btagSelTool_OR;
    //
    asg::AnaToolHandle<IMETMaker> m_metMaker;
    asg::AnaToolHandle<IMETSystematicsTool> m_metSystTool;
    //
    asg::AnaToolHandle<TrigConf::ITrigConfigTool> m_trigConfTool;
    asg::AnaToolHandle<Trig::TrigDecisionTool> m_trigDecTool;
    asg::AnaToolHandle<Trig::IMatchingTool> m_trigMatchingTool;
    //
    asg::AnaToolHandle<CP::IIsolationCorrectionTool> m_isoCorrTool;
    asg::AnaToolHandle<CP::IIsolationSelectionTool> m_isoTool;
    asg::AnaToolHandle<CP::IIsolationCloseByCorrectionTool> m_isoCloseByTool;
    //
    asg::AnaToolHandle<CP::IPileupReweightingTool> m_prwTool;
    //
    asg::AnaToolHandle<ORUtils::IOverlapTool> m_tauJetORtool;
    ORUtils::ToolBox m_orToolbox;
    //
    asg::AnaToolHandle<IWeightTool> m_pmgSHnjetWeighter;
    asg::AnaToolHandle<IWeightTool> m_pmgSHnjetWeighterWZ;
    //

  }; // Class SUSYObjDef_xAOD

  // decorators
  const static SG::AuxElement::Decorator<char> dec_baseline("baseline");
  const static SG::AuxElement::Decorator<char> dec_selected("selected"); //for priority-aware OR of baseline objects
  const static SG::AuxElement::Decorator<char> dec_signal("signal");
  const static SG::AuxElement::Decorator<char> dec_isol("isol");
  const static SG::AuxElement::Decorator<char> dec_passOR("passOR");
  const static SG::AuxElement::Decorator<double> dec_effscalefact("effscalefact");
  const static SG::AuxElement::Decorator<char> dec_signal_less_JVT("signal_less_JVT"); //!< Decorator for signal jets without a JVT requirement
  // const accessors for reading decorations that we set
  const static SG::AuxElement::ConstAccessor<char> acc_baseline("baseline");
  const static SG::AuxElement::ConstAccessor<char> acc_selected("selected"); //for priority-aware OR of baseline objects
  const static SG::AuxElement::ConstAccessor<char> acc_signal("signal");
  const static SG::AuxElement::ConstAccessor<char> acc_isol("isol");
  const static SG::AuxElement::ConstAccessor<char> acc_passOR("passOR");
  const static SG::AuxElement::ConstAccessor<char> acc_signal_less_JVT("signal_less_JVT"); //!< Accessor for signal jets without a JVT requirement
  const static SG::AuxElement::ConstAccessor<char> acc_trigmatched("trigmatched");
  // more decorations that are set externally
  const static SG::AuxElement::ConstAccessor<unsigned int> acc_OQ("OQ");
  const static SG::AuxElement::ConstAccessor<int> acc_truthType("truthType");
  const static SG::AuxElement::ConstAccessor<int> acc_truthOrigin("truthOrigin");
  const static SG::AuxElement::ConstAccessor<int> acc_bkgTruthOrigin("bkgTruthOrigin");
  const static SG::AuxElement::ConstAccessor<unsigned int> randomrunnumber("RandomRunNumber");

} // namespace ST


#endif // not SUSYTOOLS_SUSYOBJDEF_XAOD_H
