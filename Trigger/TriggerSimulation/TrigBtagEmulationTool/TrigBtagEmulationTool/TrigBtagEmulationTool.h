/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
*/

#ifndef TrigBtagEmulationTool_H
#define TrigBtagEmulationTool_H

#include "TrigBtagEmulationTool/ITrigBtagEmulationTool.h"
#include "TrigBtagEmulationTool/feature.h"
#include "TrigBtagEmulationTool/baseTrigBtagEmulationChainJetIngredient.h"
#include "TrigBtagEmulationTool/jetManager.h"

// EDM
#include "xAODEventInfo/EventInfo.h"

#include "xAODTrigger/JetRoIContainer.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAttributes.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"

#include "ParticleJetTools/JetFlavourInfo.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"

// Infrastructural includes
#include "AsgTools/AsgTool.h"
#include "PATCore/TAccept.h"
#include "AthContainers/AuxElement.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <map>

#include "AsgTools/AsgTool.h"

#ifndef XAOD_STANDALONE
// Offline tools
#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/BTagSecVertexing.h"
#include "BTagging/BTagTool.h"

#include "StoreGate/StoreGateSvc.h"
#else
#include "TrigConfxAOD/xAODConfigTool.h"
#endif


namespace Trig {
  
  class TrigBtagEmulationChain {
    
  public:

    /// Constructor with signature name 
    TrigBtagEmulationChain(const std::vector<std::string>& chainDefinition, ToolHandle<Trig::TrigDecisionTool>& trigDec);    

    /// Trigger decision ingredient definition
    void addDecisionIngredient(std::string decision);
    
    /// L1 Jet ingredient definition
    void addL1JetIngredient(std::string triggerName,float min_pt, float min_eta, float max_eta, unsigned int min_mult, float min_ht, unsigned int topEt,float min_invm,bool isCF = false) ;

    /// L1 Jet ingredient definition
    void addL1JJJetIngredient(std::string triggerName,float min_pt, float min_eta, float max_eta, unsigned int min_mult, float min_ht, unsigned int topEt,float min_invm,bool isCF = false);

    /// HLT Jet ingredient definition
    void addHLTJetIngredient(std::string triggerName,float min_pt, float min_eta, float max_eta, unsigned int min_mult, float min_ht, float min_gsc, float min_invm);

    /// HLT Bjet ingredient definition
    void addHLTBjetIngredient(std::string triggerName,float min_pt, float min_eta, float max_eta,std::string tagger, float min_weight, unsigned int min_mult, float min_ht, float min_gsc, float anti_weight, float min_invm);

    /// Jet Evaluation
    void evaluate();

    /// Chain evaluation
    bool isPassed();

    // Dump
    void Print();

    // Utilities
    bool hasFeature(std::string feature);

    /// Event cleanup
    void clear();
  
    /// Name
    std::string name();

    /// Configuration accessors
    bool isSplit();
    bool isIP3DSV1();
    bool isCOMB();
    bool isMV2c20();
    std::string tagger();
    
    bool addJet(std::string item,std::vector< struct TrigBtagEmulationJet >& jets);

  private:

    // Chain name
    std::string m_name;

    // Chain selections
    std::vector<std::string> m_ingredientsDecision;
    std::vector<TrigBtagEmulationChainJetIngredient_L1*>  m_ingredientsL1Jet;
    std::vector<TrigBtagEmulationChainJetIngredient_HLT*> m_ingredientsHLTJet;

    // Trigger decision
    ToolHandle<Trig::TrigDecisionTool>& m_trigDec;


    // Chain age
    bool m_old;

    // Jet cuts
    unsigned int m_jetMultCut;
    double m_jetPtCut;

    // Bjet cuts
    unsigned int m_bjetMultCut;
    double m_bjetPtCut;
    std::string m_bjetConfig;
    std::string m_bjetTagger;
    double m_bjetTaggerCut;

    // Chain multiplicity result
    unsigned int m_jetCount;
    unsigned int m_bjetCount;

  };


  class TrigBtagEmulationTool : public asg::AsgTool, virtual public Trig::ITrigBtagEmulationTool {
    ASG_TOOL_CLASS(TrigBtagEmulationTool, Trig::ITrigBtagEmulationTool)
      
      public:
    
    //****************************************************************************** 
    TrigBtagEmulationTool(const std::string& name);
    ~TrigBtagEmulationTool() {};
    
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    
    void addEmulatedChain(const std::vector<std::string>&);
    std::vector<std::string> addEmulatedChain(const std::string);
    bool isPassed(const std::string&);

  private:

    void clear();
    StatusCode initTriggerChainsMenu();

    StatusCode getInputContainerSG(std::vector<const xAOD::Jet*>& jetContainers,
                                   std::string& inputItem, std::string& jetName);

    // *** Attributes ** //
  private:
    std::map< std::string,std::string > m_2015Menu;
    std::map< std::string,std::string > m_2016Menu;
    std::map< std::string,std::string > m_2017Menu;

    // SERVICES
    ToolHandle<Trig::TrigDecisionTool> m_trigDec;

#ifndef XAOD_STANDALONE
    ServiceHandle<StoreGateSvc> m_storeGate;
    ServiceHandle< TrigConf::ITrigConfigSvc > m_configSvc; 
    ServiceHandle< TrigConf::ITrigConfigSvc > m_dsSvc;


    // OFFLINE TOOLS
    ToolHandle< Analysis::IBTagTool > m_bTagTool;
    ToolHandle< Analysis::IBTagTrackAssociation > m_bTagTrackAssocTool;
    ToolHandle< Analysis::IBTagSecVertexing > m_bTagSecVtxTool;
#else
    //    TrigConf::xAODConfigTool *m_trigConfigTool; 

    ToolHandle< TrigConf::ITrigConfigSvc > m_configSvc;
    ToolHandle< TrigConf::ITrigConfigSvc > m_dsSvc;
#endif  

#ifdef XAOD_STANDALONE
    std::string m_TrigDecToolName; //!
    std::string m_xAODConfToolName; //!
#endif


    // INPUT PROPERTIES
    bool m_useTriggerNavigation;
    bool m_tagOfflineWeights;
    bool m_tagOnlineWeights;
    std::string m_input_chain;
    std::string m_input_chainSplit;
    std::string m_input_chainGSC;
    std::string m_input_pvKey;
    std::string m_input_jetKey;
    std::string m_input_tpKey;
    std::string m_input_pvKeySplit;
    std::string m_input_jetKeySplit;
    std::string m_input_tpKeySplit;
    std::string m_input_btagName;
    std::string m_input_jetName;
    std::string m_input_jetNameSplit;
    std::string m_input_JetName_GSC;

    std::string m_input_pvKey_GSC;
    std::string m_input_jetKey_GSC;
    std::string m_input_tpKey_GSC;

    std::string m_trigger_menu;
    std::string m_autoconfiguredMenu;

    int m_verbosity;
    long long int m_previousEvent;

    // jet Managers
    Trig::jetManager *m_manager_ef;
    Trig::jetManager *m_manager_split;
    Trig::jetManager *m_manager_gsc;
    Trig::jetManager *m_manager_ef_gsc;
    Trig::jetManager *m_manager_split_gsc;

    // OUTPUT PROPERTIES
    std::vector< std::vector< std::string > > m_emulatedChainDefinitions;

    // EMULATED CHAINS
    std::map< std::string, TrigBtagEmulationChain > m_emulatedChains;

    // to-be-emulated chains
    bool checkTriggerChain(const std::vector<std::string>&);

    bool m_splitTrigger;
    bool m_btagTrigger;
    bool m_htTrigger;
    bool m_gscTrigger;

    bool hasSplit();
    bool hasBtag();
    bool hasHT();
    bool hasGSC();

  };

}//namespace

#endif
