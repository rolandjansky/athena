// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SUSYTOOLS_SUSYOBJDEF_XAODTOOL_H
#define SUSYTOOLS_SUSYOBJDEF_XAODTOOL_H

// Framework include(s) -- base class
#include "AsgTools/IAsgTool.h"

// EDM include(s):
// Note that these are type defs, so we must include headers here
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/Vertex.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthParticleContainer.h"

// Needed for jet functions (take a shallow copy)
#include "xAODCore/ShallowCopy.h"

// For the SystInfo struct
#include "PATInterfaces/SystematicSet.h"

// For the TrigDefs
#include "TrigDecisionInterface/Conditions.h"

// For string search
#include "TString.h"
#include "TRegexp.h"

// System includes
#include <iostream> // For warnings in static functions
#include <vector>
#include <string>

// Forward declarations
namespace Trig {
  class ChainGroup;
}

namespace ST {

  struct SystInfo{
    CP::SystematicSet systset;
    bool affectsKinematics;
    bool affectsWeights;
    unsigned int affectsType;
    std::set<unsigned int> affectedWeights;
  };

  // Define a more compact enum than the IParticle one
  enum SystObjType {
    Unknown = 0,
    Jet,
    Egamma,
    Electron,
    Photon,
    Muon,
    Tau,
    BTag,
    MET_TST,
    MET_CST,
    MET_Track,
    EventWeight
  };


  // Define types of weights
  // This doesn't work with enums: class enums can't be assigned to an std::int8_t easily
  // Normal enums means redefinitions of e.g. "Trigger" and "Isolation"
  namespace Weights { 
    static const unsigned int Unknown = 0;
    
    namespace Jet {
      static const unsigned int Btag = 1001;
      static const unsigned int JVT = 1002;
    }
    
    namespace Muon {
      static const unsigned int Reconstruction = 1101;
      static const unsigned int Isolation = 1102;
      static const unsigned int ID = 1103;
      static const unsigned int Trigger = 1104;
    }
    
    namespace Electron {
      static const unsigned int Reconstruction = 1201;
      static const unsigned int Isolation = 1202;
      static const unsigned int ID = 1203;
      static const unsigned int Trigger = 1204;
      static const unsigned int ChargeID = 1205;
    }
    
    namespace Tau {
      static const unsigned int Reconstruction = 1301;
      static const unsigned int Trigger = 1302;
    }
    
    namespace Photon {
      static const unsigned int Reconstruction = 1401;
      static const unsigned int Isolation = 1402;
      static const unsigned int ID = 1403;
      static const unsigned int Trigger = 1404;
    }
  }
  
  static const double DUMMYDEF = -999.;

  // Helper method for affected objects
  static inline bool testAffectsObject(xAOD::Type::ObjectType type, unsigned int test) {
    switch(test) {
    case Jet:      return (type==xAOD::Type::Jet);
    case Egamma:   return (type==xAOD::Type::Electron||type==xAOD::Type::Photon);
    case Electron: return (type==xAOD::Type::Electron);
    case Photon:   return (type==xAOD::Type::Photon);
    case Muon:     return (type==xAOD::Type::Muon);
    case Tau:      return (type==xAOD::Type::Tau);
    case BTag:     return (type==xAOD::Type::BTag);
    default: break;
    }
    return false;
  }

  // Helper method for affected objects
  static inline std::string testAffectsObject(unsigned int test) {
    switch(test) {
    case Jet:      return "Jet";
    case Egamma:   return "Egamma";
    case Electron: return "Electron";
    case Photon:   return "Photon";
    case Muon:     return "Muon";
    case Tau:      return "Tau";
    case MET_TST:
    case MET_CST:
    case MET_Track: return "MET";
    case EventWeight: return "EventWeight";
    default:       break;
    }
    return "Unknown";
  }

  static inline int getMCShowerType(const std::string& sample_name) {
    /** Get MC generator index for the b-tagging efficiency maps*/
    // This needs VERY careful syncing with m_showerType in SUSYToolsInit!  Change with care!
    const static std::vector<TString> gen_mc_generator_keys = {"PYTHIA8EVTGEN"};
    //This was the 20.7 vector... {"PYTHIAEVTGEN", "HERWIGPPEVTGEN", "PYTHIA8EVTGEN", "SHERPA_CT10", "SHERPA"};

    //pre-process sample name
    TString tmp_name(sample_name);
    tmp_name.ReplaceAll("Py8EG","PYTHIA8EVTGEN");
    if(tmp_name.Contains("Pythia") && !tmp_name.Contains("Pythia8") && !tmp_name.Contains("EvtGen")) tmp_name.ReplaceAll("Pythia","PYTHIA8EVTGEN");
    if(tmp_name.Contains("Pythia8") && !tmp_name.Contains("EvtGen")) tmp_name.ReplaceAll("Pythia8","PYTHIA8EVTGEN");

    //capitalize the entire sample name
    tmp_name.ToUpper();

    //find shower type in name
    unsigned int ishower = 0;
    for( const auto & gen : gen_mc_generator_keys ){
      if( tmp_name.Contains(gen) ){ return ishower; }
      ishower++;
    }

    // See if they are doing something really unwise, just in case
    TRegexp is_data("^data1[5-9]_13TeV");
    if (tmp_name.Contains(is_data)){
      std::cout << "ST::getMCShowerType WARNING: Asking for the MC shower when running on a data file is not advised.  Just returning 0." << std::endl;
      return 0;
    }

    std::cout << "ST::getMCShowerType WARNING: Unknown MC generator detected. Returning default 0=PowhegPythia8(410501) ShowerType for btagging MC/MC maps." << std::endl;
    return 0;
  }


  // Simple interface
  //
  // Following the design principles outlined in the TF3 recommendations.
  //
  //
  class ISUSYObjDef_xAODTool : public virtual asg::IAsgTool {
	
    // Declare the interface that the class provides
    ASG_TOOL_INTERFACE( ST::ISUSYObjDef_xAODTool )
	    
    public:
    virtual StatusCode readConfig() = 0;

    virtual int getMCShowerType(const std::string& sample_name) const = 0;

    // For checking the origin of the input
    virtual bool isData() const = 0;
    virtual bool isAtlfast() const = 0;

    // override the AsgTool setProperty function for booleans
    virtual StatusCode setBoolProperty(const std::string& name, const bool& property) = 0;

    // autoconfiguration of pileup-reweighting tool
    virtual StatusCode autoconfigurePileupRWTool() = 0;

    // Apply the correction on a modifyable object
    virtual StatusCode FillMuon(xAOD::Muon& input, const float ptcut, const float etacut) = 0;
    virtual StatusCode FillJet(xAOD::Jet& input, const bool doCalib = true, const bool isFat = false) = 0;
    virtual StatusCode FillTau(xAOD::TauJet& input) = 0;
    virtual StatusCode FillElectron(xAOD::Electron& input, const float etcut, const float etacut) = 0;
    virtual StatusCode FillPhoton(xAOD::Photon& input, const float ptcut, const float etacut) = 0;

    virtual const xAOD::Vertex* GetPrimVtx() const = 0;
		
    virtual StatusCode GetJets(xAOD::JetContainer*& copy,xAOD::ShallowAuxContainer*& copyaux,const bool recordSG=true, const std::string& jetkey="", const xAOD::JetContainer* containerToBeCopied = 0) = 0;
    virtual StatusCode GetJetsSyst(const xAOD::JetContainer& calibjets,xAOD::JetContainer*& copy,xAOD::ShallowAuxContainer*& copyaux, const bool recordSG=true, const std::string& jetkey="") = 0;
    virtual StatusCode GetFatJets(xAOD::JetContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, const bool recordSG = false, const std::string& jetkey = "", const bool doLargeRdecorations = false, const xAOD::JetContainer* containerToBeCopied = 0) = 0;
    virtual StatusCode GetTaus(xAOD::TauJetContainer*& copy,xAOD::ShallowAuxContainer*& copyaux,const bool recordSG=true, const std::string& taukey="TauJets", const xAOD::TauJetContainer* containerToBeCopied = 0) = 0;
    virtual StatusCode GetMuons(xAOD::MuonContainer*& copy,xAOD::ShallowAuxContainer*& copyaux,const bool recordSG=true, const std::string& muonkey="Muons", const xAOD::MuonContainer* containerToBeCopied = 0) = 0;
    virtual StatusCode GetElectrons(xAOD::ElectronContainer*& copy,xAOD::ShallowAuxContainer*& copyaux,const bool recordSG=true,const std::string& elekey="Electrons", const xAOD::ElectronContainer* containerToBeCopied = 0) = 0;
    virtual StatusCode GetPhotons(xAOD::PhotonContainer*& copy,xAOD::ShallowAuxContainer*& copyaux,const bool recordSG=true,const std::string& photonkey="Photons", const xAOD::PhotonContainer* containerToBeCopied = 0) = 0;
    virtual StatusCode GetMET(xAOD::MissingETContainer& met,
			      const xAOD::JetContainer* jet,
			      const xAOD::ElectronContainer* elec = 0,
			      const xAOD::MuonContainer* muon = 0,
			      const xAOD::PhotonContainer* gamma = 0,
			      const xAOD::TauJetContainer* taujet = 0,
			      bool doTST=true, bool doJVTCut=true,
			      const xAOD::IParticleContainer* invis = 0) = 0;

    virtual StatusCode GetTrackMET(xAOD::MissingETContainer& met,
				   const xAOD::JetContainer* jet,
				   const xAOD::ElectronContainer* elec = 0,
				   const xAOD::MuonContainer* muon = 0
				   // const xAOD::PhotonContainer* gamma = 0,
				   // const xAOD::TauJetContainer* taujet = 0,
				   ) = 0;

    virtual bool IsSignalJet(const xAOD::Jet& input,  const float ptcut, const float etacut) const = 0;

    virtual bool IsBadJet(const xAOD::Jet& input) const = 0;

    virtual bool IsBJetLoose(const xAOD::Jet& input) const = 0; 
    virtual bool JetPassJVT(xAOD::Jet& input, bool update_jvt) = 0;

    virtual bool IsHighPtMuon(const xAOD::Muon& input) const = 0;

    virtual bool IsSignalMuon(const xAOD::Muon& input, const float ptcut, const float d0sigcut, const float z0cut, const float etacut = DUMMYDEF) const = 0;

    virtual bool IsSignalElectron(const xAOD::Electron& input, const float etcut, const float d0sigcut, const float z0cut, const float etacut = DUMMYDEF) const = 0;
 
    virtual bool IsCosmicMuon(const xAOD::Muon& input,const float z0cut, const float d0cut) const = 0;

    virtual bool IsSignalTau(const xAOD::TauJet& input, const float ptcut, const float etacut) const = 0;

    virtual bool IsBadMuon(const xAOD::Muon& input, const float qopcut) const = 0;

    virtual bool IsSignalPhoton(const xAOD::Photon& input, const float ptcut, const float etacut = DUMMYDEF) const = 0;

    virtual bool IsBJet(const xAOD::Jet& input) const = 0;

    virtual bool IsTruthBJet(const xAOD::Jet& input) const = 0;

    virtual int IsBJetContinuous(const xAOD::Jet& input) const = 0;

    virtual double JVT_SF(const xAOD::JetContainer* jets) = 0;

    virtual double JVT_SFsys(const xAOD::JetContainer* jets, const CP::SystematicSet& systConfig) = 0;

    virtual float BtagSF(const xAOD::JetContainer* jets) const = 0;

    virtual float BtagSFsys(const xAOD::JetContainer* jets, const CP::SystematicSet& systConfig) = 0;

    virtual float GetSignalMuonSF(const xAOD::Muon& mu, const bool recoSF = true, const bool isoSF = true, const bool doBadMuonHP = true, const bool warnOVR = true) = 0;

    virtual float GetSignalElecSF(const xAOD::Electron& el, const bool recoSF = true, const bool idSF = true, const bool triggerSF = true, const bool isoSF = true, const std::string& trigExpr = "e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose", const bool chfSF = false) = 0;

    virtual double GetEleTriggerEfficiency(const xAOD::Electron& el, const std::string& trigExpr = "e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose") const = 0; 

    virtual double GetEleTriggerEfficiencySF(const xAOD::Electron& el, const std::string& trigExpr = "e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose") const = 0; 


    virtual double GetMuonTriggerEfficiency(const xAOD::Muon& mu, const std::string& trigExpr = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", const bool isdata = false) = 0; 

    virtual double GetTotalMuonTriggerSF(const xAOD::MuonContainer& sfmuons, const std::string& trigExpr) = 0;

    virtual double GetTotalMuonSF(const xAOD::MuonContainer& muons, const bool recoSF = true, const bool isoSF = true, const std::string& trigExpr = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", const bool bmhptSF = true) = 0;

    virtual float GetTotalElectronSF(const xAOD::ElectronContainer& electrons, const bool recoSF = true, const bool idSF = true, const bool triggerSF = true, const bool isoSF = true, const std::string& trigExpr = "singleLepton", const bool chfSF = false) = 0;  // singleLepton == Ele.TriggerSFStringSingle value

    virtual double GetTotalMuonSFsys(const xAOD::MuonContainer& muons, const CP::SystematicSet& systConfig, const bool recoSF = true, const bool isoSF = true, const std::string& trigExpr = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", const bool bmhptSF = true) = 0;

    virtual float GetTotalElectronSFsys(const xAOD::ElectronContainer& electrons, const CP::SystematicSet& systConfig, const bool recoSF = true, const bool idSF = true, const bool triggerSF = true, const bool isoSF = true, const std::string& trigExpr = "singleLepton", const bool chfSF = false) = 0;   // singleLepton == Ele.TriggerSFStringSingle value

    virtual double GetSignalTauSF(const xAOD::TauJet& tau, const bool idSF = true, const bool triggerSF = true,  const std::string& trigExpr = "tau25_medium1_tracktwo") = 0;
  
    virtual double GetSignalTauSFsys(const xAOD::TauJet& tau, const CP::SystematicSet& systConfig, const bool idSF = true, const bool triggerSF = true,  const std::string& trigExpr = "tau25_medium1_tracktwo") = 0;

    virtual double GetTauTriggerEfficiencySF(const xAOD::TauJet& tau, const std::string& trigExpr = "tau25_medium1_tracktwo") = 0;

    virtual double GetTotalTauSF(const xAOD::TauJetContainer& taus, const bool idSF = true, const bool triggerSF = true, const std::string& trigExpr = "tau25_medium1_tracktwo") = 0;

    virtual double GetTotalTauSFsys(const xAOD::TauJetContainer& taus, const CP::SystematicSet& systConfig, const bool idSF = true, const bool triggerSF = true, const std::string& trigExpr = "tau25_medium1_tracktwo") = 0;

    virtual double GetSignalPhotonSF(const xAOD::Photon& ph, const bool effSF = true, const bool isoSF = true) const = 0;

    virtual double GetSignalPhotonSFsys(const xAOD::Photon& ph, const CP::SystematicSet& systConfig, const bool effSF = true, const bool isoSF = true) = 0;

    virtual double GetTotalPhotonSF(const xAOD::PhotonContainer& photons, const bool effSF = true, const bool isoSF = true) const = 0;

    virtual double GetTotalPhotonSFsys(const xAOD::PhotonContainer& photons, const CP::SystematicSet& systConfig, const bool effSF = true, const bool isoSF = true) = 0;

    virtual double GetTotalJetSF(const xAOD::JetContainer* jets, const bool btagSF = true, const bool jvtSF = true) = 0;

    virtual double GetTotalJetSFsys(const xAOD::JetContainer* jets, const CP::SystematicSet& systConfig, const bool btagSF = true, const bool jvtSF = true) = 0;

    virtual bool IsMETTrigPassed(unsigned int runnumber = 0, bool j400_OR = false) const = 0;
    virtual bool IsMETTrigPassed(const std::string& triggerName, bool j400_OR = false) const = 0;

    virtual bool IsTrigPassed(const std::string&, unsigned int condition=TrigDefs::Physics) const = 0;

    virtual bool IsTrigMatched(const xAOD::IParticle *part, const std::string& tr_item) = 0;
    virtual bool IsTrigMatched(const xAOD::IParticle *part1, const xAOD::IParticle *part2, const std::string& tr_item) = 0;
    virtual bool IsTrigMatched(const std::vector<const xAOD::IParticle*>& v, const std::string& tr_item) = 0;
    virtual bool IsTrigMatched(const std::initializer_list<const xAOD::IParticle*> &v, const std::string& tr_item) = 0;
  
    virtual void TrigMatch(const xAOD::IParticle* p, std::initializer_list<std::string>::iterator, std::initializer_list<std::string>::iterator) = 0;
    virtual void TrigMatch(const xAOD::IParticle* p, const std::vector<std::string>& items) = 0;
    virtual void TrigMatch(const xAOD::IParticle* p, const std::initializer_list<std::string>& items) = 0;
    virtual void TrigMatch(const xAOD::IParticleContainer* v, const std::vector<std::string>& items) = 0;
    virtual void TrigMatch(const xAOD::IParticleContainer* v, const std::initializer_list<std::string>& items) = 0;
    virtual void TrigMatch(const std::initializer_list<const xAOD::IParticle*>& v, const std::vector<std::string>& items) = 0;
    virtual void TrigMatch(const std::initializer_list<const xAOD::IParticle*>& v, const std::initializer_list<std::string>& items) = 0;
    virtual void TrigMatch(const xAOD::IParticle* p, const std::string& item) = 0;
    virtual void TrigMatch(const xAOD::IParticleContainer* v,  const std::string& item) = 0;
    virtual void TrigMatch(const std::initializer_list<const xAOD::IParticle*> &v, const std::string& item) = 0;


    virtual float GetTrigPrescale(const std::string&) const = 0;

    virtual const Trig::ChainGroup* GetTrigChainGroup(const std::string&) const = 0;

    virtual float GetPileupWeight() = 0;
    
    virtual ULong64_t GetPileupWeightHash( ) = 0;

    virtual float GetDataWeight(const std::string&) = 0;
 
    virtual float GetCorrectedAverageInteractionsPerCrossing(bool includeDataSF=false) = 0;

    virtual double GetSumOfWeights(int channel) = 0;
    
    virtual unsigned int GetRandomRunNumber(bool muDependentRRN = true) = 0;

    virtual StatusCode ApplyPRWTool(bool muDependentRRN = true) = 0;
    
    virtual unsigned int GetRunNumber() const = 0;

    virtual int treatAsYear(const int runNumber=-1) const = 0;
     
    virtual StatusCode OverlapRemoval(const xAOD::ElectronContainer *electrons, const xAOD::MuonContainer *muons, const xAOD::JetContainer *jets,
				      const xAOD::PhotonContainer* gamma = 0, const xAOD::TauJetContainer* taujet = 0, const xAOD::JetContainer *fatjets = 0) = 0;

    virtual StatusCode NearbyLeptonCorrections(const xAOD::ElectronContainer *electrons = nullptr, const xAOD::MuonContainer *muons = nullptr) const = 0;

    virtual CP::SystematicCode resetSystematics() = 0;

    virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& systConfig ) = 0;

    virtual bool isPrompt(const xAOD::IParticle* part) const = 0;

    virtual StatusCode FindSusyHP(const xAOD::TruthParticleContainer *truthP, int& pdgid1, int& pdgid2, bool isTruth3 = false) const = 0;

    virtual StatusCode FindSusyHP(const xAOD::TruthEvent *truthE, int& pdgid1, int& pdgid2) const = 0;

    virtual std::vector<ST::SystInfo> getSystInfoList() const = 0;

    virtual std::string TrigSingleLep() const = 0;
    virtual std::string TrigDiLep() const = 0;
    virtual std::string TrigMixLep() const = 0;


    // Temporary function for Sherpa 2.2 V+jets n-jets reweighting 
    // (see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/CentralMC15ProductionList#NEW_Sherpa_v2_2_V_jets_NJet_rewe)
    virtual float getSherpaVjetsNjetsWeight() const = 0;
    virtual float getSherpaVjetsNjetsWeight(const std::string& jetContainer) const = 0;

    enum DataSource {
      Undefined = -1,
      Data,
      FullSim,
      AtlfastII
    };

  }; // class ISUSYObjDef_xAODTool
    
} // namespace ST

#endif // SUSYTOOLS_SUSYOBJDEF_XAODTOOL_H
