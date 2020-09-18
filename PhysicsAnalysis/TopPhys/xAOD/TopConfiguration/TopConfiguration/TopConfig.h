/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPCONFIGURATION_TOPCONFIG_H
#define ANALYSISTOP_TOPCONFIGURATION_TOPCONFIG_H

/**
 * @author John Morris <john.morris@cern.ch>
 *
 * @brief TopConfig
 *   A simple configuration that is NOT a singleton
 *
 **/

// System include(s):
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <utility>

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

// Persistent settings
#include "TopConfiguration/TopPersistentSettings.h"
// Tree Filter
#include "TopConfiguration/TreeFilter.h"

#include "TopDataPreparation/SampleXsection.h"

namespace top {
  class AodMetaDataAccess;
  class ConfigurationSettings;

  class TopConfig final {
  public:
    TopConfig();
    explicit TopConfig(const top::TopPersistentSettings* settings);
    virtual ~TopConfig() {}

    // Delete Standard constructors
    TopConfig(const TopConfig& rhs) = delete;
    TopConfig(TopConfig&& rhs) = delete;
    TopConfig& operator = (const TopConfig& rhs) = delete;

    // create settings to persist to mini-xAOD
    top::TopPersistentSettings* createPersistentSettings();

    void setConfigSettings(top::ConfigurationSettings* const& settings);

    // Prevent any more configuration
    void fixConfiguration();

    // Make all CP Tools?
    inline bool makeAllCPTools() const {return m_makeAllCPTools;}

    // GRL file
    void setGrlDir(const std::string& s);
    inline const std::string& grlDir() const {return m_grlDir;}
    void setGrlFile(const std::string& s);
    inline const std::vector<std::string> grlFile() const {return m_grlFile;}

    // TDP path
    void setTDPPath(const std::string& s);
    inline const std::string& getTDPPath() const {return m_topDataPreparationPath;}
    
    //showering algorithm
    void setShoweringAlgorithm(SampleXsection::showering in) { m_showeringAlgo=in; }
    SampleXsection::showering getShoweringAlgorithm() const {return m_showeringAlgo;}

    inline bool recomputeCPvars() const {return m_recomputeCPvars;}

    // What objects are we using
    inline bool usePhotons()    const {return m_usePhotons;}
    inline bool useElectrons()  const {return m_useElectrons;}
    inline bool useFwdElectrons()  const {return m_useFwdElectrons;}
    inline bool useMuons()      const {return m_useMuons;}
    inline bool useSoftMuons()      const {return m_useSoftMuons;}
    inline bool useTaus()       const {return m_useTaus;}
    inline bool useJets()       const {return m_useJets;}
    inline bool useLargeRJets() const {return m_useLargeRJets;}
    inline bool useTrackJets()  const {return m_useTrackJets;}
    inline bool useTracks()  const {return m_useTracks;}
    inline bool useJetGhostTrack()  const {return m_useJetGhostTrack;}
    inline bool useRCJets()     const {return m_useRCJets;}
    inline bool useVarRCJets()  const {return m_useVarRCJets;}

    inline bool useParticleFlowJets() const {return m_useParticleFlowJets;}

    inline bool useTruthParticles() const {return m_useTruthParticles;}
    inline bool useTruthElectrons()  const {return m_useTruthElectrons;}
    inline bool useTruthMuons()  const {return m_useTruthMuons;}
    inline bool useTruthPhotons()  const {return m_useTruthPhotons;}
    inline bool useTruthJets()  const {return m_useTruthJets;}
    inline bool useTruthLargeRJets()  const {return m_useTruthLargeRJets;}
    inline bool useTruthTaus()  const {return m_useTruthTaus;}
    inline bool useTruthMET()  const {return m_useTruthMET;}

    inline bool applyTTVACut() const {return m_applyTTVACut;}

    inline bool useAntiMuons() const {return m_useAntiMuons;};

    // MC or not MC - this should be set by metadata
    inline bool isMC() const {return m_isMC;}
    inline void setIsMC(const bool value) {
      if (!m_configFixed) {
        m_isMC = value;
      }
    }

    // AFII?
    inline bool isAFII() const {return m_isAFII;}
    inline void setIsAFII(const bool value) {
      if (!m_configFixed) {
        m_isAFII = value;
      }
    }

    // List of branches to be removed
    inline std::vector<std::string> filterBranches() const {return m_filterBranches;}
    inline void setFilterBranches(const std::vector<std::string>& value) {
      if (!m_configFixed) {
        m_filterBranches = value;
      }
    }
    
    // List of PartonLevel branches to be removed
    inline std::vector<std::string> filterPartonLevelBranches() const {return m_filterPartonLevelBranches;}
    inline void setFilterPartonLevelBranches(const std::vector<std::string>& value) {
      if (!m_configFixed) {
        m_filterPartonLevelBranches = value;
      }
    }
    
    // List of ParticleLevel branches to be removed
    inline std::vector<std::string> filterParticleLevelBranches() const {return m_filterParticleLevelBranches;}
    inline void setFilterParticleLevelBranches(const std::vector<std::string>& value) {
      if (!m_configFixed) {
        m_filterParticleLevelBranches = value;
      }
    }

    // List of NominalLoose branches to be removed
    inline std::vector<std::string> filterNominalLooseBranches() const {return m_filterNominalLooseBranches;}
    inline void setFilterNominalLooseBranches(const std::vector<std::string>& value) {
      if (!m_configFixed) {
        m_filterNominalLooseBranches = value;
      }
    }

    // Generators name
    inline std::string getGenerators() const {return m_generators;}
    inline void setGenerators(const std::string value) {
      if (!m_configFixed) {
        m_generators = value;
      }
    }

    // AMITag
    inline std::string getAMITag() const {return m_AMITag;}
    inline void setAMITag(const std::string value) {
      if (!m_configFixed) {
        m_AMITag = value;
      }
    }

    // Is this a Primary xAOD?
    inline bool isPrimaryxAOD() const {return m_isPrimaryxAOD;}
    inline void setIsPrimaryxAOD(const bool value) {
      if (!m_configFixed) {
        m_isPrimaryxAOD = value;
      }
    }

    // Is this a Truth DxAOD?
    inline bool isTruthDxAOD() const {return m_isTruthDxAOD;}
    inline void setIsTruthDxAOD(const bool value) {
      if (!m_configFixed) {
        m_isTruthDxAOD = value;
      }
    }

    // What derivation type is it?
    inline std::string getDerivationStream() const {return m_derivationStream;}
    inline void setDerivationStream(const std::string value) {
      if (!m_configFixed) {
        m_derivationStream = value;
      }
    }

    // AMI tag from metadata
    std::string const& getAmiTag() const;
    void setAmiTag(std::string const& amiTag);

    inline unsigned int getDSID() const {return m_DSID;}
    inline void setDSID(unsigned int value) {
      // Check here if this is a sherpa 2.2 V+jets sample
      // (see below to set DSIDs)
      setIsSherpa22Vjets(value);
      m_DSID = value;
    }

    // Is this Sherpa 2.2 V+jets?
    // (if so then we need to reweight based on num truth jets)
    inline bool isSherpa22Vjets() const {return m_is_sherpa_22_vjets;}
    inline void setIsSherpa22Vjets(unsigned int dsid) {
      // m_is_sherpa_22_vjets = false by default
      // Sherpa 2.2 W+jets (from TopDataPrep)
      if (dsid >= 363331 && dsid <= 363354) m_is_sherpa_22_vjets = true;
      if (dsid >= 363436 && dsid <= 363483) m_is_sherpa_22_vjets = true;
      // Sherpa 2.2 Z+jets (from TopDataPrep)
      if (dsid >= 363102 && dsid <= 363122) m_is_sherpa_22_vjets = true;
      if (dsid >= 363361 && dsid <= 363435) m_is_sherpa_22_vjets = true;
    }

    // map index for MC/MC b-tagging scale factors
    inline unsigned int getMapIndex() const {return m_MapIndex;}
    inline void setMapIndex(unsigned int value) {m_MapIndex = value;}

    // Run normal (Tight) selection and dumps the non-Loose trees
    // Default is true for both Data and MC
    inline bool doTightEvents() const {return m_doTightEvents;}
    // Run Loose selection and dumps the Loose trees
    // Default is true for Data and false for MC
    inline bool doLooseEvents() const {return m_doLooseEvents;}

    // Run systematics on Loose selection
    // Default is true
    inline bool doTightSysts() const {return m_doTightSysts;}
    // Run systematics on Loose selection
    // Default is true
    inline bool doLooseSysts() const {return m_doLooseSysts;}

    // Do fakes MM weight calculation using FakeBkgTools from IFF
    inline bool doFakesMMWeightsIFF() const {return m_doFakesMMWeightsIFF;}

    // Configurations for MM fake estimate using FakeBkgTools from IFF
    inline std::string FakesMMConfigIFF() const {return m_FakesMMConfigIFF;}

    // Debug mode for MM fake estimate using FakeBkgTools from IFF
    inline bool FakesMMIFFDebug() const {return m_doFakesMMIFFDebug;}

    // enables calculation of MM weights using FakeBkgTools from IFF
    // only possible for data loose
    // doing it on MC loose is explicitly forbidden
    inline void setFakesMMWeightsCalculationIFF()
    {m_doFakesMMWeightsIFF = true;}
    inline void setFakesMMConfigIFF(const std::string& configIFF)
    {m_FakesMMConfigIFF = configIFF;}
    inline void setFakesMMIFFDebug()
    {m_doFakesMMIFFDebug = true;}
    
    //by default AT uses always tight objects in MET re-building; these options allow for using loose objects instead in the loose analysis and/or in the nominal analysis
    inline void setUseLooseObjectsInMETInLooseTree() {if (!m_configFixed) m_useLooseObjectsInMETInLooseTree = true;}
    inline bool useLooseObjectsInMETInLooseTree() const {return m_useLooseObjectsInMETInLooseTree;}
    inline void setUseLooseObjectsInMETInNominalTree() {if (!m_configFixed) m_useLooseObjectsInMETInNominalTree = true;}
    inline bool useLooseObjectsInMETInNominalTree() const {return m_useLooseObjectsInMETInNominalTree;}
    //this will write a separate branch with the met built using loose objects
    inline void setWriteMETBuiltWithLooseObjects() {if (!m_configFixed) m_writeMETBuiltWithLooseObjects=true;}
    inline bool writeMETBuiltWithLooseObjects() {return m_writeMETBuiltWithLooseObjects;}

    // By default the top group does overlap removal on the tight lepton definitions
    // If you use this you are going off piste and need to report
    // your findings to the top reconstruction meeting
    inline void setOverlapRemovalOnLooseLeptonDef() {
      if (!m_configFixed) {
        m_doOverlapRemovalOnLooseLeptonDef = true;
      }
    }

    inline bool doOverlapRemovalOnLooseLeptonDef() const {return m_doOverlapRemovalOnLooseLeptonDef;}

    inline std::string overlapRemovalProcedure() const
    {return m_overlap_removal_procedure;}

    inline float overlapRemovalSlidingInnerDRel() const
    {return m_overlapRemovalSlidingInnerDRel;}

    inline float overlapRemovalSlidingInnerDRmu() const
    {return m_overlapRemovalSlidingInnerDRmu;}

    // do overlap removal also with large-R jets
    // (using whatever procedure is used in the official tools)
    inline void setLargeJetOverlapRemoval() {
      if (!m_configFixed) {
        m_doLargeJetOverlapRemoval = true;
      }
    }

    inline bool doLargeJetOverlapRemoval() const {return m_doLargeJetOverlapRemoval;}

    // In the *_Loose trees, lepton SFs are calculated considering
    // tight ID and isolation instead of loose
    // Only tight leptons are considered in the event SF calculation
    inline void setApplyTightSFsInLooseTree() {
      if (!m_configFixed) {
        m_applyTightSFsInLooseTree = true;
      }
    }

    inline bool applyTightSFsInLooseTree() const {return m_applyTightSFsInLooseTree;}

    // Apply Electron In Jet subtraction
    // For boosted analysis
    inline bool applyElectronInJetSubtraction() const {return m_applyElectronInJetSubtraction;}
    inline void setApplyElectronInJetSubtraction() {
      if (!m_configFixed) {
        m_applyElectronInJetSubtraction = true;
      }
    }

    // Dump truth block info
    inline bool doTruthBlockInfo() const {return m_doTruthBlockInfo;}
    inline void setTruthBlockInfo() {
      if (!m_configFixed) {
        m_doTruthBlockInfo = true;
      }
    }

    // Write truth PDF info
    inline bool doTruthPDFInfo() const {return m_doTruthPDFInfo;}
    inline void setTruthPDFInfo() {
      if (!m_configFixed) {
        m_doTruthPDFInfo = true;
      }
    }

    // Write truth PDF info in reco tree
    inline bool doTruthPDFInfoInNominalTrees() const {return m_doTruthPDFInfoInNominalTrees;}
    inline void setTruthPDFInfoInNominalTrees() {
      if (!m_configFixed) {
        m_doTruthPDFInfoInNominalTrees = true;
      }
    }

    // Write MC generator weights
    inline bool doMCGeneratorWeights() const {return m_doMCGeneratorWeights;}
    inline void setMCGeneratorWeights() {
      if (!m_configFixed) {
        m_doMCGeneratorWeights = true;
      }
    }

    // Write truth PDF info in reco tree
    inline bool doMCGeneratorWeightsInNominalTrees() const {return m_doMCGeneratorWeightsInNominalTrees;}
    inline void setMCGeneratorWeightsInNominalTrees() {
      if (!m_configFixed) {
        m_doMCGeneratorWeightsInNominalTrees = true;
      }
    }

    inline const std::vector<std::string>& nominalWeightNames() const {return m_nominalWeightNames;}
    inline void setNominalWeightName(const std::string& name) {
      m_nominalWeightName = name;
    }
    inline void setNominalWeightIndex(size_t index) {
      m_nominalWeightIndex = index;
    }
    inline std::string nominalWeightName() const {return m_nominalWeightName;}
    inline size_t nominalWeightIndex() const {return m_nominalWeightIndex;}
    inline bool forceNominalWeightFallbackIndex() const {return m_forceWeightIndex;}

    inline void setMCweightsVectorSize(size_t weights_size) {
      m_MCweightsSize = weights_size;
    }

    inline size_t MCweightsVectorSize() const {return m_MCweightsSize;}

    // Top Parton History
    inline bool doTopPartonHistory() const {return m_doTopPartonHistory;}
    inline void setTopPartonHistory() {
      if (!m_configFixed) {
        m_doTopPartonHistory = true;
      }
    }

    // Register Top Parton History in flat ntuple
    // This doesn't use m_configFixed - this is bad :(
    // This is a simple implementation of saving xAOD objects to ntuples
    // John has more advanced ideas on how to do this
    // This is just a quick fix for now to get the concept working
    inline bool isTopPartonHistoryRegisteredInNtuple() const {return m_isTopPartonHistoryRegisteredInNtuple;}
    inline void setTopPartonHistoryRegisteredInNtuple()
    {m_isTopPartonHistoryRegisteredInNtuple = true;}
    
    inline bool doTopPartonLevel() const {return m_doTopPartonLevel;}
    inline void setTopPartonLevel(bool in) {
      if (!m_configFixed) {
        m_doTopPartonLevel = in;
      }
    }

    // TopParticleLevel
    inline bool doTopParticleLevel() const {return m_doTopParticleLevel;}
    inline void setTopParticleLevel(bool in) {
      if (!m_configFixed) {
        m_doTopParticleLevel = in;
      }
    }

    inline bool doParticleLevelOverlapRemovalMuJet() const {return m_doParticleLevelOverlapRemovalMuJet;}
    inline bool doParticleLevelOverlapRemovalElJet() const {return m_doParticleLevelOverlapRemovalElJet;}
    inline bool doParticleLevelOverlapRemovalJetPhoton() const {return m_doParticleLevelOverlapRemovalJetPhoton;}
    inline void setParticleLevelOverlapRemovalMuJet(bool value) {
      if (!m_configFixed) {
        m_doParticleLevelOverlapRemovalMuJet = value;
      }
    }

    inline void setParticleLevelOverlapRemovalElJet(bool value) {
      if (!m_configFixed) {
        m_doParticleLevelOverlapRemovalElJet = value;
      }
    }

    inline void setParticleLevelOverlapRemovalJetPhoton(bool value) {
      if (!m_configFixed) {
        m_doParticleLevelOverlapRemovalJetPhoton = value;
      }
    }

    // KLFitter
    inline bool doKLFitter() const {return m_doKLFitter;}
    inline void setKLFitter() {
      if (!m_configFixed) {
        m_doKLFitter = true;
      }
    }

    inline const std::string& KLFitterTransferFunctionsPath() const {return m_KLFitterTransferFunctionsPath;}
    inline const std::string& KLFitterOutput() const {return m_KLFitterOutput;}
    inline const std::string& KLFitterJetSelectionMode() const {return m_KLFitterJetSelectionMode;}
    inline const std::string& KLFitterBTaggingMethod() const {return m_KLFitterBTaggingMethod;}
    inline const std::string& KLFitterLH() const {return m_KLFitterLH;}

    inline bool KLFitterTopMassFixed() const {return m_KLFitterTopMassFixed;}
    inline bool KLFitterSaveAllPermutations() const {return m_KLFitterSaveAllPermutations;}

    inline bool KLFitterFailOnLessThanXJets() const {return m_KLFitterFailOnLessThanXJets;}
    // PseudoTop
    inline bool doPseudoTop() const {return m_doPseudoTop;}
    inline void setPseudoTop() {
      if (!m_configFixed) {
        m_doPseudoTop = true;
      }
    }

    // Triggers
    inline virtual void allTriggers_Tight(std::shared_ptr<std::unordered_map<std::string,
                                                                             std::vector<std::string> > > triggers) {
      if (!m_configFixed) {
        m_allTriggers_Tight = triggers;
      }
    }

    inline virtual void electronTriggers_Tight(std::shared_ptr<std::unordered_map<std::string,
                                                                                  std::vector<std::string> > > triggers) {
      if (!m_configFixed) {
        m_electronTriggers_Tight = triggers;
      }
    }

    inline virtual void muonTriggers_Tight(std::shared_ptr<std::unordered_map<std::string,
                                                                              std::vector<std::string> > > triggers) {
      if (!m_configFixed) {
        m_muonTriggers_Tight = triggers;
      }
    }

    inline virtual void tauTriggers_Tight(std::shared_ptr<std::unordered_map<std::string,
                                                                             std::vector<std::string> > > triggers) {
      if (!m_configFixed) {
        m_tauTriggers_Tight = triggers;
      }
    }

    inline virtual void allTriggers_Loose(std::shared_ptr<std::unordered_map<std::string,
                                                                             std::vector<std::string> > > triggers) {
      if (!m_configFixed) {
        m_allTriggers_Loose = triggers;
      }
    }

    inline virtual void electronTriggers_Loose(std::shared_ptr<std::unordered_map<std::string,
                                                                                  std::vector<std::string> > > triggers) {
      if (!m_configFixed) {
        m_electronTriggers_Loose = triggers;
      }
    }

    inline virtual void muonTriggers_Loose(std::shared_ptr<std::unordered_map<std::string,
                                                                              std::vector<std::string> > > triggers) {
      if (!m_configFixed) {
        m_muonTriggers_Loose = triggers;
      }
    }

    inline virtual void tauTriggers_Loose(std::shared_ptr<std::unordered_map<std::string,
                                                                             std::vector<std::string> > > triggers) {
      if (!m_configFixed) {
        m_tauTriggers_Loose = triggers;
      }
    }

    inline std::shared_ptr<std::vector<std::string> > allSelectionNames() const {return m_allSelectionNames;}
    virtual const std::vector<std::string>& allTriggers_Tight(const std::string& selection) const;
    virtual const std::vector<std::string>& electronTriggers_Tight(const std::string& selection) const;
    virtual const std::vector<std::string>& muonTriggers_Tight(const std::string& selection) const;
    virtual const std::vector<std::string>& tauTriggers_Tight(const std::string& selection) const;

    virtual const std::vector<std::string>& allTriggers_Loose(const std::string& selection) const;
    virtual const std::vector<std::string>& electronTriggers_Loose(const std::string& selection) const;
    virtual const std::vector<std::string>& muonTriggers_Loose(const std::string& selection) const;
    virtual const std::vector<std::string>& tauTriggers_Loose(const std::string& selection) const;

    // StoreGate Keys
    virtual void sgKeyMCParticle(const std::string& s);
    virtual void sgKeyPhotons(const std::string& s);
    virtual void sgKeyElectrons(const std::string& s);
    virtual void sgKeyFwdElectrons(const std::string& s);
    virtual void sgKeyMuons(const std::string& s);
    virtual void sgKeySoftMuons(const std::string& s);
    virtual void sgKeyTaus(const std::string& s);
    virtual void sgKeyJets(const std::string& s);
    virtual void sgKeyLargeRJets(const std::string& s);
    virtual void sgKeyTrackJets(const std::string& s);
    virtual void sgKeyTracks(const std::string& s);

    virtual void sgKeyTruthElectrons(const std::string& s);
    virtual void sgKeyTruthMuons(const std::string& s);
    virtual void sgKeyTruthPhotons(const std::string& s);
    virtual void sgKeyTruthMET(const std::string& s);
    virtual void sgKeyTruthJets(const std::string& s);
    virtual void sgKeyTruthLargeRJets(const std::string& s);
    virtual void sgKeyTruthTaus(const std::string& s);

    inline virtual void sgKeyMissingEt(const std::string& s) {
      if (!m_configFixed) {
        m_sgKeyMissingEt = s;
      }
    }

    inline virtual void sgKeyMissingEtLoose(const std::string& s) {
      if (!m_configFixed) {
        m_sgKeyMissingEtLoose = s;
      }
    }

    inline float jetResponseMatchingDeltaR() const {return m_jetResponseMatchingDeltaR;}
    inline void setJetResponseMatchingDeltaR(const float value) {m_jetResponseMatchingDeltaR = value;}

    inline const std::string& sgKeyEventInfo()  const {return m_sgKeyEventInfo;}
    inline const std::string& sgKeyPrimaryVertices() const {return m_sgKeyPrimaryVertices;}
    inline virtual const std::string& sgKeyPhotons()    const {return m_sgKeyPhotons;}
    inline virtual const std::string& sgKeyElectrons()  const {return m_sgKeyElectrons;}
    inline virtual const std::string& sgKeyFwdElectrons()  const {return m_sgKeyFwdElectrons;}
    inline virtual const std::string& sgKeyMuons()      const {return m_sgKeyMuons;}
    inline virtual const std::string& sgKeySoftMuons()      const {return m_sgKeySoftMuons;}
    inline virtual const std::string& sgKeyTaus()       const {return m_sgKeyTaus;}
    inline virtual const std::string& sgKeyJets()       const {return m_sgKeyJets;}
    inline virtual const std::string& sgKeyJetsType()   const {return m_sgKeyJetsType;}
    inline virtual const std::string& sgKeyLargeRJets() const {return m_sgKeyLargeRJets;}
    inline virtual const std::string& sgKeyTrackJets()  const {return m_sgKeyTrackJets;}
    inline virtual const std::string& sgKeyTracks()  const {return m_sgKeyTracks;}
    inline virtual const std::string& sgKeyTrackJetsType()  const {return m_sgKeyTrackJetsType;}
    inline virtual const std::string& sgKeyMissingEt()  const {return m_sgKeyMissingEt;}
    inline virtual const std::string& sgKeyMissingEtLoose()  const {return m_sgKeyMissingEtLoose;}
    inline const std::string& sgKeyInDetTrackParticles() const {return m_sgKeyInDetTrackParticles;}

    inline const std::string& sgKeyTruthEvent() const {return m_sgKeyTruthEvent;}
    inline virtual const std::string& sgKeyMCParticle() const {return m_sgKeyMCParticle;}
    inline const std::string& sgKeyTruthPhotons() const {return m_sgKeyTruthPhotons;}
    inline const std::string& sgKeyTruthElectrons() const {return m_sgKeyTruthElectrons;}
    inline const std::string& sgKeyTruthMuons() const {return m_sgKeyTruthMuons;}
    inline const std::string& sgKeyTruthTaus() const {return m_sgKeyTruthTaus;}
    inline virtual const std::string& sgKeyTruthJets()  const {return m_sgKeyTruthJets;}
    inline virtual const std::string& sgKeyTruthLargeRJets()  const {return m_sgKeyTruthLargeRJets;}
    inline const std::string& sgKeyTruthMET() const {return m_sgKeyTruthMET;}
    inline const std::string& sgKeyTopPartonHistory() const {return m_sgKeyTopPartonHistory;}

    inline const std::string& sgKeyTopSystematicEvents() const {return m_sgKeyTopSystematicEvents;}
    inline const std::string& sgKeyTopSystematicEventsLoose() const {return m_sgKeyTopSystematicEventsLoose;}
    inline const std::string& passEventSelectionDecoration() const {return m_passEventSelectionDecoration;}
    virtual void decoKeyJetGhostTrack(const std::string& key);
    inline virtual const std::string& decoKeyJetGhostTrack() const {return m_decoKeyJetGhostTrack;}
    virtual void runPeriodJetGhostTrack(const std::vector<std::uint32_t>& vect);
    virtual void runPeriodTrack(const std::vector<std::uint32_t>& vect);

    virtual const std::string& sgKeyPhotons(const std::size_t hash) const;
    virtual const std::string& sgKeyElectrons(const std::size_t hash) const;
    virtual const std::string& sgKeyFwdElectrons(const std::size_t hash) const;
    virtual const std::string& sgKeyElectronsStandAlone(const std::size_t hash) const;
    virtual const std::string& sgKeyFwdElectronsStandAlone(const std::size_t hash) const;
    virtual const std::string& sgKeyMuons(const std::size_t hash) const;
    virtual const std::string& sgKeySoftMuons(const std::size_t hash) const;
    virtual const std::string& sgKeyTaus
      (const std::size_t hash) const;
    virtual const std::string& sgKeyJets(const std::size_t hash, const bool useLooseLeptonJets) const;
    virtual const std::string& sgKeyJetsStandAlone(const std::size_t hash) const;
    virtual const std::string& sgKeyLargeRJets(const std::size_t hash) const;
    virtual const std::string& sgKeyTrackJets(const std::size_t hash) const;
    virtual const std::string& sgKeyTracks(const std::size_t hash) const;
    virtual const std::string& sgKeyMissingEt(const std::size_t hash) const;
    virtual const std::string& sgKeyMissingEtLoose(const std::size_t hash) const;

    // For top::TopEventMaker
    const std::string& sgKeyPhotonsTDS(const std::size_t hash) const;
    const std::string& sgKeyPhotonsTDSAux(const std::size_t hash) const;
    const std::string& sgKeyElectronsTDS(const std::size_t hash) const;
    const std::string& sgKeyElectronsTDSAux(const std::size_t hash) const;
    const std::string& sgKeyFwdElectronsTDS(const std::size_t hash) const;
    const std::string& sgKeyFwdElectronsTDSAux(const std::size_t hash) const;
    const std::string& sgKeyMuonsTDS(const std::size_t hash) const;
    const std::string& sgKeyMuonsTDSAux(const std::size_t hash) const;
    const std::string& sgKeySoftMuonsTDS(const std::size_t hash) const;
    const std::string& sgKeySoftMuonsTDSAux(const std::size_t hash) const;
    const std::string& sgKeyTausTDS(const std::size_t hash) const;
    const std::string& sgKeyTausTDSAux(const std::size_t hash) const;
    const std::string& sgKeyJetsTDS(const std::size_t hash, const bool useLooseLeptonJets) const;
    const std::string& sgKeyJetsTDSAux(const std::size_t hash, const bool useLooseLeptonJets) const;
    const std::string& sgKeyLargeRJetsTDS(const std::size_t hash) const;
    const std::string& sgKeyLargeRJetsTDSAux(const std::size_t hash) const;
    const std::string& sgKeyTrackJetsTDS(const std::size_t hash) const;
    const std::string& sgKeyTrackJetsTDSAux(const std::size_t hash) const;
    const std::string& sgKeyTracksTDS(const std::size_t hash) const;
    const std::string& sgKeyTracksTDSAux(const std::size_t hash) const;

    // KLFitter
    const std::string& sgKeyKLFitter(const std::size_t hash) const;
    const std::string& sgKeyKLFitterLoose(const std::size_t hash) const;

    // PseudoTop
    const std::string& sgKeyPseudoTop(const std::size_t hash) const;
    const std::string& sgKeyPseudoTop(const std::string) const;
    const std::string& sgKeyPseudoTopLoose(const std::size_t hash) const;
    const std::string& sgKeyPseudoTopLoose(const std::string) const;

    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    // Jet Ghost-Track Systematics and selection

    // The jet decoration name for a given systematic (nominal also possible).
    const std::string& decoKeyJetGhostTrack(const std::size_t hash) const;

    // Retrieve names of all (registered) systematics that affect ghost tracks.
    const std::vector<std::string>& systematicsJetGhostTrack() const {return m_jetGhostTrackSystematics;}

    // Retrieve run periods for ghost tracks.
    const std::vector<std::uint32_t>& runPeriodsJetGhostTrack() const {return m_jetGhostTrackRunPeriods;}

    // Retrieve run periods for tracks.                                                                                                                                                           
    const std::vector<std::uint32_t>& runPeriodsTrack() const {return m_trackRunPeriods;}

    // Retrieve mapping from systematic hash to CP::SystematicSet.
    inline std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > systMapJetGhostTrack()   const {
      return m_systMapJetGhostTrack;
    }
    
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


    // special: allow to dump the systematics-shifted b-tagging SFs in the systematics trees
    inline virtual void dumpBtagSystsInSystTrees(const bool& b) {
      if (!m_configFixed) {
        m_dumpBtagSystsInSystTrees = b;
      }
    }

    inline virtual const bool& dumpBtagSystsInSystTrees() {return m_dumpBtagSystsInSystTrees;}

    // special: store per-jet btag SGs
    inline virtual void storePerJetBtagSFs(const bool& b) {
      if (!m_configFixed) {
        m_storePerJetBtagSFs = b;
      }
    }

    inline virtual const bool& storePerJetBtagSFs() {return m_storePerJetBtagSFs;}

    // Electron configuration
    inline virtual void egammaSystematicModel(const std::string& s) {
      if (!m_configFixed) {
        m_egammaSystematicModel = s;
      }
    }

    inline virtual void electronEfficiencySystematicModel(const std::string& s) {
      if (!m_configFixed) {
        m_electronEfficiencySystematicModel = s;
      }
    }

    inline virtual void electronEfficiencySystematicModelEtaBinning(const std::string& s) {
      if (!m_configFixed) {
        m_electronEfficiencySystematicModelEtaBinning = s;
      }
    }

    inline virtual void electronEfficiencySystematicModelEtBinning(const std::string& s) {
      if (!m_configFixed) {
        m_electronEfficiencySystematicModelEtBinning = s;
      }
    }

    inline virtual void electronID(const std::string& s) {
      if (!m_configFixed) {
        m_electronID = s;
      }
    }

    inline virtual void electronIDLoose(const std::string& s) {
      if (!m_configFixed) {
        m_electronIDLoose = s;
      }
    }

    inline void removeElectronVetoLArCrack() {
      if (!m_configFixed) {
        m_electronVetoLArCrack = false;
      }
    }

    inline virtual void electronVetoLArCrack(const bool b) {
      if (!m_configFixed) {
        m_electronVetoLArCrack = b;
      }
    }

    inline virtual void electronPtcut(const float pt) {
      if (!m_configFixed) {
        m_electronPtcut = pt;
      }
    }

    inline virtual void electronIsolation(const std::string& iso) {
      if (!m_configFixed) {
        m_electronIsolation = iso;
      }
    }

    inline virtual void electronIsolationLoose(const std::string& iso) {
      if (!m_configFixed) {
        m_electronIsolationLoose = iso;
      }
    }

    void electronIsolationSF(std::string const& iso) {
      if (!m_configFixed) {
        m_electronIsolationSF = iso;
      }
    }

    void electronIsolationSFLoose(std::string const& iso) {
      if (!m_configFixed) {
        m_electronIsolationSFLoose = iso;
      }
    }

    inline virtual void useElectronChargeIDSelection(const std::string& s) {
      if (!m_configFixed) {
        m_useElectronChargeIDSelection = (s == "True" || s == "true");
      }
    }

    inline virtual void useEgammaLeakageCorrection(const std::string& s) {
      if (!m_configFixed) {
        m_useEgammaLeakageCorrection = (s == "True" || s == "true");
      }
    }

    inline virtual void enablePromptLeptonImprovedVetoStudies(const std::string& s) {
      if (!m_configFixed) {
	m_enablePromptLeptonImprovedVetoStudies = (s == "True" || s == "true");
      }
    }

    inline virtual const std::string& egammaSystematicModel() {return m_egammaSystematicModel;}
    inline virtual const std::string& electronEfficiencySystematicModel() {return m_electronEfficiencySystematicModel;}
    inline virtual const std::string& electronEfficiencySystematicModelEtaBinning() {
      return m_electronEfficiencySystematicModelEtaBinning;
    }
    inline virtual const std::string& electronEfficiencySystematicModelEtBinning() {
      return m_electronEfficiencySystematicModelEtBinning;
    }
    inline virtual const std::string& electronID()     const {return m_electronID;}
    inline virtual const std::string& electronIDLoose()  const {return m_electronIDLoose;}
    inline virtual bool electronVetoLArCrack() const {return m_electronVetoLArCrack;}
    inline virtual float electronPtcut()       const {return m_electronPtcut;}
    inline virtual const std::string& electronIsolation() const {return m_electronIsolation;}
    inline virtual const std::string& electronIsolationLoose() const {return m_electronIsolationLoose;}
    std::string const& electronIsolationSF() const {return m_electronIsolationSF;}
    std::string const& electronIsolationSFLoose() const {return m_electronIsolationSFLoose;}
    inline virtual bool electronIsoSFs() const {return m_electronIsoSFs;}
    inline const std::string& electronIDDecoration() const {return m_electronIDDecoration;}
    inline const std::string& electronIDLooseDecoration() const {return m_electronIDLooseDecoration;}
    inline bool useElectronChargeIDSelection() const {return m_useElectronChargeIDSelection;}
    inline bool useEgammaLeakageCorrection() const {return m_useEgammaLeakageCorrection;}
    inline bool enablePromptLeptonImprovedVetoStudies() const {return m_enablePromptLeptonImprovedVetoStudies;}

    // Fwd electron
    inline virtual void fwdElectronID(const std::string& s) {
      if (!m_configFixed) {
        m_fwdElectronID = s;
      }
    }

    inline virtual void fwdElectronIDLoose(const std::string& s) {
      if (!m_configFixed) {
        m_fwdElectronIDLoose = s;
      }
    }

    inline virtual void fwdElectronPtcut(const float pt) {
      if (!m_configFixed) {
        m_fwdElectronPtcut = pt;
      }
    }

    inline virtual void fwdElectronMinEtacut(const float eta) {
      if (!m_configFixed) {
        m_fwdElectronMinEtacut = eta;
      }
    }

    inline virtual void fwdElectronMaxEtacut(const float eta) {
      if (!m_configFixed) {
        m_fwdElectronMaxEtacut = eta;
      }
    }

    inline virtual void fwdElectronBCIDCleaningMinRun(const int fwdElectronBCIDCleaningMinRun) {
      if (!m_configFixed) {
        m_fwdElectronBCIDCleaningMinRun = fwdElectronBCIDCleaningMinRun;
      }
    }

    inline virtual void fwdElectronBCIDCleaningMaxRun(const int fwdElectronBCIDCleaningMaxRun) {
      if (!m_configFixed) {
        m_fwdElectronBCIDCleaningMaxRun = fwdElectronBCIDCleaningMaxRun;
      }
    }

    inline virtual const std::string& fwdElectronID()     const {return m_fwdElectronID;}
    inline virtual const std::string& fwdElectronIDLoose()     const {return m_fwdElectronIDLoose;}
    inline virtual float fwdElectronPtcut()       const {return m_fwdElectronPtcut;}
    inline virtual float fwdElectronMinEtacut()       const {return m_fwdElectronMinEtacut;}
    inline virtual float fwdElectronMaxEtacut()       const {return m_fwdElectronMaxEtacut;}
    inline virtual int fwdElectronBCIDCleaningMinRun() const {return m_fwdElectronBCIDCleaningMinRun;}
    inline virtual int fwdElectronBCIDCleaningMaxRun() const {return m_fwdElectronBCIDCleaningMaxRun;}

    // Photon configuration
    inline virtual void photonPtcut(const float pt) {
      if (!m_configFixed) {
        m_photon_configuration.pt = pt;
      }
    }

    inline virtual void photonEtacut(const float eta) {
      if (!m_configFixed) {
        m_photon_configuration.eta = eta;
      }
    }

    inline virtual void photonIdentification(const std::string& quality) {
      if (!m_configFixed) {
        m_photon_configuration.identification = quality;
      }
    }

    inline virtual void photonIdentificationLoose(const std::string& quality) {
      if (!m_configFixed) {
        m_photon_configuration_loose.identification = quality;
      }
    }

    inline virtual void photonIsolation(const std::string& quality) {
      if (!m_configFixed) {
        m_photon_configuration.isolation = quality;
      }
    }

    inline virtual void photonIsolationLoose(const std::string& quality) {
      if (!m_configFixed) {
        m_photon_configuration_loose.isolation = quality;
      }
    }

    // Muon configuration
    inline virtual void muonPtcut(const float pt) {
      if (!m_configFixed) {
        m_muonPtcut = pt;
      }
    }

    inline virtual void muonEtacut(const float eta) {
      if (!m_configFixed) {
        m_muonEtacut = eta;
      }
    }

    inline virtual void muonQuality(const std::string& quality) {
      if (!m_configFixed) {
        m_muonQuality = quality;
      }
    }

    void muonUseMVALowPt(const bool& UseMVALowPt) {
      if (!m_configFixed) {
	m_muonUseMVALowPt = UseMVALowPt;
      }
    }

    void muonUse2stationMuonsHighPt(const bool& Use2stationMuonsHighPt) {
      if (!m_configFixed) {
	m_muonUse2stationMuonsHighPt = Use2stationMuonsHighPt;
      }
    }

    void muonUseMVALowPtLoose(const bool& UseMVALowPtLoose) {
      if (!m_configFixed) {
	m_muonUseMVALowPtLoose = UseMVALowPtLoose;
      }
    }

    void muonUse2stationMuonsHighPtLoose(const bool& Use2stationMuonsHighPtLoose) {
      if (!m_configFixed) {
	m_muonUse2stationMuonsHighPtLoose = Use2stationMuonsHighPtLoose;
      }
    }

    inline virtual void muonQualityLoose(const std::string& quality) {
      if (!m_configFixed) {
        m_muonQualityLoose = quality;
      }
    }

    inline virtual void muonIsolation(const std::string& iso) {
      if (!m_configFixed) {
        m_muonIsolation = iso;
      }
    }

    inline virtual void muonIsolationLoose(const std::string& iso) {
      if (!m_configFixed) {
        m_muonIsolationLoose = iso;
      }
    }

    void muonIsolationSF(std::string const& iso) {
      if (!m_configFixed) {
        m_muonIsolationSF = iso;
      }
    }

    void muonIsolationSFLoose(std::string const& iso) {
      if (!m_configFixed) {
        m_muonIsolationSFLoose = iso;
      }
    }

    void muonMuonDoSmearing2stationHighPt(const bool& MuonDoSmearing2stationHighPt) {
      if (!m_configFixed) {
	m_muonMuonDoSmearing2stationHighPt = MuonDoSmearing2stationHighPt;
      }
    }

    void muonMuonDoExtraSmearingHighPt(const bool& MuonDoExtraSmearingHighPt) {
      if (!m_configFixed) {
	m_muonMuonDoExtraSmearingHighPt = MuonDoExtraSmearingHighPt;
      }
    }

    inline virtual float muonPtcut() const {return m_muonPtcut;}
    inline virtual float muonEtacut() const {return m_muonEtacut;}
    inline virtual const std::string& muonQuality() const {return m_muonQuality;}
    inline virtual const std::string& muonQualityLoose() const {return m_muonQualityLoose;}
    inline virtual bool muonUseMVALowPt() const {return m_muonUseMVALowPt;}
    inline virtual bool muonUse2stationMuonsHighPt() const {return m_muonUse2stationMuonsHighPt;}
    inline virtual bool muonUseMVALowPtLoose() const {return m_muonUseMVALowPtLoose;}
    inline virtual bool muonUse2stationMuonsHighPtLoose() const {return m_muonUse2stationMuonsHighPtLoose;}
    inline virtual const std::string& muonIsolation() const {return m_muonIsolation;}
    inline virtual const std::string& muonIsolationLoose() const {return m_muonIsolationLoose;}
    std::string const& muonIsolationSF() const {return m_muonIsolationSF;}
    std::string const& muonIsolationSFLoose() const {return m_muonIsolationSFLoose;}
    inline virtual bool muonMuonDoSmearing2stationHighPt() const {return m_muonMuonDoSmearing2stationHighPt;}
    inline virtual bool muonMuonDoExtraSmearingHighPt() const {return m_muonMuonDoExtraSmearingHighPt;}

    // Soft Muon configuration
    inline virtual void softmuonPtcut(const float pt) {
      if (!m_configFixed) {
        m_softmuonPtcut = pt;
      }
    }

    inline virtual void softmuonEtacut(const float eta) {
      if (!m_configFixed) {
        m_softmuonEtacut = eta;
      }
    }

    inline virtual void softmuonQuality(const std::string& quality) {
      if (!m_configFixed) {
        m_softmuonQuality = quality;
      }
    }

    void softmuonUseMVALowPt(const bool UseMVALowPtSoftMuon) {
      if (!m_configFixed) {
        m_softmuonUseMVALowPt = UseMVALowPtSoftMuon;
      }
    }

    inline virtual void softmuonDRJetcut(const float DRJet) {
      if (!m_configFixed) {
        m_softmuonDRJetcut = DRJet;
      }
    }
    
    inline virtual void softmuonDRJetcutUseRapidity(const bool in) {
      if (!m_configFixed) {
        m_softmuonDRJetcutUseRapidity = in;
      }
    }
    
    inline virtual void softmuonAdditionalTruthInfo(bool in) {
      if (!m_configFixed) {
        m_softmuonAdditionalTruthInfo=in;
      }
    }
    
    inline virtual void softmuonAdditionalTruthInfoCheckPartonOrigin(bool in) {
      if (!m_configFixed) {
        m_softmuonAdditionalTruthInfoCheckPartonOrigin=in;
      }
    }
    
    inline virtual void softmuonAdditionalTruthInfoDoVerbose(bool in) {
      if (!m_configFixed) {
        m_softmuonAdditionalTruthInfoDoVerbose=in;
      }
    }

    inline virtual float softmuonPtcut() const {return m_softmuonPtcut;}
    inline virtual float softmuonEtacut() const {return m_softmuonEtacut;}
    inline virtual const std::string& softmuonQuality() const {return m_softmuonQuality;}
    inline virtual bool softmuonUseMVALowPt() const {return m_softmuonUseMVALowPt;}
    inline virtual float softmuonDRJetcut() const {return m_softmuonDRJetcut;}
    inline virtual bool softmuonDRJetcutUseRapidity() const {return m_softmuonDRJetcutUseRapidity;}
    inline virtual bool softmuonAdditionalTruthInfo() const { return m_softmuonAdditionalTruthInfo;}
    inline virtual bool softmuonAdditionalTruthInfoCheckPartonOrigin() const { return m_softmuonAdditionalTruthInfoCheckPartonOrigin;}
    inline virtual bool softmuonAdditionalTruthInfoDoVerbose() const { return m_softmuonAdditionalTruthInfoDoVerbose;}

    // Jet configuration
    inline virtual void jetPtcut(const float pt) {
      if (!m_configFixed) {
        m_jetPtcut = pt;
      }
    }

    inline virtual void jetEtacut(const float eta) {
      if (!m_configFixed) {
        m_jetEtacut = eta;
      }
    }

    inline virtual void jetPtGhostTracks(const float pt, const float small_jet_pt) {
      if (!m_configFixed) {
        if ( small_jet_pt >= pt+4999){  
            m_jetPtGhostTracks = pt;
        }
        else {
            m_jetPtGhostTracks = (small_jet_pt - 5000) > 20000 ? (small_jet_pt - 5000) : 20000;
        }
      }
    }
    
    inline virtual void jetEtaGhostTracks(const float eta) {
      if (!m_configFixed) {
        m_jetEtaGhostTracks = eta;
      }
    }
    
    inline virtual void ghostTrackspT(const float pt) {
      if (!m_configFixed) {
        m_ghostTrackspT = pt;
      }
    }
    
    inline virtual void ghostTracksVertexAssociation(const std::string& vertexassociation) {
      if (!m_configFixed) {
        m_ghostTracksVertexAssociation = vertexassociation;
      }
    }
    
    inline virtual void ghostTracksQuality(const std::string& ghostTracksQuality) {
     if (!m_configFixed) {
        m_ghostTracksQuality = ghostTracksQuality;
     }
    }

    inline virtual float jetPtcut()  const {return m_jetPtcut;}
    inline virtual float jetEtacut() const {return m_jetEtacut;}
    
    inline virtual float ghostTrackspT()  const {return m_ghostTrackspT;}
    inline virtual const std::string& ghostTracksVertexAssociation()  const {return m_ghostTracksVertexAssociation;}
    inline virtual const std::string& ghostTracksQuality()  const {return m_ghostTracksQuality;}
    
    inline virtual float jetPtGhostTracks()  const {return m_jetPtGhostTracks;}
    inline virtual float jetEtaGhostTracks()  const {return m_jetEtaGhostTracks;}


    inline virtual void largeRJetPtcut(const float pt) {
      if (!m_configFixed) {
        m_largeRJetPtcut = pt;
      }
    }

    inline virtual void largeRJetEtacut(const float eta) {
      if (!m_configFixed) {
        m_largeRJetEtacut = eta;
      }
    }

    inline virtual void largeRJetUncertainties_NPModel(const std::string& largeR_config) {
      if (!m_configFixed) {
        m_largeRJetUncertainties_NPModel = largeR_config;
      }
    }
    
    inline virtual void largeRJetUncertaintiesConfigDir(const std::string& largeRConfigDir) {
      if (!m_configFixed) {
        m_largeRJetUncertaintiesConfigDir = largeRConfigDir;
      }
    }

    inline virtual void largeRJESJMSConfig(const std::string& largeR_config) {
      if (!m_configFixed) {
        m_largeRJESJMSConfig = largeR_config;
      }
    }

    inline virtual float largeRJetPtcut()  const {return m_largeRJetPtcut;}
    inline virtual float largeRJetEtacut() const {return m_largeRJetEtacut;}
    inline virtual const std::map<std::string,std::string> largeRJetSubstructureVariables() const {return m_largeRJetSubstructureVariables;}
    inline virtual const std::string& largeRJetUncertainties_NPModel() const {return m_largeRJetUncertainties_NPModel;}
    inline virtual const std::string& largeRJetUncertaintiesConfigDir() const {return m_largeRJetUncertaintiesConfigDir;}
    inline virtual const std::string& largeRJESJMSConfig() const {return m_largeRJESJMSConfig;}

    inline virtual void trackJetPtcut(const float pt) {
      if (!m_configFixed) {
        m_trackJetPtcut = pt;
      }
    }

    inline virtual void trackJetEtacut(const float eta) {
      if (!m_configFixed) {
        m_trackJetEtacut = eta;
      }
    }

    inline virtual float trackJetPtcut()  const {return m_trackJetPtcut;}
    inline virtual float trackJetEtacut() const {return m_trackJetEtacut;}


    inline virtual void trackPtcut(const float pt) {
      if (!m_configFixed) {
        m_trackPtcut = pt;
      }
    }

    inline virtual void trackEtacut(const float eta) {
      if (!m_configFixed) {
        m_trackEtacut = eta;
      }
    }

    inline virtual void trackQuality(const std::string& quality) {
      if (!m_configFixed) {
        m_trackQuality = quality;
      }
    }


    inline virtual const std::string& trackQuality() const {return m_trackQuality;}
    inline virtual float trackPtcut()  const {return m_trackPtcut;}
    inline virtual float trackEtacut() const {return m_trackEtacut;}




    inline virtual float RCJetPtcut() const {return m_RCJetPtcut;}
    inline virtual float RCJetEtacut() const {return m_RCJetEtacut;}
    inline virtual float RCInputJetPtMin() const {return m_RCInputJetPtMin;}
    inline virtual float RCInputJetEtaMax() const {return m_RCInputJetEtaMax;}
    inline virtual float RCJetTrimcut() const {return m_RCJetTrimcut;}
    inline virtual float RCJetRadius() const {return m_RCJetRadius;}
    inline virtual bool useRCJetSubstructure() const {return m_useRCJetSubstructure;}
    inline virtual bool useRCJetAdditionalSubstructure() const {return m_useRCJetAdditionalSubstructure;}

    inline virtual void RCJetPtcut(const float pt) {
      if (!m_configFixed) {
        m_RCJetPtcut = pt;
      }
    }

    inline virtual void RCJetEtacut(const float eta) {
      if (!m_configFixed) {
        m_RCJetEtacut = eta;
      }
    }

    inline virtual void RCInputJetPtMin(const float pt) {
      if (!m_configFixed) {
        m_RCInputJetPtMin = pt;
      }
    }

    inline virtual void RCInputJetEtaMax(const float eta) {
      if (!m_configFixed) {
        m_RCInputJetEtaMax = eta;
      }
    }

    inline virtual void RCJetTrimcut(const float trim) {
      if (!m_configFixed) {
        m_RCJetTrimcut = trim;
      }
    }

    inline virtual void RCJetRadius(const float radius) {
      if (!m_configFixed) {
        m_RCJetRadius = radius;
      }
    }

    inline virtual void useRCJetSubstructure(const bool use) {
      if (!m_configFixed) {
        m_useRCJetSubstructure = use;
      }
    }

    inline virtual void useRCJetAdditionalSubstructure(const bool use) {
      if (!m_configFixed) {
        m_useRCJetAdditionalSubstructure = use;
      }
    }

    inline virtual float VarRCJetPtcut() const {return m_VarRCJetPtcut;}
    inline virtual float VarRCJetEtacut() const {return m_VarRCJetEtacut;}
    inline virtual float VarRCJetTrimcut() const {return m_VarRCJetTrimcut;}
    inline virtual float VarRCJetMaxRadius() const {return m_VarRCJetMaxRadius;}
    inline virtual const std::string& VarRCJetRho() const {return m_VarRCJetRho;}
    inline virtual const std::string& VarRCJetMassScale() const {return m_VarRCJetMassScale;}
    inline virtual bool useVarRCJetSubstructure() const {return m_useVarRCJetSubstructure;}
    inline virtual bool useVarRCJetAdditionalSubstructure() const {return m_useVarRCJetAdditionalSubstructure;}

    inline virtual void VarRCJetPtcut(const float pt) {
      if (!m_configFixed) {
        m_VarRCJetPtcut = pt;
      }
    }

    inline virtual void VarRCJetEtacut(const float eta) {
      if (!m_configFixed) {
        m_VarRCJetEtacut = eta;
      }
    }

    inline virtual void VarRCJetTrimcut(const float trim) {
      if (!m_configFixed) {
        m_VarRCJetTrimcut = trim;
      }
    }

    inline virtual void VarRCJetMaxRadius(const float radius) {
      if (!m_configFixed) {
        m_VarRCJetMaxRadius = radius;
      }
    }

    inline virtual void VarRCJetRho(const std::string& rho) {
      if (!m_configFixed) {
        m_VarRCJetRho = rho;
      }
    }

    inline virtual void VarRCJetMassScale(const std::string& mass_scale) {
      if (!m_configFixed) {
        m_VarRCJetMassScale = mass_scale;
      }
    }

    inline virtual void useVarRCJetSubstructure(const bool use) {
      if (!m_configFixed) {
        m_useVarRCJetSubstructure = use;
      }
    }

    inline virtual void useVarRCJetAdditionalSubstructure(const bool use) {
      if (!m_configFixed) {
        m_useVarRCJetAdditionalSubstructure = use;
      }
    }


    virtual void jetUncertainties_NPModel(const std::string& s);
    virtual void jetUncertainties_QGFracFile(const std::string& s);
    virtual void jetUncertainties_QGHistPatterns(const std::string& s);
    inline bool doMultipleJES() const {return m_doMultipleJES;}
    inline virtual const std::string& jetUncertainties_NPModel() const {return m_jetUncertainties_NPModel;}
    inline virtual const std::string& jetUncertainties_QGFracFile() const {return m_jetUncertainties_QGFracFile;}
    inline virtual const std::vector<std::string>& jetUncertainties_QGHistPatterns() const {
                                                                                            return m_jetUncertainties_QGHistPatterns;
                                                                                                                                     }

    inline virtual void jetJERSmearingModel(const std::string& s) {
      if (!m_configFixed) {
        m_jetJERSmearingModel = s;
      }
    }

    inline virtual const std::string& jetJERSmearingModel() const {return m_jetJERSmearingModel;}

    inline virtual void jetCalibSequence(const std::string& s) {
      if (!m_configFixed) {
        m_jetCalibSequence = s;
      }
    }

    inline virtual const std::string& jetCalibSequence() const {return m_jetCalibSequence;}

    inline virtual void jetStoreTruthLabels(bool b) {
      if (!m_configFixed) {
        m_jetStoreTruthLabels = b;
      }
    }

    inline virtual bool jetStoreTruthLabels() const {return m_jetStoreTruthLabels;}

    inline virtual void doJVTinMET(const bool& doJVT) {
      if (!m_configFixed) {
        m_doJVTInMETCalculation = doJVT;
      }
    }

    inline virtual bool doJVTinMET() const {return m_doJVTInMETCalculation;}
    inline virtual void saveFailJVTJets(const bool& doJVT) {
      if (!m_configFixed) {
        m_saveFailJVTJets = doJVT;
      }
    }

    inline virtual bool saveFailJVTJets() const {return m_saveFailJVTJets;}

    inline const std::string& getJVTWP() const {return m_JVTWP;}
    inline void setJVTWP(const std::string& value) {m_JVTWP = value;}

    inline virtual void doForwardJVTinMET(const bool& dofJVT) {
      if (!m_configFixed) {
        m_doForwardJVTInMETCalculation = dofJVT;
      }
    }

    inline virtual bool doForwardJVTinMET() const {return m_doForwardJVTInMETCalculation;}
    inline virtual void saveFailForwardJVTJets(const bool& dofJVT) {
      if (!m_configFixed) {
        m_saveFailForwardJVTJets = dofJVT;
      }
    }

    inline virtual bool saveFailForwardJVTJets() const {return m_saveFailForwardJVTJets;}

    inline const std::string& getfJVTWP() const {return m_fJVTWP;}
    inline void setfJVTWP(const std::string& value) {m_fJVTWP = value;}

    inline virtual float JSF() const {return m_JSF;}
    inline virtual float bJSF() const {return m_bJSF;}

    // MET Configuration
    inline virtual void METUncertaintiesConfigDir(const std::string& METConfigDir) {
      if (!m_configFixed) {
        m_METUncertaintiesConfigDir = METConfigDir;
      }
    }
    inline virtual const std::string& METUncertaintiesConfigDir() const {return m_METUncertaintiesConfigDir;}

    // Tau configuration setters
    inline virtual void tauPtcut(const float pt) {
      if (!m_configFixed) {
        m_tau_configuration.pt = pt;
        m_tau_configuration_loose.pt = pt;
      }
    }

    inline virtual void tauEtaRegions(const std::string& etaRegions) {
      if (!m_configFixed) {
        m_tau_configuration.etaRegions = etaRegions;
        m_tau_configuration_loose.etaRegions = etaRegions;
      }
    }

    inline virtual void tauJetIDWP(const std::string& s) {
      if (!m_configFixed) m_tau_configuration.jetIDWP = s;
    }

    inline virtual void tauJetIDWPLoose(const std::string& s) {
      if (!m_configFixed) m_tau_configuration_loose.jetIDWP = s;
    }

    inline virtual void tauEleBDTWP(const std::string& s) {
      if (!m_configFixed) m_tau_configuration.eleBDTWP = s;
    }

    inline virtual void tauEleBDTWPLoose(const std::string& s) {
      if (!m_configFixed) m_tau_configuration_loose.eleBDTWP = s;
    }

    inline virtual void tauEleOLR(bool do_tau_ele_olr) {
      if (!m_configFixed) m_tau_configuration.eleOLR = do_tau_ele_olr;
    }

    inline virtual void tauEleOLRLoose(bool do_tau_ele_olr) {
      if (!m_configFixed) m_tau_configuration_loose.eleOLR = do_tau_ele_olr;
    }

    inline virtual void tauSFDoRNNID(bool do_tau_rnn_id) {
      if (!m_configFixed) m_tau_configuration.doRNNID = do_tau_rnn_id;
    }

    inline virtual void tauSFDoBDTID(bool do_tau_bdt_id) {
      if (!m_configFixed) m_tau_configuration.doBDTID = do_tau_bdt_id;
    }

    inline virtual void tauJetConfigFile(const std::string& s) {
      if (!m_configFixed) m_tau_configuration.fileName = s;
    }

    inline virtual void tauJetConfigFileLoose(const std::string& s) {
      if (!m_configFixed) m_tau_configuration_loose.fileName = s;
    }

    inline virtual void tauSubstructureSF(bool do_tau_substructure_sf) {
      if (!m_configFixed) m_tau_configuration.substructureSF = do_tau_substructure_sf;
    }

    inline virtual void tauSubstructureSFLoose(bool do_tau_substructure_sf) {
      if (!m_configFixed) m_tau_configuration_loose.substructureSF = do_tau_substructure_sf;
    }

    // Tau configuration getters
    inline virtual float tauPtcut() const {
      return m_tau_configuration.pt;
    }

    inline const std::string& tauEtaRegions() const {
       return m_tau_configuration.etaRegions;
    }

    inline const std::string& tauJetIDWP() const {
      return m_tau_configuration.jetIDWP;
    }

    inline const std::string& tauJetIDWPLoose() const {
      return m_tau_configuration_loose.jetIDWP;
    }

    inline const std::string& tauEleBDTWP() const {
      return m_tau_configuration.eleBDTWP;
    }

    inline const std::string& tauEleBDTWPLoose() const {
      return m_tau_configuration_loose.eleBDTWP;
    }

    inline virtual bool tauEleOLR() {
      return m_tau_configuration.eleOLR;
    }

    inline virtual bool tauEleOLRLoose() {
      return m_tau_configuration_loose.eleOLR;
    }

    inline const std::string& tauJetConfigFile() {
      return m_tau_configuration.fileName;
    }

    inline const std::string& tauJetConfigFileLoose() {
      return m_tau_configuration_loose.fileName;
    }

    inline virtual bool tauSubstructureSF() {
      return m_tau_configuration.substructureSF;
    }

    inline virtual bool tauSubstructureSFLoose() {
      return m_tau_configuration_loose.substructureSF;
    }

    // Applying new tau energy calibration
    inline bool applyTauMVATES() {
      return true;
    }

    inline bool tauSFDoRNNID() const {
      return m_tau_configuration.doRNNID;
    }

    inline bool tauSFDoBDTID() const {
      return m_tau_configuration.doBDTID;
    }

    // photon getters
    inline virtual float photonPtcut() const {
      return m_photon_configuration.pt;
    }

    inline virtual float photonEtacut() const {
      return m_photon_configuration.eta;
    }

    inline const std::string& photonIdentification() {
      return m_photon_configuration.identification;
    }

    inline const std::string& photonIdentificationLoose() {
      return m_photon_configuration_loose.identification;
    }

    inline const std::string& photonIsolation() {
      return m_photon_configuration.isolation;
    }

    inline const std::string& photonIsolationLoose() {
      return m_photon_configuration_loose.isolation;
    }

    // inline const std::string& tauJetID() const {return m_tauJetID;}
    // inline const std::string& tauJetIDBkg() const {return m_tauJetIDBkg;}
    // inline const std::string& tauEVetoID() const {return m_tauEVetoID;}
    // inline virtual bool tauVetoLArCrack() const {return m_tauVetoLArCrack;}
    // inline virtual float tauPtcut() const {return m_tauPtcut;}

    // [[[-----------------------------------------------
    // Particle Level (Truth) configuration

    // electrons
    inline virtual void truth_electron_PtCut(const float pt) {
      if (!m_configFixed) {
        m_truth_electron.PtCut = pt;
      }
    }

    inline virtual void truth_electron_EtaCut(const float eta) {
      if (!m_configFixed) {
        m_truth_electron.EtaCut = eta;
      }
    }

    inline virtual float truth_electron_PtCut() const {return m_truth_electron.PtCut;}
    inline virtual float truth_electron_EtaCut() const {return m_truth_electron.EtaCut;}
    inline virtual bool truth_electron_NotFromHadron() const {return m_truth_electron.NotFromHadron;}
    inline virtual bool truth_electron_TauIsHadron() const {return m_truth_electron.TauIsHadron;}

    // muons
    inline virtual void truth_muon_PtCut(const float pt) {
      if (!m_configFixed) {
        m_truth_muon.PtCut = pt;
      }
    }

    inline virtual void truth_muon_EtaCut(const float eta) {
      if (!m_configFixed) {
        m_truth_muon.EtaCut = eta;
      }
    }

    inline virtual float truth_muon_PtCut() const {return m_truth_muon.PtCut;}
    inline virtual float truth_muon_EtaCut() const {return m_truth_muon.EtaCut;}
    inline virtual bool truth_muon_NotFromHadron() const {return m_truth_muon.NotFromHadron;}
    inline virtual bool truth_muon_TauIsHadron() const {return m_truth_muon.TauIsHadron;}
    
    // soft muons
    inline virtual void truth_softmuon_PtCut(const float pt) {
      if (!m_configFixed) {
        m_truth_softmuon.PtCut = pt;
      }
    }

    inline virtual void truth_softmuon_EtaCut(const float eta) {
      if (!m_configFixed) {
        m_truth_softmuon.EtaCut = eta;
      }
    }

    inline virtual float truth_softmuon_PtCut() const {return m_truth_softmuon.PtCut;}
    inline virtual float truth_softmuon_EtaCut() const {return m_truth_softmuon.EtaCut;}

    // photons
    inline virtual void truth_photon_PtCut(const float pt) {
      if (!m_configFixed) {
        m_truth_photon.PtCut = pt;
      }
    }

    inline virtual void truth_photon_EtaCut(const float eta) {
      if (!m_configFixed) {
        m_truth_photon.EtaCut = eta;
      }
    }

    inline virtual void truth_photon_Origin(const std::string& value) {
      if (!m_configFixed) {
        m_truth_photon.Origin = value;
      }
    }

    inline virtual void truth_photon_Isolation(const std::string& value) {
      if (!m_configFixed) {
        m_truth_photon.Isolation = value;
      }
    }

    inline virtual float truth_photon_PtCut() const {return m_truth_photon.PtCut;}
    inline virtual float truth_photon_EtaCut() const {return m_truth_photon.EtaCut;}
    inline virtual const std::string& truth_photon_Origin() const {return m_truth_photon.Origin;}
    inline virtual const std::string& truth_photon_Isolation() const {return m_truth_photon.Isolation;}

    // jets
    inline virtual void truth_jet_PtCut(const float pt) {
      if (!m_configFixed) {
        m_truth_jet.PtCut = pt;
      }
    }

    inline virtual void truth_jet_EtaCut(const float eta) {
      if (!m_configFixed) {
        m_truth_jet.EtaCut = eta;
      }
    }

    inline virtual float truth_jet_PtCut() const {return m_truth_jet.PtCut;}
    inline virtual float truth_jet_EtaCut() const {return m_truth_jet.EtaCut;}

    // large R jets
    inline virtual void truth_jet_largeR_PtCut(const float pt) {
      if (!m_configFixed) {
        m_truth_largeRJet.PtCut = pt;
      }
    }

    inline virtual void truth_jet_largeR_EtaCut(const float eta) {
      if (!m_configFixed) {
        m_truth_largeRJet.EtaCut = eta;
      }
    }

    inline virtual float truth_jet_largeR_PtCut() const {return m_truth_largeRJet.PtCut;}
    inline virtual float truth_jet_largeR_EtaCut() const {return m_truth_largeRJet.EtaCut;}

    inline virtual void truth_tau_PtCut(const float pt) {
      if (!m_configFixed) {
        m_truth_tau.PtCut = pt;
      }
    }

    inline virtual void truth_tau_EtaCut(const float eta) {
      if (!m_configFixed) {
        m_truth_tau.EtaCut = eta;
      }
    }

    inline virtual float truth_tau_PtCut() const {return m_truth_tau.PtCut;}
    inline virtual float truth_tau_EtaCut() const {return m_truth_tau.EtaCut;}

    // -----------------------------------------------]]]

    /// HL LHC studies
    inline virtual void HLLHC(const bool s) {
      if (!m_configFixed) {
        m_HLLHC = s;
      }
    }

    inline virtual bool HLLHC() const {return m_HLLHC;}
    inline virtual void HLLHCFakes(const bool s) {
      if (!m_configFixed) {
        m_HLLHCFakes = s;
      }
    }

    inline virtual bool HLLHCFakes() const {return m_HLLHCFakes;}

    void setBTaggingSFSysts(std::string WP, const std::set<std::string>& btagging_SFs, bool isTrackJet = false);

    inline virtual std::set<std::string>  btagging_namedSysts(std::string WP) const {return bTag_named_systs.at(WP);}
    inline virtual unsigned int btagging_num_B_eigenvars(std::string WP) const {return bTag_eigen_B.at(WP);}
    inline virtual unsigned int btagging_num_C_eigenvars(std::string WP) const {return bTag_eigen_C.at(WP);}
    inline virtual unsigned int btagging_num_Light_eigenvars(std::string WP) const {return bTag_eigen_light.at(WP);}

    inline virtual std::set<std::string>  trkjet_btagging_namedSysts(std::string WP) const {return bTag_named_systs_trkJet.at(WP);}
    inline virtual unsigned int trkjet_btagging_num_B_eigenvars(std::string WP) const {return bTag_eigen_B_trkJet.at(WP);}
    inline virtual unsigned int trkjet_btagging_num_C_eigenvars(std::string WP) const {return bTag_eigen_C_trkJet.at(WP);}
    inline virtual unsigned int trkjet_btagging_num_Light_eigenvars(std::string WP) const {return bTag_eigen_light_trkJet.at(WP);}


    const std::vector<std::pair<std::string, std::string> > boostedJetTaggers() const {return m_chosen_boostedJetTaggers;}
    const std::unordered_map<std::string, std::string> boostedTaggerSFnames() const {return m_boostedTaggerSFnames;}
    void setCalibBoostedJetTagger(const std::string& WP, const std::string& SFname);
    // B-tagging WPs requested by user (updated to pair of strings to hold algorithm and WP)
    const std::vector<std::pair<std::string, std::string> > bTagWP() const {return m_chosen_btaggingWP_caloJet;}
    const std::vector<std::pair<std::string, std::string> > bTagWP_trkJet() const {return m_chosen_btaggingWP_trkJet;}
    // parse b-tagging configuration from config file into a vector of pair <algorithm, WP>
    void parse_bTagWPs(const std::string& btagWPsettingString,
        std::vector<std::pair<std::string, std::string>>& btagWPlist,
        const std::string& jetCollectionName);
    // B-tagging systematics requested by user to be excluded from EV treatment, separated by semi-colons
    const std::string bTagSystsExcludedFromEV() const {return m_bTagSystsExcludedFromEV;}

    // B-tagging WPs actually available, according to CDI file
    // will be set in TopCPTools
    void setBTagWP_available(std::string btagging_WP);
    void setBTagWP_available_trkJet(std::string btagging_WP);
    const std::vector<std::string>& bTagWP_available() const {return m_available_btaggingWP;}
    const std::vector<std::string>& bTagWP_available_trkJet() const {return m_available_btaggingWP_trkJet;}
    // B-tagging WPs actually calibrated, according to CDI file
    // will be set in TopCPTools
    void setBTagWP_calibrated(std::string btagging_WP);
    void setBTagWP_calibrated_trkJet(std::string btagging_WP);
    const std::vector<std::string>& bTagWP_calibrated() const {return m_calibrated_btaggingWP;}
    const std::vector<std::string>& bTagWP_calibrated_trkJet() const {return m_calibrated_btaggingWP_trkJet;}
    // B-tagging algorithms, e.g. DL1, DL1r, DL1rmu
    // which of them can be initialized for the given CDI file
    // used for e.g. storing algorithm discriminant in the event saver
    void setBTagAlgo_available(std::string algo, std::string toolName);
    void setBTagAlgo_available_trkJet(std::string algo, std::string toolName);
    const std::set<std::string>& bTagAlgo_available() const {return m_available_btaggingAlgos;}
    const std::set<std::string>& bTagAlgo_available_trkJet() const {return m_available_btaggingAlgos_trkJet;}
    // since MV2c10 is the only non-DL1 b-tagger, we just expose a bool to check if MV2c10 is used or not
    bool bTagAlgo_MV2c10_used() const {return m_MV2c10_algo_used;}
    bool bTagAlgo_MV2c10_used_trkJet() const {return m_MV2c10_algo_used_trkJet;}

    const std::unordered_map<std::string, std::string>& bTagAlgo_selToolNames() const {return m_algo_selTools;}
    const std::unordered_map<std::string, std::string>& bTagAlgo_selToolNames_trkJet() const {return m_algo_selTools_trkJet;}

    std::string FormatedWP(std::string raw_WP);

    bool printCDIpathWarning() const
    {return m_cdi_path_warning;}
    void setPrintCDIpathWarning(bool flag)
    {m_cdi_path_warning = flag;}
    const std::string bTaggingCDIPath() const
    {return m_btagging_cdi_path;}
    const std::string& bTaggingCalibration_B() const
    {return m_btagging_calibration_B;};
    const std::string& bTaggingCalibration_C() const
    {return m_btagging_calibration_C;};
    const std::string& bTaggingCalibration_Light() const
    {return m_btagging_calibration_Light;};

    // LHAPDF settings

    inline virtual std::vector<std::string> LHAPDFSets() {return m_lhapdf_options.pdf_set_names;}
    inline virtual bool doLHAPDF() {return m_lhapdf_options.doLHAPDF;}
    inline virtual std::string baseLHAPDF() {return m_lhapdf_options.baseLHAPDF;}
    inline virtual bool doLHAPDFInNominalTrees() {return m_lhapdf_options.doLHAPDFInNominalTrees;}
    inline virtual bool saveLHAPDFEvent() {return m_lhapdf_options.save_per_event;}
    void addLHAPDFResult(const std::string& pdf_name,
                         std::vector<float> sum_of_weights);

    std::unordered_map<std::string, std::vector<float> >& getLHAPDFResults() {return m_lhapdf_options.pdf_results;};


    // Pileup Reweighting Settings
    bool doPileupReweighting() {return m_pileup_reweighting.apply;};
    bool PileupUseGRLTool() {return m_pileup_reweighting.use_grl_tool;};

    const std::vector<std::string>& PileupConfig() {return m_pileup_reweighting.config_files;};
    const std::vector<std::string>& PileupLumiCalc() {return m_pileup_reweighting.lumi_calc_files;};

    bool PileupMuDependent() {return m_pileup_reweighting.mu_dependent;};

    // Update for R21
    const std::vector<std::string>& PileupConfig_FS() {return m_pileup_reweighting.config_files_FS;};
    const std::vector<std::string>& PileupConfig_AF() {return m_pileup_reweighting.config_files_AF;};
    const std::vector<std::string>& PileupActualMu_FS() {return m_pileup_reweighting.actual_mu_FS;};
    const std::vector<std::string>& PileupActualMu_AF() {return m_pileup_reweighting.actual_mu_AF;};
    inline virtual float PileupDataTolerance() const {return m_pileup_reweighting.unrepresented_data_tol;};
    const std::vector<int>& PileupPeriodAssignments() const {return m_pileup_reweighting.periodAssignments;};

    const std::vector<double>& PileUpCustomScaleFactors() {return m_pileup_reweighting.custom_SF;};

    inline const std::string& muonTriggerSF() const {return m_muon_trigger_SF;}

    inline bool demandPriVtx() const {return m_demandPriVtx;}

    // Where the sum of event weights
    // before derivation framework is kept
    inline const std::string& sumOfEventWeightsMetaData() const {return m_sumOfEventWeightsMetaData;}

    // Object Selector Name
    inline virtual void objectSelectionName(const std::string& s) {
      if (!m_configFixed) {
        m_objectSelectionName = s;
      }
    }

    inline virtual const std::string& objectSelectionName() const {return m_objectSelectionName;}
    // Output Format
    inline virtual void outputFormat(const std::string& s) {
      if (!m_configFixed) {
        m_outputFormat = s;
      }
    }

    inline virtual const std::string& outputFormat() const {return m_outputFormat;}
    // Output Events
    virtual void outputEvents(const std::string& s);
    inline virtual const std::string& outputEvents() const {return m_outputEvents;}
    inline bool saveOnlySelectedEvents() const {return m_saveOnlySelectedEvents;}

    // SetAutoFlush(0) on EventSaverFlatNtuple for ANALYSISTO-44 workaround
    inline bool outputFileSetAutoFlushZero() const {return m_outputFileSetAutoFlushZero;}
    // Better configurable settings for TTree memory optimisation (ANALYSISTO-44, ANALYSISTO-463)
    inline int outputFileNEventAutoFlush() const {return m_outputFileNEventAutoFlush;}
    inline int outputFileBasketSizePrimitive() const {return m_outputFileBasketSizePrimitive;}
    inline int outputFileBasketSizeVector() const {return m_outputFileBasketSizeVector;}

    // Number of events to run on (only for testing)
    inline virtual unsigned int numberOfEventsToRun() const {return m_numberOfEventsToRun;}
    inline virtual void numberOfEventsToRun(const unsigned int& n_events) {
      if (!m_configFixed) {
        m_numberOfEventsToRun = n_events;
      }
    }

    // Number of events to skip (only for testing)
    inline virtual unsigned int numberOfEventsToSkip() const {return m_numberOfEventsToSkip;}
    inline virtual void numberOfEventsToSkip(const unsigned int& n_events) {
      if (!m_configFixed) {
        m_numberOfEventsToSkip = n_events;
      }
    }

    // systematics
    inline virtual void systematics(const std::string& s) {
      if (!m_configFixed) {
        m_systematics = s;
      }
    }

    inline virtual const std::string& systematics() const {return m_systematics;}

    /// syst helper functions
    bool isSystNominal(const std::string& syst) const;
    bool isSystAll(const std::string& syst) const;
    bool getSystematicsList(const std::string& syst, std::set<std::string>& systList) const;
    bool contains(const std::set<std::string>& systList, const std::string& name) const;

    virtual void systematicsPhotons(const std::list<CP::SystematicSet>& syst);
    virtual void systematicsElectrons(const std::list<CP::SystematicSet>& syst);
    virtual void systematicsFwdElectrons(const std::list<CP::SystematicSet>& syst);
    virtual void systematicsMuons(const std::list<CP::SystematicSet>& syst);
    virtual void systematicsSoftMuons(const std::list<CP::SystematicSet>& syst);
    virtual void systematicsTaus(const std::list<CP::SystematicSet>& syst);
    virtual void systematicsJets(const std::list<CP::SystematicSet>& syst);
    virtual void systematicsLargeRJets(const std::list<CP::SystematicSet>& syst);
    virtual void systematicsTrackJets(const std::list<CP::SystematicSet>& syst);
    virtual void systematicsTracks(const std::list<CP::SystematicSet>& syst);
    virtual void systematicsMET(const std::list<CP::SystematicSet>& syst);

    virtual void systematicsJetGhostTrack(const std::list<CP::SystematicSet>& syst);

    inline virtual void jetSubstructureName(const std::string& s) {
      if (!m_configFixed) {
        m_jetSubstructureName = s;
      }
    }

    inline virtual const std::string& jetSubstructureName() const {return m_jetSubstructureName;}

    // Systematic hash values
    inline std::size_t nominalHashValue() const {return m_nominalHashValue;}
    inline std::shared_ptr<std::unordered_set<std::size_t> > systHashPhotons()   const {return m_systHashPhotons;}
    inline std::shared_ptr<std::unordered_set<std::size_t> > systHashElectrons() const {return m_systHashElectrons;}
    inline std::shared_ptr<std::unordered_set<std::size_t> > systHashFwdElectrons() const {return m_systHashFwdElectrons;}
    inline std::shared_ptr<std::unordered_set<std::size_t> > systHashMuons()     const {return m_systHashMuons;}
    inline std::shared_ptr<std::unordered_set<std::size_t> > systHashSoftMuons()     const {return m_systHashSoftMuons;}
    inline std::shared_ptr<std::unordered_set<std::size_t> > systHashTaus()      const {return m_systHashTaus;}
    inline std::shared_ptr<std::unordered_set<std::size_t> > systHashJets()      const {return m_systHashJets;}
    inline std::shared_ptr<std::unordered_set<std::size_t> > systHashAll()       const {return m_systHashAll;}
    inline std::shared_ptr<std::unordered_set<std::size_t> > systHashElectronInJetSubtraction() const {return m_systHash_electronInJetSubtraction;}

    // Systematic Maps
    inline std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > systMapPhotons()   const {return m_systMapPhotons;}
    inline std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > systMapElectrons() const {return m_systMapElectrons;}
    inline std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > systMapFwdElectrons() const {return m_systMapFwdElectrons;}
    inline std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > systMapMuons()     const {return m_systMapMuons;}
    inline std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > systMapSoftMuons()     const {return m_systMapSoftMuons;}
    inline std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > systMapTaus()      const {return m_systMapTaus;}
    inline std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > systMapJets()      const {return m_systMapJets;}
    inline std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > systMapLargeRJets()      const {return m_systMapLargeRJets;}

    // Systematic StoreGate key Maps
    inline std::shared_ptr<std::unordered_map<std::size_t, std::string> > systSgKeyMapPhotons()    const {return m_systSgKeyMapPhotons;}
    inline std::shared_ptr<std::unordered_map<std::size_t, std::string> > systSgKeyMapElectrons()  const {return m_systSgKeyMapElectrons;}
    inline std::shared_ptr<std::unordered_map<std::size_t, std::string> > systSgKeyMapFwdElectrons()  const {return m_systSgKeyMapFwdElectrons;}
    inline std::shared_ptr<std::unordered_map<std::size_t, std::string> > systSgKeyMapMuons()      const {return m_systSgKeyMapMuons;}
    inline std::shared_ptr<std::unordered_map<std::size_t, std::string> > systSgKeyMapSoftMuons()      const {return m_systSgKeyMapSoftMuons;}
    inline std::shared_ptr<std::unordered_map<std::size_t, std::string> > systSgKeyMapTaus()       const {return m_systSgKeyMapTaus;}
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > systSgKeyMapJets(const bool useLooseLeptonJets) const;
    inline std::shared_ptr<std::unordered_map<std::size_t, std::string> > systSgKeyMapLargeRJets() const {return m_systSgKeyMapLargeRJets;}
    inline std::shared_ptr<std::unordered_map<std::size_t, std::string> > systSgKeyMapTrackJets()  const {return m_systSgKeyMapTrackJets;}
    inline std::shared_ptr<std::unordered_map<std::size_t, std::string> > systSgKeyMapTracks()  const {return m_systSgKeyMapTracks;}

    // TTree names
    inline std::shared_ptr<std::unordered_map<std::size_t, std::string> > systAllTTreeNames() const {return m_systAllTTreeNames;}
    const std::string& systematicName(const std::size_t hash) const;
    // TTree index
    unsigned int ttreeIndex(const std::size_t hash) const;
    unsigned int ttreeIndexLoose(const std::size_t hash) const;

    AodMetaDataAccess& aodMetaData();
    AodMetaDataAccess const& aodMetaData() const {return *m_aodMetaData;}

    // Function to handle release series such that it can be cleaner to update in the future
    void setReleaseSeries();
    inline int getReleaseSeries() const {return m_release_series;}

    // Create bootstrapping weights
    inline bool saveBootstrapWeights() const {return m_saveBootstrapWeights;}
    inline void setSaveBootstrapWeights(const bool value) {m_saveBootstrapWeights = value;}
    inline int getNumberOfBootstrapReplicas() const {return m_BootstrapReplicas;}
    inline void setNumberOfBootstrapReplicas(const int value) {m_BootstrapReplicas = value;}

    // Switch to use BadBatmanCleaning
    inline bool useBadBatmanCleaning() const {return m_useBadBatmanCleaning;}
    inline void setUseBadBatmanCleaning(const bool value) {m_useBadBatmanCleaning = value;}
    inline unsigned int badBatmanCleaningMin() const {return m_badBatmanCleaningMin;}
    inline void setBadBatmanCleaningMin(const unsigned int value) {m_badBatmanCleaningMin = value;}
    inline unsigned int badBatmanCleaningMax() const {return m_badBatmanCleaningMax;}
    inline void setBadBatmanCleaningMax(const unsigned int value) {m_badBatmanCleaningMax = value;}

    // Switch to use event-level jet cleaning tool for studies
    inline bool useEventLevelJetCleaningTool() const {return m_useEventLevelJetCleaningTool;}
    inline void setUseEventLevelJetCleaningTool(const bool value) {m_useEventLevelJetCleaningTool = value;}

    // Just a function that might need to be used in multiple places - return the running year (2015, 2016, 2017)
    std::string getYear(unsigned int runnumber, const bool isMC);
    
    const std::string& getYear(){return m_year;}
    void SetYear(const std::string& year){m_year = year;}

    void SetTriggersToYear(const bool isMC);

    // Setter and getter functions for recording whether we have configured the nominal objects
    inline virtual void setNominalAvailable(const bool s) {m_isNominalAvailable = s;}
    inline bool isNominalAvailable() const {return m_isNominalAvailable;}

    // Function to set the options for global trigger tool
    void setGlobalTriggerConfiguration(std::vector<std::string>, std::vector<std::string>, std::vector<std::string>, std::vector<std::string>);
    inline bool useGlobalTrigger() const {return m_trigGlobalConfiguration.isActivated;} // Was this requested by the
                                                                                         // user
    inline auto const& getGlobalTriggers() const {return m_trigGlobalConfiguration.trigger;}
    inline auto const& getGlobalTriggersLoose() const {return m_trigGlobalConfiguration.trigger_loose;}
    inline bool useGlobalTriggerConfiguration() const {return m_trigGlobalConfiguration.isConfigured;} // Was this
                                                                                                       // subsequently
                                                                                                       // configured
    inline std::vector<std::string> getGlobalTriggerElectronSystematics() const {return m_trigGlobalConfiguration.electron_trigger_systematics;}
    inline std::vector<std::string> getGlobalTriggerMuonSystematics()     const {return m_trigGlobalConfiguration.muon_trigger_systematics;}
    inline std::vector<std::string> getGlobalTriggerElectronTools()       const {return m_trigGlobalConfiguration.electron_trigger_tool_names;}
    inline std::vector<std::string> getGlobalTriggerMuonTools()           const {return m_trigGlobalConfiguration.muon_trigger_tool_names;}
    
    inline const TreeFilter* getTreeFilter() const { return m_treeFilter.get();}

    inline const std::unordered_map<std::string, std::string>& GetMCMCTranslator() const {return m_showerMCMCtranslator;}
    
  private:
    // Prevent any more configuration
    bool m_configFixed;

    // Make all CP Tools, or just the SF
    bool m_makeAllCPTools;

    // GRL file
    std::string m_grlDir;
    std::vector<std::string> m_grlFile = {};

    // TDP
    std::string m_topDataPreparationPath;

    // Use which objects
    bool m_usePhotons;
    bool m_useElectrons;
    bool m_useFwdElectrons;
    bool m_useMuons;
    bool m_useSoftMuons;
    bool m_useAntiMuons;
    bool m_useTaus;
    bool m_useJets;
    bool m_useLargeRJets;
    bool m_useTrackJets;
    bool m_useRCJets;
    bool m_useVarRCJets;

    // Ghost Tracks on jets can not really be disabled because they are always
    // available. However, we want the systematics to be executed automatically
    // whenever the user has "configured" ghost tracks.
    bool m_useJetGhostTrack;

    bool m_useTracks;

    // Are we using particle flow jets
    // Need this as some things aren't supported at the moment
    // C++11 so nice inline initialization (we should make this consistent)
    bool m_useParticleFlowJets = false;

    bool m_useTruthParticles;
    bool m_useTruthElectrons;
    bool m_useTruthMuons;
    bool m_useTruthPhotons;
    bool m_useTruthJets;
    bool m_useTruthLargeRJets;
    bool m_useTruthTaus;
    bool m_useTruthMET;

    // define if d0/z0 cut should be used at all
    bool m_applyTTVACut;

    bool m_demandPriVtx; // whether at leas one primary vertex is required for each event

    std::string m_jetSubstructureName;

    // recompute CP vars?
    bool m_recomputeCPvars;

    // Store in config a boolean which lets us know if we called the nominal object setup
    bool m_isNominalAvailable;

    // Do systematics? - this needs many more configuration options
    std::string m_systematics;
    std::string m_nominalSystName;
    std::string m_allSystName;

    unsigned int m_DSID;
    unsigned int m_MapIndex;
    SampleXsection::showering m_showeringAlgo;
    bool m_is_sherpa_22_vjets = false;

    bool m_isMC;
    bool m_isAFII;
    std::vector<std::string> m_filterBranches, m_filterPartonLevelBranches, m_filterParticleLevelBranches, m_filterNominalLooseBranches;
    std::string m_generators;
    std::string m_AMITag;
    bool m_isPrimaryxAOD;
    bool m_isTruthDxAOD = false;
    std::string m_derivationStream;
    std::string m_amiTag;
    int m_amiTagSet = 0;

    // Do fakes MM weights calculation? - only for data loose
    bool m_doFakesMMWeightsIFF;
    // Configurations for MM fake estimate
    std::string m_FakesMMConfigIFF;
    // Debug mode?
    bool m_doFakesMMIFFDebug;
    
    //options to select if you want to use loose objects for MET rebuilding instead of the tight ones
    bool m_useLooseObjectsInMETInLooseTree;
    bool m_useLooseObjectsInMETInNominalTree;
    //this will write a separate branch with the met built using loose objects
    bool m_writeMETBuiltWithLooseObjects;

    // By default the top group does overlap removal on
    // the tight lepton definitions.
    // If you turn this to true you are going off piste and need to report
    // your findings to the top reconstruction meeting.
    bool m_doOverlapRemovalOnLooseLeptonDef;

    // String to describe which overlap removal procedure is used
    // Current options are whether to use the ASG recommended
    // overlap removal (recommended)
    // procedure, or the one developed by the harmonization
    // task force and use throughout 2015 (harmonized)
    std::string m_overlap_removal_procedure = "recommended";

    float m_overlapRemovalSlidingInnerDRel = 0.2;
    float m_overlapRemovalSlidingInnerDRmu = 0.2;

    // do overlap removal also with large-R jets
    // (using whatever procedure is used in the official tools)
    bool m_doLargeJetOverlapRemoval;

    // Dumps the normal, non-"*_Loose" trees (on demand)
    bool m_doTightEvents;
    // Dumps the "*_Loose trees (on demand)
    bool m_doLooseEvents;

    // Run systematics on the given selection
    bool m_doTightSysts;
    bool m_doLooseSysts;

    // In the *_Loose trees, lepton SFs are calculated considering
    // tight ID and isolation instead of loose
    // Only tight leptons are considered in the event SF calculation
    bool m_applyTightSFsInLooseTree;

    // Apply Electron In Jet subtraction
    // For boosted analysis
    bool m_applyElectronInJetSubtraction;

    // Write Truth block info
    bool m_doTruthBlockInfo;

    // Write Truth PDF info
    bool m_doTruthPDFInfo;
    bool m_doTruthPDFInfoInNominalTrees;

    // Write MC generator weights
    bool m_doMCGeneratorWeights;
    bool m_doMCGeneratorWeightsInNominalTrees;

    // list of names of nominal weight
    // attempts to find nominal weight in the order as specified here
    std::vector<std::string> m_nominalWeightNames;
    std::string m_nominalWeightName;
    size_t m_nominalWeightIndex;
    size_t m_MCweightsSize;
    bool m_forceWeightIndex; // to force useage of index instead of metadata

    // Top Parton History
    bool m_doTopPartonHistory;
    bool m_isTopPartonHistoryRegisteredInNtuple;
    bool m_doTopPartonLevel;

    // Top Particle Level
    bool m_doTopParticleLevel;
    bool m_doParticleLevelOverlapRemovalMuJet;
    bool m_doParticleLevelOverlapRemovalElJet;
    bool m_doParticleLevelOverlapRemovalJetPhoton;

    // KLFitter
    bool m_doKLFitter;
    std::string m_KLFitterTransferFunctionsPath;
    std::string m_KLFitterOutput;
    std::string m_KLFitterJetSelectionMode;
    std::string m_KLFitterBTaggingMethod;
    std::string m_KLFitterLH;
    bool m_KLFitterTopMassFixed;
    bool m_KLFitterSaveAllPermutations;
    bool m_KLFitterFailOnLessThanXJets;

    // PseudoTop
    bool m_doPseudoTop;

    // StoreGate / TStore keys
    std::string m_sgKeyDummy;
    std::string m_sgKeyEventInfo;
    std::string m_sgKeyPrimaryVertices;
    std::string m_sgKeyPhotons;
    std::string m_sgKeyElectrons;
    std::string m_sgKeyFwdElectrons;
    std::string m_sgKeyMuons;
    std::string m_sgKeySoftMuons;
    std::string m_sgKeyTaus;
    std::string m_sgKeyJets;
    std::string m_sgKeyJetsType;
    std::string m_sgKeyLargeRJets;
    std::string m_sgKeyTrackJets;
    std::string m_sgKeyTrackJetsType;

    std::string m_sgKeyMissingEt;
    std::string m_sgKeyMissingEtLoose;

    std::string m_sgKeyTruthEvent;
    std::string m_sgKeyMCParticle;
    std::string m_sgKeyTruthPhotons;
    std::string m_sgKeyTruthElectrons;
    std::string m_sgKeyTruthMuons;
    std::string m_sgKeyTruthTaus;
    std::string m_sgKeyTruthJets;
    std::string m_sgKeyTruthLargeRJets;
    std::string m_sgKeyTruthMET;
    std::string m_sgKeyTopPartonHistory;

    std::string m_sgKeyTopSystematicEvents;
    std::string m_sgKeyTopSystematicEventsLoose;
    std::string m_passEventSelectionDecoration;

    std::string m_decoKeyJetGhostTrack;
    std::vector<std::string> m_jetGhostTrackSystematics;
    std::vector<std::uint32_t> m_jetGhostTrackRunPeriods;

    std::string m_sgKeyInDetTrackParticles;    
    std::string m_sgKeyTracks;
    std::string m_sgKeyTracksType;
    std::vector<std::uint32_t> m_trackRunPeriods;

    

    float m_jetResponseMatchingDeltaR;

    // special: allow to dump the systematics-shifted b-tagging SFs in the systematics trees
    bool m_dumpBtagSystsInSystTrees;

    bool m_storePerJetBtagSFs;

    // Electron configuration
    std::string m_egammaSystematicModel;
    std::string m_electronEfficiencySystematicModel;
    std::string m_electronEfficiencySystematicModelEtaBinning;
    std::string m_electronEfficiencySystematicModelEtBinning;
    std::string m_electronID;
    std::string m_electronIDLoose;
    bool m_electronVetoLArCrack;
    float m_electronPtcut;
    std::string m_electronIsolation;
    std::string m_electronIsolationLoose;
    std::string m_electronIsolationSF;
    std::string m_electronIsolationSFLoose;
    bool const m_electronIsoSFs; // no longer supported, always true (use m_electronIsolationSF instead)
    int m_electron_d0SigCut;
    float m_electron_delta_z0;

    std::string m_electronIDDecoration;
    std::string m_electronIDLooseDecoration;
    bool m_useElectronChargeIDSelection;
    bool m_useEgammaLeakageCorrection;
    bool m_enablePromptLeptonImprovedVetoStudies;

    //Fwd electron configuration
    float m_fwdElectronPtcut;
    float m_fwdElectronMinEtacut;
    float m_fwdElectronMaxEtacut;
    std::string m_fwdElectronID;
    std::string m_fwdElectronIDLoose;
    int m_fwdElectronBCIDCleaningMinRun;
    int m_fwdElectronBCIDCleaningMaxRun;

    // Muon configuration
    float m_muonPtcut; // muon object selection pT cut
    float m_muonEtacut; // muon object selection (abs) eta cut
    std::string m_muonQuality; // muon quality used in object selection
    bool m_muonUseMVALowPt; //to turn on MVA for low-pT muons
    bool m_muonUse2stationMuonsHighPt; //to allow muon reco with 2-station
    std::string m_muonQualityLoose; // loose muon quality used in object selection
    bool m_muonUseMVALowPtLoose; //to turn on MVA for low-pT muons (loose tree)
    bool m_muonUse2stationMuonsHighPtLoose; //to allow muon reco with 2-station (loose tree)
    std::string m_muonIsolation;
    std::string m_muonIsolationLoose;
    std::string m_muonIsolationSF;
    std::string m_muonIsolationSFLoose;
    int m_muon_d0SigCut;
    float m_muon_delta_z0;
    bool m_muonMuonDoSmearing2stationHighPt; //to turn on/off special correction for the reco with 2-station muons with missing inner MS station allowed for abs(eta)<1.3, only HighPt WP
    bool m_muonMuonDoExtraSmearingHighPt; //to turn on/off a special correction for the muon with high momenta.

    //Soft muon configuration
    float m_softmuonPtcut; // soft muon object selection pT cut
    float m_softmuonEtacut; // soft muon object selection (abs) eta cut
    std::string m_softmuonQuality; // soft muon quality used in object selection
    bool m_softmuonUseMVALowPt; //to turn on MVA for low-pT muons
    float m_softmuonDRJetcut; // soft muon object selection DR wrt jets cut
    bool m_softmuonDRJetcutUseRapidity; // true -> use rapidity for DR(jet,mu) matching; false -> use pseudorapidity
    bool m_softmuonAdditionalTruthInfo; //additional info on the particle-level origin of the muon, see TopParticleLevel/TruthTools.h
    bool m_softmuonAdditionalTruthInfoCheckPartonOrigin; //additional info on the parton-level origin of the muon, see TopParticleLevel/TruthTools.h
    bool m_softmuonAdditionalTruthInfoDoVerbose; //to help debugging the above options
    
    // Jet configuration
    float m_jetPtcut; // jet object selection pT cut
    float m_jetEtacut; // jet object selection (abs) eta cut
    float m_jetPtGhostTracks; // jet pt threshold for ghost track systematic variations calculation
    float m_jetEtaGhostTracks; // jet eta threshold for ghost track systematic variations calculation
    std::string m_jetUncertainties_NPModel; // AllNuisanceParameters, 19NP or 3NP
    std::string m_jetUncertainties_QGFracFile; // to improve Flavour composition and response
    std::vector<std::string> m_jetUncertainties_QGHistPatterns; // to improve Flavour composition and response, with
                                                                // more flexibility
    bool m_doMultipleJES;
    std::string m_jetJERSmearingModel; // Full or Simple
    std::string m_jetCalibSequence; // GCC or JMS
    bool m_jetStoreTruthLabels; // True or False
    bool m_doJVTInMETCalculation;
    bool m_saveFailJVTJets;
    std::string m_JVTWP;
    bool m_doForwardJVTInMETCalculation;
    bool m_saveFailForwardJVTJets;
    std::string m_fJVTWP;

    // MET configuration
    std::string m_METUncertaintiesConfigDir; //Path prefix for directory with MET calibration configs
    
    //Ghost tracks quality
    float m_ghostTrackspT;
    std::string m_ghostTracksVertexAssociation;
    std::string m_ghostTracksQuality;

    // Large R jet configuration
    float m_largeRJetPtcut; // large R jet object selection pT cut
    float m_largeRJetEtacut; // large R jet object selection (abs) eta cut
    std::map<std::string,std::string> m_largeRJetSubstructureVariables;
    std::string m_largeRJetUncertainties_NPModel; //large R JES/(plus old JMS, JMR, JER) uncertainties configuration
    std::string m_largeRJetUncertaintiesConfigDir; //Relative path to directory with large R JES config
                                                  // file
    //See https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetUncertaintiesRel21Summer2019LargeR
    std::string m_largeRJESJMSConfig; // large R jet JES/JMS calibration choice - see ANALYSISTO-210

    // Track jet configuration
    float m_trackJetPtcut; // track jet object selection pT cut
    float m_trackJetEtacut; // track jet object selection (abs) eta cut

    // Tracks
    float m_trackPtcut; // track object selection pT cut
    float m_trackEtacut; // track object selection (abs) eta cut

    // Jet configuration for reclustered jets
    float m_RCJetPtcut;
    float m_RCJetEtacut;
    float m_RCInputJetPtMin;
    float m_RCInputJetEtaMax;
    float m_RCJetTrimcut;
    float m_RCJetRadius;
    bool m_useRCJetSubstructure;
    bool m_useRCJetAdditionalSubstructure;

    // Jet configuration for variable large-R jets
    float m_VarRCJetPtcut;
    float m_VarRCJetEtacut;
    float m_VarRCJetTrimcut;
    float m_VarRCJetMaxRadius;
    std::string m_VarRCJetRho;
    std::string m_VarRCJetMassScale;
    bool m_useVarRCJetSubstructure;
    bool m_useVarRCJetAdditionalSubstructure;

    std::string m_trackQuality; // track quality to be used in track selection                                                                                                                              

    // these are needed for the top mass analysis, per default should be 1.0
    float m_JSF;
    float m_bJSF;

    // Tau configuration
    struct {
      // filename to load tau configuration from.
      // If this is an empty string then take the 'default'
      std::string fileName = "Default";
      // The jetIDWP in CamelCase
      // see
      // https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauSelectionTool.rst
      // for supported WPs
      std::string jetIDWP = "RNNMedium";
      bool doRNNID = true;
      bool doBDTID = false;
      // the electron BDTWP
      std::string eleBDTWP = "Medium";
      bool substructureSF = false;
      // Whether to perform electron overlap removal
      bool eleOLR = false;
      // pT cut on taus
      float pt = 20000;
      std::string etaRegions="[0., 1.37, 1.52, 2.5]";
    } m_tau_configuration, m_tau_configuration_loose;

    // photon configuration
    struct {
      // pT cut on photons
      float pt = 25000;
      float eta = 2.5;
      std::string isolation = "None";
      std::string identification = "None";
    } m_photon_configuration, m_photon_configuration_loose;

    // [[[-----------------------------------------------
    // Particle Level (truth) configuration

    // electrons
    struct {
      float PtCut;           // [ParticleLevel / Truth] Electron Object
      // Selection minimum pT Cut (Standard ATLAS
      // units, [MeV]).
      float EtaCut;          // [ParticleLevel / Truth] Electron Object
      // Selection maximum absolute eta Cut.
      bool NotFromHadron;    // [ParticleLevel / Truth] Whether to apply the
      // 'NotFromHadron' requirement onto the objects.
      bool TauIsHadron;      // [ParticleLevel / Truth] Whether a tauon is a
      // hadron during the 'NotFromHadron' check
    } m_truth_electron;

    // muons
    struct {
      float PtCut;           // [ParticleLevel / Truth] Muon Object
      // Selection minimum pT Cut (Standard ATLAS
      // units, [MeV]).
      float EtaCut;          // [ParticleLevel / Truth] Muon Object
      // Selection maximum absolute eta Cut.
      bool NotFromHadron;    // [ParticleLevel / Truth] Whether to apply the
      // 'NotFromHadron' requirement onto the objects.
      bool TauIsHadron;      // [ParticleLevel / Truth] Whether a tauon is a
      // hadron during the 'NotFromHadron' check
    } m_truth_muon;
    
    // soft muons
    struct {
      float PtCut;           // [ParticleLevel / Truth] Muon Object
      // Selection minimum pT Cut (Standard ATLAS
      // units, [MeV]).
      float EtaCut;          // [ParticleLevel / Truth] Muon Object
      // Selection maximum absolute eta Cut.
    } m_truth_softmuon;

    // photons
    struct {
      float PtCut;           // [ParticleLevel / Truth] Photon Object
      // Selection minimum pT Cut (Standard ATLAS
      // units, [MeV]).
      float EtaCut;          // [ParticleLevel / Truth] Photon Object
      // Selection maximum absolute eta Cut.
      std::string Origin;    // [ParticleLevel / Truth] Photon Object
      // Selection truth origin configuration.
      std::string Isolation; // [ParticleLevel / Truth] Photon Object
      // Selection truth isolation configuration.
    } m_truth_photon;

    // taus
    struct {
      float PtCut;           // [ParticleLevel / Truth] Tau Object
      // Selection minimum pT Cut (Standard ATLAS
      // units, [MeV]).
      float EtaCut;          // [ParticleLevel / Truth] Tau Object
      // Selection maximum absolute eta Cut.
    } m_truth_tau;

    // normal / large R jets
    struct {
      float PtCut;           // [ParticleLevel / Truth] Jet Object
      // Selection minimum pT Cut (Standard ATLAS
      // units, [MeV]).
      float EtaCut;          // [ParticleLevel / Truth] Jet Object
      // Selection maximum absolute eta Cut.
    } m_truth_jet, m_truth_largeRJet;


    // -----------------------------------------------]]]

    // Options for upgrade studies
    bool m_HLLHC;
    bool m_HLLHCFakes;

    // Boosted jet taggers requested by user
    std::vector<std::pair<std::string, std::string> > m_chosen_boostedJetTaggers;
    std::unordered_map<std::string, std::string> m_boostedTaggerSFnames;

    // B-tagging WPs requested by the user (updated to pair of string to hold algorithm and WP)
    std::vector<std::pair<std::string, std::string> > m_chosen_btaggingWP;
    std::vector<std::pair<std::string, std::string> > m_chosen_btaggingWP_caloJet;
    std::vector<std::pair<std::string, std::string> > m_chosen_btaggingWP_trkJet;
    // B-tagging systematics requested by user to be excluded from EV treatment, separated by semi-colons
    std::string m_bTagSystsExcludedFromEV = "";

    // list of B-tagging WP actualy available
    std::vector<std::string> m_available_btaggingWP;
    std::vector<std::string> m_available_btaggingWP_trkJet;
    // list of B-tagging WP actualy calibrated
    std::vector<std::string> m_calibrated_btaggingWP;
    std::vector<std::string> m_calibrated_btaggingWP_trkJet;
    // list of B-tagging algorithms requested
    std::set<std::string> m_available_btaggingAlgos;
    std::set<std::string> m_available_btaggingAlgos_trkJet;
    bool m_MV2c10_algo_used = false;
    bool m_MV2c10_algo_used_trkJet = false;

    std::unordered_map<std::string, std::string> m_algo_selTools;
    std::unordered_map<std::string, std::string> m_algo_selTools_trkJet;

    // B-tagging calibration to be used
    bool m_cdi_path_warning = false;
    std::string m_btagging_cdi_path = "Default";
    std::string m_btagging_calibration_B = "default";
    std::string m_btagging_calibration_C = "default";
    std::string m_btagging_calibration_Light = "default";

    // b-tag SF helpers - one of each per WP
    // will be set in the BTagScaleFactorCalculator
    // These are the base names of the SF systematics (without __up/__down)
    std::unordered_map<std::string, std::set<std::string> > bTag_base_names;
    std::unordered_map<std::string, std::set<std::string> > bTag_base_names_trkJet;
    // and those of them which are not eigenvectors
    std::unordered_map<std::string, std::set<std::string> > bTag_named_systs;
    std::unordered_map<std::string, std::set<std::string> > bTag_named_systs_trkJet;
    // how many B eigen-variations do we have?
    std::unordered_map<std::string, unsigned int> bTag_eigen_B;
    std::unordered_map<std::string, unsigned int> bTag_eigen_B_trkJet;
    // how many C eigen-variations do we have?
    std::unordered_map<std::string, unsigned int> bTag_eigen_C;
    std::unordered_map<std::string, unsigned int> bTag_eigen_C_trkJet;
    // how many Light eigen-variations do we have?
    std::unordered_map<std::string, unsigned int> bTag_eigen_light;
    std::unordered_map<std::string, unsigned int> bTag_eigen_light_trkJet;

    // LHAPDF options
    struct {
      std::vector<std::string> pdf_set_names = {};
      std::string baseLHAPDF = ""; // used only if XF1*XF2==0

      // options
      bool doLHAPDF = false;
      bool doLHAPDFInNominalTrees = false;

      bool save_per_event = false;

      // we pass the results back to config in PDFScaleFactor::finalize
      std::unordered_map< std::string, std::vector<float> > pdf_results;
    } m_lhapdf_options;

    // Pile-up reweighting
    struct {
      std::vector<std::string> lumi_calc_files = {};

      std::vector<std::string> config_files = {};

      // R21 - Need to allow configuration for FS and AF2
      std::vector<std::string> config_files_FS = {};
      std::vector<std::string> config_files_AF = {};
      std::vector<std::string> actual_mu_FS = {};
      std::vector<std::string> actual_mu_AF = {};
      float unrepresented_data_tol = 0.05;

      bool apply = false;

      bool use_grl_tool = false;

      // Whether or not to perform mu dependent
      // pile-up reweighting. Optional because can
      // result in a possible loss of statistics
      // that are crucial for things like MVA training.
      bool mu_dependent = true;

      std::vector<double> custom_SF = {};

      std::vector<int> periodAssignments = {};
    } m_pileup_reweighting;

    // Struct for holding TrigGlobalEfficiencyCorrectionTool settings in order to
    // manage systematic variations through this tool

    struct {
      typedef std::unordered_map<std::string, std::vector<std::string> > triggermap_t;
      // -- Set from cutfile --//
      // Boolean to be set to true if the user activates a flag
      bool isActivated = false;
      // Maps of periods -> list of triggers
      triggermap_t trigger;
      triggermap_t trigger_loose;

      // -- Set from TopCPTools  --//
      // Boolean to be set to true if we set this information
      bool isConfigured = false;
      // Names of CP::SystematicSet from electron trigger tools
      std::vector<std::string> electron_trigger_systematics;
      // Names of CP::SystematicSet from muon trigger tools
      std::vector<std::string> muon_trigger_systematics;
      // Name of the underlying electron tools, to be accessed and passes CP::SystematicSet
      std::vector<std::string> electron_trigger_tool_names;
      // Name of the underlying muon tools, to be accessed and passes CP::SystematicSet
      std::vector<std::string> muon_trigger_tool_names;
    } m_trigGlobalConfiguration;

    // Muon Trigger SF configuration
    std::string m_muon_trigger_SF;


    // Selections
    std::shared_ptr<std::vector<std::string> > m_allSelectionNames;
    // Trigger configuration
    // First string is the selection name, second string is the trigger
    std::shared_ptr<std::unordered_map<std::string, std::vector<std::string> > > m_allTriggers_Tight;
    std::shared_ptr<std::unordered_map<std::string, std::vector<std::string> > > m_electronTriggers_Tight;
    std::shared_ptr<std::unordered_map<std::string, std::vector<std::string> > > m_muonTriggers_Tight;
    std::shared_ptr<std::unordered_map<std::string, std::vector<std::string> > > m_tauTriggers_Tight;
    std::shared_ptr<std::unordered_map<std::string, std::vector<std::string> > > m_allTriggers_Loose;
    std::shared_ptr<std::unordered_map<std::string, std::vector<std::string> > > m_electronTriggers_Loose;
    std::shared_ptr<std::unordered_map<std::string, std::vector<std::string> > > m_muonTriggers_Loose;
    std::shared_ptr<std::unordered_map<std::string, std::vector<std::string> > > m_tauTriggers_Loose;
    std::vector<std::string> m_dummyTrigger;

    // Where the sum of event weights
    // before derivation framework is kept
    std::string m_sumOfEventWeightsMetaData;

    // Object Selector name
    std::string m_objectSelectionName;
    // Output Format
    std::string m_outputFormat;
    // Output Events
    std::string m_outputEvents;
    bool m_saveOnlySelectedEvents;
    // SetAutoFlush(0) on EventSaverFlatNtuple for ANALYSISTO-44 workaround
    bool m_outputFileSetAutoFlushZero;
    // Better configurable settings for TTree memory optimisation (ANALYSISTO-44, ANALYSISTO-463)
    int m_outputFileNEventAutoFlush;
    int m_outputFileBasketSizePrimitive;
    int m_outputFileBasketSizeVector;
    // Number of events to run on (for testing)
    unsigned int m_numberOfEventsToRun;

    // Number of events to skip (for testing)
    unsigned int m_numberOfEventsToSkip;

    // AOD meta-data access service
    AodMetaDataAccess* m_aodMetaData;

    // Systematics
    std::size_t m_nominalHashValue;

    std::shared_ptr<std::unordered_set<std::size_t> > m_systHashPhotons;
    std::shared_ptr<std::unordered_set<std::size_t> > m_systHashElectrons;
    std::shared_ptr<std::unordered_set<std::size_t> > m_systHashFwdElectrons;
    std::shared_ptr<std::unordered_set<std::size_t> > m_systHashMuons;
    std::shared_ptr<std::unordered_set<std::size_t> > m_systHashSoftMuons;
    std::shared_ptr<std::unordered_set<std::size_t> > m_systHashTaus;
    std::shared_ptr<std::unordered_set<std::size_t> > m_systHashJets;
    std::shared_ptr<std::unordered_set<std::size_t> > m_systHashLargeRJets;
    std::shared_ptr<std::unordered_set<std::size_t> > m_systHashTrackJets;
    std::shared_ptr<std::unordered_set<std::size_t> > m_systHashTracks;
    std::shared_ptr<std::unordered_set<std::size_t> > m_systHashMET;

    std::shared_ptr<std::unordered_set<std::size_t> > m_systHashAll;
    std::shared_ptr<std::list<std::size_t> > m_list_systHashAll;

    std::shared_ptr<std::unordered_set<std::size_t> > m_systHash_electronInJetSubtraction;
    std::shared_ptr<std::list<std::size_t> > m_list_systHash_electronInJetSubtraction;

    std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > m_systMapPhotons;
    std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > m_systMapElectrons;
    std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > m_systMapFwdElectrons;
    std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > m_systMapMuons;
    std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > m_systMapSoftMuons;
    std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > m_systMapTaus;
    std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > m_systMapJets;
    std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > m_systMapLargeRJets;
    std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > m_systMapTrackJets;
    std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > m_systMapTracks;
    std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > m_systMapMET;

    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapPhotons;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapElectrons;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapFwdElectrons;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapMuons;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapSoftMuons;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapTaus;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapJets;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapJets_electronInJetSubtraction;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapJetsLoose_electronInJetSubtraction;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapLargeRJets;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapTrackJets;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapTracks;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapMET;

    // For TopEvent/SingleSystEvent - will return the nominal key if not under variation
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapPhotons;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapElectrons;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapFwdElectrons;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapMuons;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapSoftMuons;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapTaus;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapJets;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapLargeRJets;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapTrackJets;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapTracks;

    // The boosted case is a bit more complex, we need additional collections
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapElectrons_electronInJetSubtraction;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapJets_electronInJetSubtraction;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapJetsLoose_electronInJetSubtraction;

    // For top::TopEventMaker
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapPhotonsTDS;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapPhotonsTDSAux;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapElectronsTDS;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapElectronsTDSAux;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapFwdElectronsTDS;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapFwdElectronsTDSAux;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapMuonsTDS;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapMuonsTDSAux;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapSoftMuonsTDS;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapSoftMuonsTDSAux;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapTausTDS;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapTausTDSAux;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapJetsTDS;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapJetsTDSAux;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapLargeRJetsTDS;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapLargeRJetsTDSAux;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapTrackJetsTDS;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapTrackJetsTDSAux;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapTracksTDS;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapTracksTDSAux;

    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapElectrons_electronInJetSubtractionTDS;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapElectrons_electronInJetSubtractionTDSAux;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapJets_electronInJetSubtractionTDS;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapJets_electronInJetSubtractionTDSAux;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDS;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDSAux;

    // Missing ET
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapMissingET;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapMissingETLoose;

    // KLFitter
    std::string m_sgKeyKLFitter;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapKLFitter;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapKLFitterLoose;

    // PseudoTop
    std::string m_sgKeyPseudoTop;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapPseudoTop;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systSgKeyMapPseudoTopLoose;

    // Map from systematic hash to CP::SystematicSet
    std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > m_systMapJetGhostTrack;
    // Map from systematic hash to decoration key.
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systDecoKeyMapJetGhostTrack;

    // Output TTree names
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systAllTTreeNames;
    std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systPersistantAllTTreeNames;
    // Output TTree index
    std::shared_ptr<std::unordered_map<std::size_t, unsigned int> > m_systAllTTreeIndex;
    std::shared_ptr<std::unordered_map<std::size_t, unsigned int> > m_systAllTTreeLooseIndex;

    // Private function only to simplify the setting of AFII values
    void ReadIsAFII(top::ConfigurationSettings* const& settings);

    // Int holding the release series value
    int m_release_series;

    // Bool to hold whether we generate and store poisson bootstrap weights
    bool m_saveBootstrapWeights;
    int m_BootstrapReplicas;

    // Switch to use BadBatmanCleaning
    bool m_useBadBatmanCleaning;
    unsigned int m_badBatmanCleaningMin;
    unsigned int m_badBatmanCleaningMax;

    // Switch to use event-level jet cleaning tool for testing
    bool m_useEventLevelJetCleaningTool;
    
    std::shared_ptr<TreeFilter> m_treeFilter;

    std::unordered_map<std::string, std::string> m_showerMCMCtranslator;

    std::string m_year;

    //ReadFloatOption
    float readFloatOption(top::ConfigurationSettings* const& settings, std::string in) const;
  };

  std::ostream& operator << (std::ostream& os, const TopConfig& config);
}  // namespace top

#endif
