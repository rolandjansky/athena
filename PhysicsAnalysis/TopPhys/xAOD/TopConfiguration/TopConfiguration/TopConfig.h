/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

namespace top{

class AodMetaDataAccess;
class ConfigurationSettings;

class TopConfig final {
 public:
  TopConfig();
  explicit TopConfig( const top::TopPersistentSettings* settings );
  virtual ~TopConfig(){}

  // Delete Standard constructors
  TopConfig( const TopConfig& rhs ) = delete;
  TopConfig( TopConfig&& rhs ) = delete;
  TopConfig& operator=( const TopConfig& rhs ) = delete;

  // create settings to persist to mini-xAOD
  top::TopPersistentSettings* createPersistentSettings();

  void setConfigSettings( top::ConfigurationSettings* const& settings );

  // Prevent any more configuration
  void fixConfiguration();

  // Make all CP Tools?
  inline bool makeAllCPTools() const {return m_makeAllCPTools;}

  // GRL file
  void setGrlDir( const std::string& s );
  inline const std::string& grlDir() const {return m_grlDir;}
  void setGrlFile( const std::string& s );
  inline const std::vector<std::string> grlFile() const {return m_grlFile;}

  // TDP path
  void setTDPPath( const std::string& s);
  inline const std::string& getTDPPath() const { return m_topDataPreparationPath; }


  // What objects are we using
  inline bool usePhotons()    const {return m_usePhotons;   }
  inline bool useElectrons()  const {return m_useElectrons; }
  inline bool useMuons()      const {return m_useMuons;     }
  inline bool useTaus()       const {return m_useTaus;      }
  inline bool useJets()       const {return m_useJets;      }
  inline bool useLargeRJets() const {return m_useLargeRJets;}
  inline bool useTrackJets()  const {return m_useTrackJets; }
  inline bool useJetGhostTrack()  const {return m_useJetGhostTrack; }
  inline bool useRCJets()     const {return m_useRCJets;}
  inline bool useVarRCJets()  const {return m_useVarRCJets;}

  inline bool useParticleFlowJets() const {return m_useParticleFlowJets;}

  inline bool useTruthParticles() const {return m_useTruthParticles;}
  inline bool useTruthElectrons()  const {return m_useTruthElectrons; }
  inline bool useTruthMuons()  const {return m_useTruthMuons; }
  inline bool useTruthPhotons()  const {return m_useTruthPhotons; }
  inline bool useTruthJets()  const {return m_useTruthJets; }
  inline bool useTruthLargeRJets()  const {return m_useTruthLargeRJets; }
  inline bool useTruthMET()  const {return m_useTruthMET; }

  inline bool applyTTVACut() const {return m_applyTTVACut; }

  inline bool useAntiMuons() const {return m_useAntiMuons; };

  // MC or not MC - this should be set by metadata
  inline bool isMC() const {return m_isMC;}
  inline void setIsMC(const bool value) {if(!m_configFixed){m_isMC = value;}}

  // AFII?
  inline bool isAFII() const {return m_isAFII;}
  inline void setIsAFII(const bool value) {if(!m_configFixed){m_isAFII = value;}}

  // Generators name
  inline std::string getGenerators() const {return m_generators;}
  inline void setGenerators(const std::string value) {if(!m_configFixed){m_generators = value;}}

  // AMITag
  inline std::string getAMITag() const {return m_AMITag;}
  inline void setAMITag(const std::string value) {if(!m_configFixed){m_AMITag = value;}}

  // Is this a Primary xAOD?
  inline bool isPrimaryxAOD() const {return m_isPrimaryxAOD;}
  inline void setIsPrimaryxAOD(const bool value) {if(!m_configFixed){m_isPrimaryxAOD = value;}}

  // Is this a Truth DxAOD?
  inline bool isTruthDxAOD() const {return m_isTruthDxAOD;}
  inline void setIsTruthDxAOD(const bool value) {if(!m_configFixed){m_isTruthDxAOD = value;}}

  // What derivation type is it?
  inline std::string getDerivationStream() const { return m_derivationStream;}
  inline void setDerivationStream(const std::string value) {if(!m_configFixed){m_derivationStream = value;}}

  // AMI tag from metadata
  std::string const & getAmiTag() const;
  void setAmiTag(std::string const & amiTag);

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
    if (dsid >= 363331 && dsid <= 363354)
      m_is_sherpa_22_vjets = true;
    if (dsid >= 363436 && dsid <= 363483)
      m_is_sherpa_22_vjets = true;
    // Sherpa 2.2 Z+jets (from TopDataPrep)
    if (dsid >= 363102 && dsid <= 363122)
      m_is_sherpa_22_vjets = true;
    if (dsid >= 363361 && dsid <= 363435)
      m_is_sherpa_22_vjets = true;
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

  // Do fakes MM weight calculation
  inline bool doFakesMMWeights() const {return m_doFakesMMWeights;}

  // Directory of efficiency files for MM fake estimate
  inline std::string FakesMMDir() const {return m_FakesMMDir;}

  // DDebug mode for MM fake estimate
  inline bool FakesMMDebug() const {return m_doFakesMMDebug;}

  // enables calculation of MM weights
  // only possible for data loose
  // doing it on MC loose is explicitly forbidden
  inline void setFakesMMWeightsCalculation()
  {m_doFakesMMWeights = true;}
  inline void setFakesMMDir(const std::string dir)
  {m_FakesMMDir = dir;}
  inline void setFakesMMDebug()
  {m_doFakesMMDebug = true;}

  // By default the top group does overlap removal on the tight lepton definitions
  // If you use this you are going off piste and need to report
  // your findings to the top reconstruction meeting
  inline void setOverlapRemovalOnLooseLeptonDef()
  {if(!m_configFixed){m_doOverlapRemovalOnLooseLeptonDef = true;}}
  inline bool doOverlapRemovalOnLooseLeptonDef() const {return m_doOverlapRemovalOnLooseLeptonDef;}

  inline std::string overlapRemovalProcedure() const
  {return m_overlap_removal_procedure;}

  inline float overlapRemovalSlidingInnerDRel() const 
  {return m_overlapRemovalSlidingInnerDRel;} 

  inline float overlapRemovalSlidingInnerDRmu() const 
  {return m_overlapRemovalSlidingInnerDRmu;}

  // do overlap removal also with large-R jets
  // (using whatever procedure is used in the official tools)
  inline void setLargeJetOverlapRemoval()
  {if(!m_configFixed){m_doLargeJetOverlapRemoval = true;}}
  inline bool doLargeJetOverlapRemoval() const {return m_doLargeJetOverlapRemoval;}
  
  // In the *_Loose trees, lepton SFs are calculated considering
  // tight ID and isolation instead of loose
  // Only tight leptons are considered in the event SF calculation
  inline void setApplyTightSFsInLooseTree()
  {if(!m_configFixed){m_applyTightSFsInLooseTree = true;}}
  inline bool applyTightSFsInLooseTree() const {return m_applyTightSFsInLooseTree;}

  // Apply Electron In Jet subtraction
  // For boosted analysis
  inline bool applyElectronInJetSubtraction() const {return m_applyElectronInJetSubtraction;}
  inline void setApplyElectronInJetSubtraction()
  {if(!m_configFixed){m_applyElectronInJetSubtraction = true;}}

  // Dump truth block info
  inline bool doTruthBlockInfo() const {return m_doTruthBlockInfo;}
  inline void setTruthBlockInfo()
  {if(!m_configFixed){m_doTruthBlockInfo = true;}}

  // Write truth PDF info
  inline bool doTruthPDFInfo() const {return m_doTruthPDFInfo;}
  inline void setTruthPDFInfo()
  {if(!m_configFixed){m_doTruthPDFInfo = true;}}
  // Write truth PDF info in reco tree
  inline bool doTruthPDFInfoInNominalTrees() const {return m_doTruthPDFInfoInNominalTrees;}
  inline void setTruthPDFInfoInNominalTrees()
  {if(!m_configFixed){m_doTruthPDFInfoInNominalTrees = true;}}

  // Write MC generator weights
  inline bool doMCGeneratorWeights() const {return m_doMCGeneratorWeights;}
  inline void setMCGeneratorWeights()
  {if(!m_configFixed){m_doMCGeneratorWeights = true;}}
  // Write truth PDF info in reco tree
  inline bool doMCGeneratorWeightsInNominalTrees() const {return m_doMCGeneratorWeightsInNominalTrees;}
  inline void setMCGeneratorWeightsInNominalTrees()
  {if(!m_configFixed){m_doMCGeneratorWeightsInNominalTrees = true;}}

  // Top Parton History
  inline bool doTopPartonHistory() const {return m_doTopPartonHistory;}
  inline void setTopPartonHistory()
  {if(!m_configFixed){m_doTopPartonHistory = true;}}

  // Register Top Parton History in flat ntuple
  // This doesn't use m_configFixed - this is bad :(
  // This is a simple implementation of saving xAOD objects to ntuples
  // John has more advanced ideas on how to do this
  // This is just a quick fix for now to get the concept working
  inline bool isTopPartonHistoryRegisteredInNtuple() const {return m_isTopPartonHistoryRegisteredInNtuple;}
  inline void setTopPartonHistoryRegisteredInNtuple()
  {m_isTopPartonHistoryRegisteredInNtuple = true;}

  // TopParticleLevel
  inline bool doTopParticleLevel() const { return m_doTopParticleLevel; }
  inline void setTopParticleLevel() { if ( ! m_configFixed ){ m_doTopParticleLevel = true; } }

  inline bool doParticleLevelOverlapRemovalMuJet() const { return m_doParticleLevelOverlapRemovalMuJet; }
  inline bool doParticleLevelOverlapRemovalElJet() const { return m_doParticleLevelOverlapRemovalElJet; }
  inline bool doParticleLevelOverlapRemovalJetPhoton() const { return m_doParticleLevelOverlapRemovalJetPhoton; }
  inline void setParticleLevelOverlapRemovalMuJet( bool value ) { if ( ! m_configFixed ){ m_doParticleLevelOverlapRemovalMuJet = value; } }
  inline void setParticleLevelOverlapRemovalElJet( bool value ) { if ( ! m_configFixed ){ m_doParticleLevelOverlapRemovalElJet = value; } }
  inline void setParticleLevelOverlapRemovalJetPhoton( bool value ) { if ( ! m_configFixed ){ m_doParticleLevelOverlapRemovalJetPhoton = value; } }

  // KLFitter
  inline bool doKLFitter() const {return m_doKLFitter;}
  inline void setKLFitter(){if(!m_configFixed){m_doKLFitter = true;}}
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
  inline void setPseudoTop(){if(!m_configFixed){m_doPseudoTop = true;}}

  // Triggers
  inline virtual void allTriggers_Tight(std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> triggers)
  {if(!m_configFixed){m_allTriggers_Tight = triggers;}}

  inline virtual void electronTriggers_Tight(std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> triggers)
  {if(!m_configFixed){m_electronTriggers_Tight = triggers;}}

  inline virtual void muonTriggers_Tight(std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> triggers)
  {if(!m_configFixed){m_muonTriggers_Tight = triggers;}}

  inline virtual void tauTriggers_Tight(std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> triggers)
  {if(!m_configFixed){m_tauTriggers_Tight = triggers;}}

  inline virtual void allTriggers_Loose(std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> triggers)
  {if(!m_configFixed){m_allTriggers_Loose = triggers;}}

  inline virtual void electronTriggers_Loose(std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> triggers)
  {if(!m_configFixed){m_electronTriggers_Loose = triggers;}}

  inline virtual void muonTriggers_Loose(std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> triggers)
  {if(!m_configFixed){m_muonTriggers_Loose = triggers;}}

  inline virtual void tauTriggers_Loose(std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> triggers)
  {if(!m_configFixed){m_tauTriggers_Loose = triggers;}}

  inline std::shared_ptr<std::vector<std::string>> allSelectionNames() const {return m_allSelectionNames;}
  virtual const std::vector<std::string>& allTriggers_Tight(const std::string& selection) const;
  virtual const std::vector<std::string>& electronTriggers_Tight(const std::string& selection) const;
  virtual const std::vector<std::string>& muonTriggers_Tight(const std::string& selection) const;
  virtual const std::vector<std::string>& tauTriggers_Tight(const std::string& selection) const;

  virtual const std::vector<std::string>& allTriggers_Loose(const std::string& selection) const;
  virtual const std::vector<std::string>& electronTriggers_Loose(const std::string& selection) const;
  virtual const std::vector<std::string>& muonTriggers_Loose(const std::string& selection) const;
  virtual const std::vector<std::string>& tauTriggers_Loose(const std::string& selection) const;

  // StoreGate Keys
  virtual void sgKeyMCParticle      ( const std::string& s );
  virtual void sgKeyPhotons         ( const std::string& s );
  virtual void sgKeyElectrons       ( const std::string& s );
  virtual void sgKeyMuons           ( const std::string& s );
  virtual void sgKeyTaus            ( const std::string& s );
  virtual void sgKeyJets            ( const std::string& s );
  virtual void sgKeyLargeRJets      ( const std::string& s );
  virtual void sgKeyTrackJets       ( const std::string& s );

  virtual void sgKeyTruthElectrons  ( const std::string& s );
  virtual void sgKeyTruthMuons      ( const std::string& s );
  virtual void sgKeyTruthPhotons      ( const std::string& s );
  virtual void sgKeyTruthMET        ( const std::string& s );
  virtual void sgKeyTruthJets       ( const std::string& s );
  virtual void sgKeyTruthLargeRJets       ( const std::string& s );

  inline virtual void sgKeyMissingEt  ( const std::string& s ){if(!m_configFixed){m_sgKeyMissingEt = s; }}
  inline virtual void sgKeyMissingEtLoose  ( const std::string& s ){if(!m_configFixed){m_sgKeyMissingEtLoose = s; }}

  inline const std::string& sgKeyEventInfo()  const {return m_sgKeyEventInfo; }
  inline const std::string& sgKeyPrimaryVertices() const {return m_sgKeyPrimaryVertices;}
  inline virtual const std::string& sgKeyPhotons()    const {return m_sgKeyPhotons;   }
  inline virtual const std::string& sgKeyElectrons()  const {return m_sgKeyElectrons; }
  inline virtual const std::string& sgKeyMuons()      const {return m_sgKeyMuons;     }
  inline virtual const std::string& sgKeyTaus()       const {return m_sgKeyTaus;      }
  inline virtual const std::string& sgKeyJets()       const {return m_sgKeyJets;      }
  inline virtual const std::string& sgKeyLargeRJets() const {return m_sgKeyLargeRJets;}
  inline virtual const std::string& sgKeyTrackJets()  const {return m_sgKeyTrackJets; }
  inline virtual const std::string& sgKeyMissingEt()  const {return m_sgKeyMissingEt; }
  inline virtual const std::string& sgKeyMissingEtLoose()  const {return m_sgKeyMissingEtLoose; }
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
  virtual void decoKeyJetGhostTrack(const std::string & key);
  inline virtual const std::string & decoKeyJetGhostTrack() const {return m_decoKeyJetGhostTrack;}
  virtual void runPeriodJetGhostTrack(const std::vector<std::uint32_t> & vect);

  virtual const std::string& sgKeyPhotons   ( const std::size_t hash ) const;
  virtual const std::string& sgKeyElectrons ( const std::size_t hash ) const;
  virtual const std::string& sgKeyElectronsStandAlone ( const std::size_t hash ) const;
  virtual const std::string& sgKeyMuons     ( const std::size_t hash ) const;
  virtual const std::string& sgKeyTaus
    ( const std::size_t hash ) const;
  virtual const std::string& sgKeyJets      ( const std::size_t hash , const bool useLooseLeptonJets ) const;
  virtual const std::string& sgKeyJetsStandAlone ( const std::size_t hash ) const;
  virtual const std::string& sgKeyLargeRJets( const std::size_t hash ) const;
  virtual const std::string& sgKeyTrackJets ( const std::size_t hash ) const;
  virtual const std::string& sgKeyMissingEt ( const std::size_t hash ) const;
  virtual const std::string& sgKeyMissingEtLoose ( const std::size_t hash ) const;

  // For top::TopEventMaker
  const std::string& sgKeyPhotonsTDS( const std::size_t hash ) const;
  const std::string& sgKeyPhotonsTDSAux( const std::size_t hash ) const;
  const std::string& sgKeyElectronsTDS( const std::size_t hash ) const;
  const std::string& sgKeyElectronsTDSAux( const std::size_t hash ) const;
  const std::string& sgKeyMuonsTDS( const std::size_t hash ) const;
  const std::string& sgKeyMuonsTDSAux( const std::size_t hash ) const;
  const std::string& sgKeyTausTDS( const std::size_t hash ) const;
  const std::string& sgKeyTausTDSAux( const std::size_t hash ) const;
  const std::string& sgKeyJetsTDS( const std::size_t hash , const bool useLooseLeptonJets ) const;
  const std::string& sgKeyJetsTDSAux( const std::size_t hash , const bool useLooseLeptonJets ) const;
  const std::string& sgKeyLargeRJetsTDS( const std::size_t hash ) const;
  const std::string& sgKeyLargeRJetsTDSAux( const std::size_t hash ) const;
  const std::string& sgKeyTrackJetsTDS( const std::size_t hash ) const;
  const std::string& sgKeyTrackJetsTDSAux( const std::size_t hash ) const;

  // KLFitter
  const std::string& sgKeyKLFitter( const std::size_t hash ) const;
  const std::string& sgKeyKLFitterLoose( const std::size_t hash ) const;

  // PseudoTop
  const std::string& sgKeyPseudoTop( const std::size_t hash ) const;
  const std::string& sgKeyPseudoTop( const std::string ) const;
  const std::string& sgKeyPseudoTopLoose( const std::size_t hash ) const;
  const std::string& sgKeyPseudoTopLoose( const std::string ) const;

  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  // Jet Ghost-Track Systematics

  // The jet decoration name for a given systematic (nominal also possible).
  const std::string & decoKeyJetGhostTrack(const std::size_t hash) const;

  // Retrieve names of all (registered) systematics that affect ghost tracks.
  const std::vector<std::string> & systematicsJetGhostTrack() const {return m_jetGhostTrackSystematics;}
  
  // Retrieve run periods for ghost tracks.
  const std::vector<std::uint32_t> & runPeriodsJetGhostTrack() const {return m_jetGhostTrackRunPeriods;}

  // Retrieve mapping from systematic hash to CP::SystematicSet.
  inline std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> systMapJetGhostTrack()   const {return m_systMapJetGhostTrack;  }
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  
  // special: allow to dump the systematics-shifted b-tagging SFs in the systematics trees
  inline virtual void dumpBtagSystsInSystTrees( const bool& b ){if(!m_configFixed){m_dumpBtagSystsInSystTrees = b;}}
  inline virtual const bool& dumpBtagSystsInSystTrees(){return m_dumpBtagSystsInSystTrees;}

  // Electron configuration
  inline virtual void egammaSystematicModel( const std::string& s ){if(!m_configFixed){m_egammaSystematicModel = s;}}
  inline virtual void electronID( const std::string& s    ){if(!m_configFixed){m_electronID    = s;}}
  inline virtual void electronIDLoose( const std::string& s ){if(!m_configFixed){m_electronIDLoose = s;}}
  inline         void removeElectronVetoLArCrack()         {if(!m_configFixed){m_electronVetoLArCrack = false;}}
  inline virtual void electronVetoLArCrack( const bool b ){if(!m_configFixed){m_electronVetoLArCrack = b;}}
  inline virtual void electronPtcut(const float pt)       {if(!m_configFixed){m_electronPtcut = pt;}}
  inline virtual void electronIsolation(const std::string& iso) {if(!m_configFixed){m_electronIsolation = iso;}}
  inline virtual void electronIsolationLoose(const std::string& iso) {if(!m_configFixed){m_electronIsolationLoose = iso;}}
  void electronIsolationSF(std::string const & iso) {if(!m_configFixed){m_electronIsolationSF = iso;}}
  void electronIsolationSFLoose(std::string const & iso) {if(!m_configFixed){m_electronIsolationSFLoose = iso;}}
  inline virtual void useElectronChargeIDSelection(const std::string& s){if(!m_configFixed){ m_useElectronChargeIDSelection = (s=="True" || s=="true");}}

  inline virtual const std::string& egammaSystematicModel(){return m_egammaSystematicModel;}
  inline virtual const std::string& electronID()     const {return m_electronID;   }
  inline virtual const std::string& electronIDLoose()  const {return m_electronIDLoose;}
  inline virtual bool electronVetoLArCrack() const {return m_electronVetoLArCrack;}
  inline virtual float electronPtcut()       const {return m_electronPtcut;}
  inline virtual const std::string& electronIsolation() const {return m_electronIsolation;}
  inline virtual const std::string& electronIsolationLoose() const {return m_electronIsolationLoose;}
  std::string const & electronIsolationSF() const {return m_electronIsolationSF;}
  std::string const & electronIsolationSFLoose() const {return m_electronIsolationSFLoose;}
  inline virtual bool electronIsoSFs() const {return m_electronIsoSFs;}
  inline const std::string& electronIDDecoration() const {return m_electronIDDecoration;}
  inline const std::string& electronIDLooseDecoration() const {return m_electronIDLooseDecoration;}
  inline bool useElectronChargeIDSelection() const {return m_useElectronChargeIDSelection;}

  // Photon configuration
  inline virtual void photonPtcut(const float pt)             {if(!m_configFixed){m_photon_configuration.pt = pt;}}
  inline virtual void photonEtacut(const float eta)           {if(!m_configFixed){m_photon_configuration.eta = eta;}}
  inline virtual void photonIdentification(const std::string& quality) {if(!m_configFixed){m_photon_configuration.identification = quality;}}
  inline virtual void photonIdentificationLoose(const std::string& quality)  {if(!m_configFixed){m_photon_configuration_loose.identification = quality;}}
  inline virtual void photonIsolation(const std::string& quality) {if(!m_configFixed){m_photon_configuration.isolation = quality;}}
  inline virtual void photonIsolationLoose(const std::string& quality) {if(!m_configFixed){m_photon_configuration_loose.isolation = quality;}}
  inline virtual void photonUseRadiativeZ(const bool b)    {if(!m_configFixed){m_photon_configuration.useRadiativeZ = b;}}

  // Muon configuration
  inline virtual void muonPtcut(const float pt)       {if(!m_configFixed){m_muonPtcut = pt;}}
  inline virtual void muonEtacut(const float eta)     {if(!m_configFixed){m_muonEtacut = eta;}}
  inline virtual void muonQuality(const std::string& quality)       {if(!m_configFixed){m_muonQuality = quality;}}
  inline virtual void muonQualityLoose(const std::string& quality)  {if(!m_configFixed){m_muonQualityLoose = quality;}}
  inline virtual void muonIsolation(const std::string& iso) {if(!m_configFixed){m_muonIsolation = iso;}}
  inline virtual void muonIsolationLoose(const std::string& iso) {if(!m_configFixed){m_muonIsolationLoose = iso;}}
  void muonIsolationSF(std::string const & iso) {if(!m_configFixed){m_muonIsolationSF = iso;}}
  void muonIsolationSFLoose(std::string const & iso) {if(!m_configFixed){m_muonIsolationSFLoose = iso;}}

  inline virtual float muonPtcut() const {return m_muonPtcut;}
  inline virtual float muonEtacut() const {return m_muonEtacut;}
  inline virtual const std::string& muonQuality() const {return m_muonQuality;}
  inline virtual const std::string& muonQualityLoose() const {return m_muonQualityLoose;}
  inline virtual const std::string& muonIsolation() const {return m_muonIsolation;}
  inline virtual const std::string& muonIsolationLoose() const {return m_muonIsolationLoose;}
  std::string const & muonIsolationSF() const {return m_muonIsolationSF;}
  std::string const & muonIsolationSFLoose() const {return m_muonIsolationSFLoose;}

  // Jet configuration
  inline virtual void jetPtcut(const float pt)       {if(!m_configFixed){m_jetPtcut = pt;}}
  inline virtual void jetEtacut(const float eta)     {if(!m_configFixed){m_jetEtacut = eta;}}
  inline virtual void fwdJetAndMET(const std::string& fwd)  {if(!m_configFixed){m_fwdJetAndMET = fwd;}}

  inline virtual float jetPtcut()  const {return m_jetPtcut;}
  inline virtual float jetEtacut() const {return m_jetEtacut;}
  inline virtual const std::string& fwdJetAndMET() const {return m_fwdJetAndMET;}

  inline virtual void largeRJetPtcut(const float pt)    {if(!m_configFixed){m_largeRJetPtcut = pt;}}
  inline virtual void largeRJetEtacut(const float eta)  {if(!m_configFixed){m_largeRJetEtacut = eta;}}
  inline virtual void largeRJESUncertaintyConfig(const std::string& largeR_config) {if(!m_configFixed){m_largeRJESUncertaintyConfig = largeR_config;}}
  inline virtual void largeRJESJMSConfig(const std::string& largeR_config) {if(!m_configFixed){m_largeRJESJMSConfig = largeR_config;}}
  inline virtual void largeRtoptaggingConfigFile(const std::string& topTagging_config) {if(!m_configFixed){m_largeRtoptaggingConfigFile = topTagging_config;}}
  inline virtual float largeRJetPtcut()  const {return m_largeRJetPtcut;}
  inline virtual float largeRJetEtacut() const {return m_largeRJetEtacut;}
  inline virtual const std::string& largeRJESUncertaintyConfig() const {return m_largeRJESUncertaintyConfig;}
  inline virtual const std::string& largeRJESJMSConfig() const {return m_largeRJESJMSConfig;}
  inline virtual const std::string& largeRtoptaggingConfigFile() const {return m_largeRtoptaggingConfigFile;}

  inline virtual void trackJetPtcut(const float pt)    {if(!m_configFixed){m_trackJetPtcut = pt;}}
  inline virtual void trackJetEtacut(const float eta)  {if(!m_configFixed){m_trackJetEtacut = eta;}}
  inline virtual float trackJetPtcut()  const {return m_trackJetPtcut;}
  inline virtual float trackJetEtacut() const {return m_trackJetEtacut;}

  inline virtual float RCJetPtcut() const {return m_RCJetPtcut;}
  inline virtual float RCJetEtacut() const {return m_RCJetEtacut;}
  inline virtual float RCJetTrimcut() const {return m_RCJetTrimcut;}
  inline virtual float RCJetRadius() const {return m_RCJetRadius;}
  inline virtual bool  useRCJetSubstructure() const {return m_useRCJetSubstructure;}
  inline virtual bool  useRCJetAdditionalSubstructure() const {return m_useRCJetAdditionalSubstructure;}
 
  inline virtual void RCJetPtcut(const float pt)      {if(!m_configFixed){m_RCJetPtcut = pt;}}
  inline virtual void RCJetEtacut(const float eta)    {if(!m_configFixed){m_RCJetEtacut = eta;}}
  inline virtual void RCJetTrimcut(const float trim)  {if(!m_configFixed){m_RCJetTrimcut = trim;}}
  inline virtual void RCJetRadius(const float radius) {if(!m_configFixed){m_RCJetRadius = radius;}}
  inline virtual void useRCJetSubstructure(const bool use) {if (!m_configFixed){m_useRCJetSubstructure = use;}}
  inline virtual void useRCJetAdditionalSubstructure(const bool use) {if (!m_configFixed){m_useRCJetAdditionalSubstructure = use;}}
  
  inline virtual float VarRCJetPtcut() const{return m_VarRCJetPtcut;}
  inline virtual float VarRCJetEtacut() const {return m_VarRCJetEtacut;}
  inline virtual float VarRCJetTrimcut() const {return m_VarRCJetTrimcut;}
  inline virtual float VarRCJetMaxRadius() const {return m_VarRCJetMaxRadius;}
  inline virtual const std::string& VarRCJetRho() const {return m_VarRCJetRho;}
  inline virtual const std::string& VarRCJetMassScale() const {return m_VarRCJetMassScale;}
  inline virtual bool  useVarRCJetSubstructure() const {return m_useVarRCJetSubstructure;}
  inline virtual bool  useVarRCJetAdditionalSubstructure() const {return m_useVarRCJetAdditionalSubstructure;}

  inline virtual void VarRCJetPtcut(const float pt)      {if(!m_configFixed){m_VarRCJetPtcut = pt;}}
  inline virtual void VarRCJetEtacut(const float eta)    {if(!m_configFixed){m_VarRCJetEtacut = eta;}}
  inline virtual void VarRCJetTrimcut(const float trim)  {if(!m_configFixed){m_VarRCJetTrimcut = trim;}}
  inline virtual void VarRCJetMaxRadius(const float radius) {if(!m_configFixed){m_VarRCJetMaxRadius = radius;}}
  inline virtual void VarRCJetRho(const std::string& rho) {if(!m_configFixed){m_VarRCJetRho = rho;}}
  inline virtual void VarRCJetMassScale(const std::string& mass_scale) {if(!m_configFixed){m_VarRCJetMassScale = mass_scale;}}
  inline virtual void useVarRCJetSubstructure(const bool use) {if (!m_configFixed){m_useVarRCJetSubstructure = use;}}
  inline virtual void useVarRCJetAdditionalSubstructure(const bool use) {if (!m_configFixed){m_useVarRCJetAdditionalSubstructure = use;}}

  inline virtual void jetUncertainties_BunchSpacing( const std::string& s ){if(!m_configFixed){m_jetUncertainties_BunchSpacing = s;}}
  inline virtual const std::string& jetUncertainties_BunchSpacing() const {return m_jetUncertainties_BunchSpacing;}

  virtual void jetUncertainties_NPModel( const std::string& s );
  virtual void jetUncertainties_QGFracFile( const std::string& s );
  virtual void jetUncertainties_QGHistPatterns( const std::string& s );
  inline bool doMultipleJES() const {return m_doMultipleJES;}
  inline bool doLargeRSmallRCorrelations() const {return m_largeRSmallRCorrelations;}
  inline virtual const std::string& jetUncertainties_NPModel() const {return m_jetUncertainties_NPModel;}
  inline virtual const std::string& jetUncertainties_QGFracFile() const {return m_jetUncertainties_QGFracFile;}
  inline virtual const std::vector<std::string>& jetUncertainties_QGHistPatterns() const {return m_jetUncertainties_QGHistPatterns;}

  inline virtual void jetJERSmearingModel( const std::string& s ){if(!m_configFixed){m_jetJERSmearingModel = s;}}
  inline virtual const std::string& jetJERSmearingModel() const {return m_jetJERSmearingModel;}

  inline virtual void jetCalibSequence( const std::string& s ){if(!m_configFixed){m_jetCalibSequence = s;}}
  inline virtual const std::string& jetCalibSequence() const {return m_jetCalibSequence;}
  
  inline virtual void doJVTinMET( const bool& doJVT ){if(!m_configFixed){m_doJVTInMETCalculation = doJVT;}}
  inline virtual bool doJVTinMET() const {return m_doJVTInMETCalculation;}

  inline virtual float  JSF() const{return m_JSF;}
  inline virtual float bJSF() const{return m_bJSF;}

  // Tau configuration setters
  inline virtual void tauPtcut(const float pt) {
    if(!m_configFixed) {
      m_tau_configuration.pt = pt;
      m_tau_configuration_loose.pt = pt;
    }
  }
  inline virtual void tauJetIDWP(const std::string& s) {
    if(!m_configFixed) 
      m_tau_configuration.jetIDWP = s;
  }
  inline virtual void tauJetIDWPLoose(const std::string& s) {
    if(!m_configFixed)
      m_tau_configuration_loose.jetIDWP = s;
  }
  inline virtual void tauEleBDTWP(const std::string& s) {
    if(!m_configFixed) 
      m_tau_configuration.eleBDTWP = s;
  }
  inline virtual void tauEleBDTWPLoose(const std::string& s) {
    if(!m_configFixed)
      m_tau_configuration_loose.eleBDTWP = s;
  }
  inline virtual void tauEleOLR(bool do_tau_ele_olr) {
    if(!m_configFixed)
      m_tau_configuration.eleOLR = do_tau_ele_olr;
  }
  inline virtual void tauEleOLRLoose(bool do_tau_ele_olr) {
    if(!m_configFixed)
      m_tau_configuration_loose.eleOLR = do_tau_ele_olr;
  }
  inline virtual void tauJetConfigFile(const std::string& s) {
    if(!m_configFixed)
      m_tau_configuration.fileName = s;
  }
  inline virtual void tauJetConfigFileLoose(const std::string& s) {
    if(!m_configFixed)
      m_tau_configuration_loose.fileName = s;
  }

  // Tau configuration getters
  inline virtual float tauPtcut() const {
    return m_tau_configuration.pt;
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
  // Applying new tau energy calibration
  inline bool applyTauMVATES() {
    return true;
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
  inline const bool& photonUseRadiativeZ() {
    return m_photon_configuration.useRadiativeZ;
  }

  // inline const std::string& tauJetID() const {return m_tauJetID;}
  // inline const std::string& tauJetIDBkg() const {return m_tauJetIDBkg;}
  // inline const std::string& tauEVetoID() const {return m_tauEVetoID;}
  // inline virtual bool tauVetoLArCrack() const {return m_tauVetoLArCrack;}
  // inline virtual float tauPtcut() const {return m_tauPtcut;}

  // [[[-----------------------------------------------
  // Particle Level (Truth) configuration

  // electrons
  inline virtual void truth_electron_PtCut( const float pt ) { if ( ! m_configFixed ){ m_truth_electron.PtCut = pt; } }
  inline virtual void truth_electron_EtaCut( const float eta ) { if ( ! m_configFixed ){ m_truth_electron.EtaCut = eta; } }

  inline virtual float truth_electron_PtCut() const { return m_truth_electron.PtCut; }
  inline virtual float truth_electron_EtaCut() const { return m_truth_electron.EtaCut; }
  inline virtual bool truth_electron_NotFromHadron() const { return m_truth_electron.NotFromHadron; }
  inline virtual bool truth_electron_TauIsHadron() const { return m_truth_electron.TauIsHadron; }

  // muons
  inline virtual void truth_muon_PtCut( const float pt ) { if ( ! m_configFixed ){ m_truth_muon.PtCut = pt; } }
  inline virtual void truth_muon_EtaCut( const float eta ) { if ( ! m_configFixed ){ m_truth_muon.EtaCut = eta; } }

  inline virtual float truth_muon_PtCut() const { return m_truth_muon.PtCut; }
  inline virtual float truth_muon_EtaCut() const { return m_truth_muon.EtaCut; }
  inline virtual bool truth_muon_NotFromHadron() const { return m_truth_muon.NotFromHadron; }
  inline virtual bool truth_muon_TauIsHadron() const { return m_truth_muon.TauIsHadron; }

  // photons
  inline virtual void truth_photon_PtCut( const float pt ) { if ( ! m_configFixed ){ m_truth_photon.PtCut = pt; } }
  inline virtual void truth_photon_EtaCut( const float eta ) { if ( ! m_configFixed ){ m_truth_photon.EtaCut = eta; } }
  inline virtual void truth_photon_Origin(const std::string & value) { if ( ! m_configFixed ){ m_truth_photon.Origin = value; } }
  inline virtual void truth_photon_Isolation(const std::string & value) { if ( ! m_configFixed ){ m_truth_photon.Isolation = value; } }

  inline virtual float truth_photon_PtCut() const { return m_truth_photon.PtCut; }
  inline virtual float truth_photon_EtaCut() const { return m_truth_photon.EtaCut; }
  inline virtual const std::string & truth_photon_Origin() const { return m_truth_photon.Origin; }
  inline virtual const std::string & truth_photon_Isolation() const { return m_truth_photon.Isolation; }

  // jets
  inline virtual void truth_jet_PtCut( const float pt ) { if ( ! m_configFixed ){ m_truth_jet.PtCut = pt; } }
  inline virtual void truth_jet_EtaCut( const float eta ) { if ( ! m_configFixed ){ m_truth_jet.EtaCut = eta; } }

  inline virtual float truth_jet_PtCut() const { return m_truth_jet.PtCut; }
  inline virtual float truth_jet_EtaCut() const { return m_truth_jet.EtaCut; }

  // large R jets
  inline virtual void truth_jet_largeR_PtCut( const float pt ) { if ( ! m_configFixed ){ m_truth_largeRJet.PtCut = pt; } }
  inline virtual void truth_jet_largeR_EtaCut( const float eta ) { if ( ! m_configFixed ){ m_truth_largeRJet.EtaCut = eta; } }

  inline virtual float truth_jet_largeR_PtCut() const { return m_truth_largeRJet.PtCut; }
  inline virtual float truth_jet_largeR_EtaCut() const { return m_truth_largeRJet.EtaCut; }

  // -----------------------------------------------]]]

  /// HL LHC studies
  inline  virtual void HLLHC(const bool s) { if(!m_configFixed){m_HLLHC=s;} }
  inline  virtual bool HLLHC() const {return m_HLLHC;}
  inline  virtual void HLLHCFakes(const bool s) { if(!m_configFixed){m_HLLHCFakes=s;} }
  inline  virtual bool HLLHCFakes() const {return m_HLLHCFakes;}

  void setBTaggingSFSysts( std::string WP, const std::set<std::string>& btagging_SFs, bool isTrackJet=false );

  inline virtual std::set<std::string>  btagging_namedSysts(std::string WP) const { return bTag_named_systs.at(WP); }
  inline virtual unsigned int btagging_num_B_eigenvars(std::string WP) const { return bTag_eigen_B.at(WP); }
  inline virtual unsigned int btagging_num_C_eigenvars(std::string WP) const { return bTag_eigen_C.at(WP); }
  inline virtual unsigned int btagging_num_Light_eigenvars(std::string WP) const { return bTag_eigen_light.at(WP); }

  inline virtual std::set<std::string>  trkjet_btagging_namedSysts(std::string WP) const { return bTag_named_systs_trkJet.at(WP); }
  inline virtual unsigned int trkjet_btagging_num_B_eigenvars(std::string WP) const { return bTag_eigen_B_trkJet.at(WP); }
  inline virtual unsigned int trkjet_btagging_num_C_eigenvars(std::string WP) const { return bTag_eigen_C_trkJet.at(WP); }
  inline virtual unsigned int trkjet_btagging_num_Light_eigenvars(std::string WP) const { return bTag_eigen_light_trkJet.at(WP); }

  // B-tagging WPs requested by user (updated to pair of strings to hold algorithm and WP)
  const std::vector<std::pair<std::string, std::string> > bTagWP() const { return m_chosen_btaggingWP;}  
  // B-tagging systematics requested by user to be excluded from EV treatment, separated by semi-colons
  const std::string bTagSystsExcludedFromEV() const { return m_bTagSystsExcludedFromEV;}

  // B-tagging WPs actually available, according to CDI file
  // will be set in TopCPTools
  void setBTagWP_available( std::string btagging_WP );
  void setBTagWP_available_trkJet( std::string btagging_WP );
  const std::vector<std::string>& bTagWP_available() const { return m_available_btaggingWP;}
  const std::vector<std::string>& bTagWP_available_trkJet() const { return m_available_btaggingWP_trkJet;}
  // B-tagging WPs actually calibrated, according to CDI file
  // will be set in TopCPTools
  void setBTagWP_calibrated( std::string btagging_WP );
  void setBTagWP_calibrated_trkJet( std::string btagging_WP );
  const std::vector<std::string>& bTagWP_calibrated() const { return m_calibrated_btaggingWP;}
  const std::vector<std::string>& bTagWP_calibrated_trkJet() const { return m_calibrated_btaggingWP_trkJet;}

  std::string FormatedWP(std::string raw_WP);

  const std::string& bTaggingCalibration_B() const
  {return m_btagging_calibration_B;};
  const std::string& bTaggingCalibration_C() const
  {return m_btagging_calibration_C;};
  const std::string& bTaggingCalibration_Light() const
  {return m_btagging_calibration_Light;};

  // LHAPDF settings

  inline virtual std::vector<std::string> LHAPDFSets(){ return m_lhapdf_options.pdf_set_names; }
  inline virtual bool doLHAPDF(){ return m_lhapdf_options.doLHAPDF; }
  inline virtual std::string baseLHAPDF() { return m_lhapdf_options.baseLHAPDF; }
  inline virtual bool doLHAPDFInNominalTrees(){ return m_lhapdf_options.doLHAPDFInNominalTrees; }
  inline virtual bool saveLHAPDFEvent(){ return m_lhapdf_options.save_per_event; }
  void addLHAPDFResult( const std::string& pdf_name,
			std::vector<float> sum_of_weights );

  std::unordered_map<std::string, std::vector<float> >& getLHAPDFResults() { return m_lhapdf_options.pdf_results; };


  // Pileup Reweighting Settings
  bool doPileupReweighting(){ return m_pileup_reweighting.apply;};
  bool PileupUseGRLTool(){ return m_pileup_reweighting.use_grl_tool;};

  const std::vector<std::string>& PileupConfig(){ return m_pileup_reweighting.config_files; };
  const std::vector<std::string>& PileupLumiCalc(){ return m_pileup_reweighting.lumi_calc_files; };

  bool PileupMuDependent(){return m_pileup_reweighting.mu_dependent;};

  // Update for R21
  const std::vector<std::string>& PileupConfig_FS(){ return m_pileup_reweighting.config_files_FS; };
  const std::vector<std::string>& PileupConfig_AF(){ return m_pileup_reweighting.config_files_AF; };
  const std::vector<std::string>& PileupActualMu_FS(){ return m_pileup_reweighting.actual_mu_FS; };
  const std::vector<std::string>& PileupActualMu_AF(){ return m_pileup_reweighting.actual_mu_AF; };
  inline virtual  float PileupDataTolerance() const { return m_pileup_reweighting.unrepresented_data_tol; };
  
  const std::vector<double>& PileUpCustomScaleFactors(){ return m_pileup_reweighting.custom_SF; };

  inline const std::string& muonTriggerSF() const {return m_muon_trigger_SF;}

  // Where the sum of event weights
  // before derivation framework is kept
  inline const std::string& sumOfEventWeightsMetaData() const {return m_sumOfEventWeightsMetaData; }

  // Object Selector Name
  inline virtual void objectSelectionName(const std::string& s){if(!m_configFixed){m_objectSelectionName = s;}}
  inline virtual const std::string& objectSelectionName() const {return m_objectSelectionName;}
  // Output Format
  inline virtual void outputFormat(const std::string& s){if(!m_configFixed){m_outputFormat = s;}}
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
  inline virtual unsigned int numberOfEventsToRun() const { return m_numberOfEventsToRun;}
  inline virtual void numberOfEventsToRun(const unsigned int& n_events)       {if(!m_configFixed){m_numberOfEventsToRun = n_events;}}

  // Number of events to skip (only for testing)
  inline virtual unsigned int numberOfEventsToSkip() const { return m_numberOfEventsToSkip;}
  inline virtual void numberOfEventsToSkip(const unsigned int& n_events)      {if(!m_configFixed){m_numberOfEventsToSkip = n_events;}}

  // systematics
  inline virtual void systematics(const std::string& s) {if(!m_configFixed){m_systematics = s;}}
  inline virtual const std::string& systematics() const {return m_systematics;}

  /// syst helper functions
  bool isSystNominal(const std::string& syst) const;
  bool isSystAll(const std::string& syst) const;
  bool getSystematicsList(const std::string& syst, std::set<std::string>& systList) const;
  bool contains(const std::set<std::string>& systList, const std::string& name) const;

  virtual void systematicsPhotons    ( const std::list<CP::SystematicSet>& syst );
  virtual void systematicsElectrons  ( const std::list<CP::SystematicSet>& syst );
  virtual void systematicsMuons      ( const std::list<CP::SystematicSet>& syst );
  virtual void systematicsTaus       ( const std::list<CP::SystematicSet>& syst );
  virtual void systematicsJets       ( const std::list<CP::SystematicSet>& syst );
  virtual void systematicsLargeRJets ( const std::list<CP::SystematicSet>& syst );
  virtual void systematicsTrackJets  ( const std::list<CP::SystematicSet>& syst );
  virtual void systematicsMET        ( const std::list<CP::SystematicSet>& syst );

  virtual void systematicsJetGhostTrack ( const std::list<CP::SystematicSet>& syst );

  inline virtual void jetSubstructureName(const std::string &s)  {if(!m_configFixed){ m_jetSubstructureName = s;} }
  inline virtual const std::string& jetSubstructureName() const {return m_jetSubstructureName;}

  // Systematic hash values
  inline std::size_t nominalHashValue() const {return m_nominalHashValue;}
  inline std::shared_ptr<std::unordered_set<std::size_t>> systHashPhotons()   const {return m_systHashPhotons;  }
  inline std::shared_ptr<std::unordered_set<std::size_t>> systHashElectrons() const {return m_systHashElectrons;}
  inline std::shared_ptr<std::unordered_set<std::size_t>> systHashMuons()     const {return m_systHashMuons;    }
  inline std::shared_ptr<std::unordered_set<std::size_t>> systHashTaus()      const {return m_systHashTaus;     }
  inline std::shared_ptr<std::unordered_set<std::size_t>> systHashJets()      const {return m_systHashJets;     }
  inline std::shared_ptr<std::unordered_set<std::size_t>> systHashAll()       const {return m_systHashAll;      }
  inline std::shared_ptr<std::unordered_set<std::size_t>> systHashElectronInJetSubtraction() const {return m_systHash_electronInJetSubtraction;}

  // Systematic Maps
  inline std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> systMapPhotons()   const {return m_systMapPhotons;  }
  inline std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> systMapElectrons() const {return m_systMapElectrons;}
  inline std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> systMapMuons()     const {return m_systMapMuons;    }
  inline std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> systMapTaus()      const {return m_systMapTaus;     }
  inline std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> systMapJets()      const {return m_systMapJets;     }
  inline std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> systMapLargeRJets()      const {return m_systMapLargeRJets;     }

  // Systematic StoreGate key Maps
  inline std::shared_ptr<std::unordered_map<std::size_t,std::string>> systSgKeyMapPhotons()    const {return m_systSgKeyMapPhotons;   }
  inline std::shared_ptr<std::unordered_map<std::size_t,std::string>> systSgKeyMapElectrons()  const {return m_systSgKeyMapElectrons; }
  inline std::shared_ptr<std::unordered_map<std::size_t,std::string>> systSgKeyMapMuons()      const {return m_systSgKeyMapMuons;     }
  inline std::shared_ptr<std::unordered_map<std::size_t,std::string>> systSgKeyMapTaus()       const {return m_systSgKeyMapTaus;      }
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> systSgKeyMapJets(const bool useLooseLeptonJets) const;
  inline std::shared_ptr<std::unordered_map<std::size_t,std::string>> systSgKeyMapLargeRJets() const {return m_systSgKeyMapLargeRJets;}
  inline std::shared_ptr<std::unordered_map<std::size_t,std::string>> systSgKeyMapTrackJets()  const {return m_systSgKeyMapTrackJets;}

  // TTree names
  inline std::shared_ptr<std::unordered_map<std::size_t,std::string>> systAllTTreeNames() const {return m_systAllTTreeNames;}
  const std::string& systematicName( const std::size_t hash ) const;
  // TTree index
  unsigned int ttreeIndex( const std::size_t hash ) const;
  unsigned int ttreeIndexLoose( const std::size_t hash ) const;

  AodMetaDataAccess & aodMetaData();
  AodMetaDataAccess const & aodMetaData() const { return *m_aodMetaData; }

  // Function to handle release series such that it can be cleaner to update in the future
  void setReleaseSeries();
  inline int getReleaseSeries() const { return m_release_series; }

  // Create bootstrapping weights
  inline bool saveBootstrapWeights() const { return m_saveBootstrapWeights; }
  inline void setSaveBootstrapWeights(const bool value) { m_saveBootstrapWeights = value; }  
  inline int getNumberOfBootstrapReplicas() const { return m_BootstrapReplicas; }
  inline void setNumberOfBootstrapReplicas(const int value) { m_BootstrapReplicas = value; }

  // Switch to use event-level jet cleaning tool for studies
  inline bool useEventLevelJetCleaningTool() const { return m_useEventLevelJetCleaningTool; }
  inline void setUseEventLevelJetCleaningTool(const bool value) { m_useEventLevelJetCleaningTool = value; }
  
  // Just a function that might need to be used in multiple places - return the running year (2015, 2016, 2017)
  const std::string getYear(unsigned int runnumber);

  // Setter and getter functions for recording whether we have configured the nominal objects
  inline virtual void setNominalAvailable(const bool s){m_isNominalAvailable = s;}
  inline bool isNominalAvailable() const { return m_isNominalAvailable;}

  // Function to set the options for global trigger tool
  void setGlobalTriggerConfiguration(std::vector<std::string>, std::vector<std::string>, std::vector<std::string>, std::vector<std::string>);
  inline bool useGlobalTrigger() const { return m_trigGlobalConfiguration.isActivated; } // Was this requested by the user
  inline auto const & getGlobalTriggerElectronTriggers() const { return m_trigGlobalConfiguration.electron_trigger; }
  inline auto const & getGlobalTriggerElectronTriggersLoose() const { return m_trigGlobalConfiguration.electron_trigger_loose; }
  inline auto const & getGlobalTriggerMuonTriggers() const { return m_trigGlobalConfiguration.muon_trigger; }
  inline auto const & getGlobalTriggerMuonTriggersLoose() const { return m_trigGlobalConfiguration.muon_trigger_loose; }
  inline bool useGlobalTriggerConfiguration() const { return m_trigGlobalConfiguration.isConfigured; } // Was this subsequently configured
  inline std::vector<std::string> getGlobalTriggerElectronSystematics() const { return m_trigGlobalConfiguration.electron_trigger_systematics; }
  inline std::vector<std::string> getGlobalTriggerMuonSystematics()     const { return m_trigGlobalConfiguration.muon_trigger_systematics; }
  inline std::vector<std::string> getGlobalTriggerElectronTools()       const { return m_trigGlobalConfiguration.electron_trigger_tool_names; }
  inline std::vector<std::string> getGlobalTriggerMuonTools()           const { return m_trigGlobalConfiguration.muon_trigger_tool_names; }




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
  bool m_useMuons;
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
  bool m_useTruthMET;

  // define if d0/z0 cut should be used at all
  bool m_applyTTVACut;

  std::string m_jetSubstructureName;

  // Store in config a boolean which lets us know if we called the nominal object setup
  bool m_isNominalAvailable;
    
  // Do systematics? - this needs many more configuration options
  std::string m_systematics;
  std::string m_nominalSystName;
  std::string m_allSystName;

  unsigned int m_DSID;
  unsigned int m_MapIndex;
  bool m_is_sherpa_22_vjets = false;

  bool m_isMC;
  bool m_isAFII;
  std::string m_generators;
  std::string m_AMITag;
  bool m_isPrimaryxAOD;
  bool m_isTruthDxAOD = false;
  std::string m_derivationStream;
  std::string m_amiTag;
  int m_amiTagSet = 0;

  // Do fakes MM weights calculation? - only for data loose
  bool m_doFakesMMWeights;
  // Directory of efficiency files for MM fake estimate
  std::string m_FakesMMDir;
  // Debug mode?
  bool m_doFakesMMDebug;

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

  // Top Parton History
  bool m_doTopPartonHistory;
  bool m_isTopPartonHistoryRegisteredInNtuple;

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
  std::string m_sgKeyMuons;
  std::string m_sgKeyTaus;
  std::string m_sgKeyJets;
  std::string m_sgKeyLargeRJets;
  std::string m_sgKeyTrackJets;
  std::string m_sgKeyMissingEt;
  std::string m_sgKeyMissingEtLoose;
  std::string m_sgKeyInDetTrackParticles;

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

  // special: allow to dump the systematics-shifted b-tagging SFs in the systematics trees
  bool m_dumpBtagSystsInSystTrees;
  
  // Electron configuration
  std::string m_egammaSystematicModel;
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

  // Muon configuration
  float m_muonPtcut; // muon object selection pT cut
  float m_muonEtacut; // muon object selection (abs) eta cut
  std::string m_muonQuality; // muon quality used in object selection
  std::string m_muonQualityLoose; // loose muon quality used in object selection
  std::string m_muonIsolation;
  std::string m_muonIsolationLoose;
  std::string m_muonIsolationSF;
  std::string m_muonIsolationSFLoose;
  int   m_muon_d0SigCut;
  float m_muon_delta_z0;


  // Jet configuration
  float m_jetPtcut; // jet object selection pT cut
  float m_jetEtacut; // jet object selection (abs) eta cut
  std::string m_fwdJetAndMET; // type of treatment of forward jets, including for MET calculation
  std::string m_jetUncertainties_BunchSpacing; // 25ns or 50ns
  std::string m_jetUncertainties_NPModel; // AllNuisanceParameters, 19NP or 3NP
  std::string m_jetUncertainties_QGFracFile; // to improve Flavour composition and response
  std::vector<std::string> m_jetUncertainties_QGHistPatterns; // to improve Flavour composition and response, with more flexibility
  bool m_doMultipleJES;
  bool m_largeRSmallRCorrelations = false; // Add correlations of large/small R jets
  std::string m_jetJERSmearingModel; // Full or Simple
  std::string m_jetCalibSequence; // GCC or JMS
  bool m_doJVTInMETCalculation;

  // Large R jet configuration
  float m_largeRJetPtcut; // large R jet object selection pT cut
  float m_largeRJetEtacut; // large R jet object selection (abs) eta cut
  std::string m_largeRJESUncertaintyConfig; // large R jet JES uncertainty configuration file
  // see: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetUncertainties2015PrerecLargeR
  std::string m_largeRJESJMSConfig; // large R jet JES/JMS calibration choice - see ANALYSISTO-210
  std::string m_largeRtoptaggingConfigFile;

  // Track jet configuration
  float m_trackJetPtcut; // track jet object selection pT cut
  float m_trackJetEtacut; // track jet object selection (abs) eta cut

  // Jet configuration for reclustered jets
  float m_RCJetPtcut;
  float m_RCJetEtacut;
  float m_RCJetTrimcut;
  float m_RCJetRadius;
  bool  m_useRCJetSubstructure;
  bool  m_useRCJetAdditionalSubstructure;
  
  // Jet configuration for variable large-R jets
  float m_VarRCJetPtcut;
  float m_VarRCJetEtacut;
  float m_VarRCJetTrimcut;
  float m_VarRCJetMaxRadius;
  std::string m_VarRCJetRho;
  std::string m_VarRCJetMassScale;
  bool  m_useVarRCJetSubstructure;
  bool  m_useVarRCJetAdditionalSubstructure;

  // these are needed for the top mass analysis, per default should be 1.0
  float m_JSF;
  float m_bJSF;

  // Tau configuration
  struct {
    // filename to load tau configuration from.
    // If this is an empty string then take the 'default'
    std::string fileName = "Default";
    // The jetIDWP in CamelCase
    // see https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauSelectionTool.rst
    // for supported WPs
    std::string jetIDWP = "Medium";
    // the electron BDTWP
    std::string eleBDTWP = "Medium";
    // Whether to perform electron overlap removal
    bool eleOLR = false;
    // pT cut on taus
    float pt = 20000;
  } m_tau_configuration, m_tau_configuration_loose;

  // photon configuration
  struct {
    // pT cut on photons
    float pt = 25000;
    float eta = 2.5;
    std::string isolation = "None";
    std::string identification = "None";
    bool useRadiativeZ = false;
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

  // B-tagging WPs requested by the user (updated to pair of string to hold algorithm and WP)
  std::vector<std::pair<std::string, std::string> > m_chosen_btaggingWP; // = { };
  // B-tagging systematics requested by user to be excluded from EV treatment, separated by semi-colons
  std::string m_bTagSystsExcludedFromEV = "";
  
  // list of B-tagging WP actualy available
  std::vector<std::string> m_available_btaggingWP;
  std::vector<std::string> m_available_btaggingWP_trkJet;
  // list of B-tagging WP actualy calibrated
  std::vector<std::string> m_calibrated_btaggingWP;
  std::vector<std::string> m_calibrated_btaggingWP_trkJet;

  // B-tagging calibration to be used
  std::string m_btagging_calibration_B = "default";
  std::string m_btagging_calibration_C = "default";
  std::string m_btagging_calibration_Light = "default";

  // b-tag SF helpers - one of each per WP
  // will be set in the BTagScaleFactorCalculator
  // These are the base names of the SF systematics (without __up/__down)
  std::unordered_map<std::string,std::set<std::string>> bTag_base_names;
  std::unordered_map<std::string,std::set<std::string>> bTag_base_names_trkJet;
  // and those of them which are not eigenvectors
  std::unordered_map<std::string,std::set<std::string>> bTag_named_systs;
  std::unordered_map<std::string,std::set<std::string>> bTag_named_systs_trkJet;
  // how many B eigen-variations do we have?
  std::unordered_map<std::string,unsigned int> bTag_eigen_B;
  std::unordered_map<std::string,unsigned int> bTag_eigen_B_trkJet;
  // how many C eigen-variations do we have?
  std::unordered_map<std::string,unsigned int> bTag_eigen_C;
  std::unordered_map<std::string,unsigned int> bTag_eigen_C_trkJet;
  // how many Light eigen-variations do we have?
  std::unordered_map<std::string,unsigned int> bTag_eigen_light;
  std::unordered_map<std::string,unsigned int> bTag_eigen_light_trkJet;

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

  } m_pileup_reweighting;

  // Struct for holding TrigGlobalEfficiencyCorrectionTool settings in order to 
  // manage systematic variations through this tool

  struct{
    typedef std::unordered_map<std::string, std::vector<std::string>> triggermap_t;
    // -- Set from cutfile --//
    // Boolean to be set to true if the user activates a flag
    bool isActivated  = false;
    // Maps of periods -> list of triggers
    triggermap_t electron_trigger;
    triggermap_t electron_trigger_loose;
    triggermap_t muon_trigger;
    triggermap_t muon_trigger_loose;

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
  std::shared_ptr<std::vector<std::string>> m_allSelectionNames;
  // Trigger configuration
  // First string is the selection name, second string is the trigger
  std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> m_allTriggers_Tight;
  std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> m_electronTriggers_Tight;
  std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> m_muonTriggers_Tight;
  std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> m_tauTriggers_Tight;
  std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> m_allTriggers_Loose;
  std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> m_electronTriggers_Loose;
  std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> m_muonTriggers_Loose;
  std::shared_ptr<std::unordered_map<std::string,std::vector<std::string>>> m_tauTriggers_Loose;
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
  AodMetaDataAccess * m_aodMetaData;

  // Systematics
  std::size_t m_nominalHashValue;

  std::shared_ptr<std::unordered_set<std::size_t>> m_systHashPhotons;
  std::shared_ptr<std::unordered_set<std::size_t>> m_systHashElectrons;
  std::shared_ptr<std::unordered_set<std::size_t>> m_systHashMuons;
  std::shared_ptr<std::unordered_set<std::size_t>> m_systHashTaus;
  std::shared_ptr<std::unordered_set<std::size_t>> m_systHashJets;
  std::shared_ptr<std::unordered_set<std::size_t>> m_systHashLargeRJets;
  std::shared_ptr<std::unordered_set<std::size_t>> m_systHashTrackJets;
  std::shared_ptr<std::unordered_set<std::size_t>> m_systHashMET;

  std::shared_ptr<std::unordered_set<std::size_t>> m_systHashAll;
  std::shared_ptr<std::list<std::size_t>> m_list_systHashAll;

  std::shared_ptr<std::unordered_set<std::size_t>> m_systHash_electronInJetSubtraction;
  std::shared_ptr<std::list<std::size_t>> m_list_systHash_electronInJetSubtraction;

  std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> m_systMapPhotons;
  std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> m_systMapElectrons;
  std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> m_systMapMuons;
  std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> m_systMapTaus;
  std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> m_systMapJets;
  std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> m_systMapLargeRJets;
  std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> m_systMapTrackJets;
  std::shared_ptr<std::unordered_map<std::size_t,CP::SystematicSet>> m_systMapMET;

  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapPhotons;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapElectrons;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapMuons;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapTaus;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapJets;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapJets_electronInJetSubtraction;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapJetsLoose_electronInJetSubtraction;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapLargeRJets;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapTrackJets;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapMET;

  // For TopEvent/SingleSystEvent - will return the nominal key if not under variation
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapPhotons;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapElectrons;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapMuons;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapTaus;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapJets;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapLargeRJets;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapTrackJets;
  // The boosted case is a bit more complex, we need additional collections
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapElectrons_electronInJetSubtraction;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapJets_electronInJetSubtraction;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapJetsLoose_electronInJetSubtraction;

  // For top::TopEventMaker
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapPhotonsTDS;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapPhotonsTDSAux;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapElectronsTDS;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapElectronsTDSAux;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapMuonsTDS;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapMuonsTDSAux;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapTausTDS;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapTausTDSAux;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapJetsTDS;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapJetsTDSAux;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapLargeRJetsTDS;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapLargeRJetsTDSAux;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapTrackJetsTDS;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapTrackJetsTDSAux;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapElectrons_electronInJetSubtractionTDS;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapElectrons_electronInJetSubtractionTDSAux;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapJets_electronInJetSubtractionTDS;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapJets_electronInJetSubtractionTDSAux;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDS;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDSAux;

  // Missing ET
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapMissingET;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapMissingETLoose;

  // KLFitter
  std::string m_sgKeyKLFitter;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapKLFitter;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapKLFitterLoose;

  // PseudoTop
  std::string m_sgKeyPseudoTop;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapPseudoTop;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systSgKeyMapPseudoTopLoose;

  // Map from systematic hash to CP::SystematicSet
  std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > m_systMapJetGhostTrack;
  // Map from systematic hash to decoration key.
  std::shared_ptr<std::unordered_map<std::size_t, std::string> > m_systDecoKeyMapJetGhostTrack;

  // Output TTree names
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systAllTTreeNames;
  std::shared_ptr<std::unordered_map<std::size_t,std::string>> m_systPersistantAllTTreeNames;
  // Output TTree index
  std::shared_ptr<std::unordered_map<std::size_t,unsigned int>> m_systAllTTreeIndex;
  std::shared_ptr<std::unordered_map<std::size_t,unsigned int>> m_systAllTTreeLooseIndex;

  // Private function only to simplify the setting of AFII values
  void ReadIsAFII(top::ConfigurationSettings* const& settings);

  // Int holding the release series value
  int m_release_series;

  // Bool to hold whether we generate and store poisson bootstrap weights
  bool m_saveBootstrapWeights;
  int  m_BootstrapReplicas;
  
  // Switch to use event-level jet cleaning tool for testing
  bool m_useEventLevelJetCleaningTool;


};
}  // namespace top

std::ostream& operator<<(std::ostream& os, const top::TopConfig& config);

#endif
