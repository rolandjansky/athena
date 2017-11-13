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


// Offline tools
#ifdef XAOD_STANDALONE
#include "TrigConfxAOD/xAODConfigTool.h"

#elif defined( XAOD_ANALYSIS )
#include "StoreGate/StoreGateSvc.h"

#else
#include "StoreGate/StoreGateSvc.h"
#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/BTagSecVertexing.h"
#include "BTagging/BTagTool.h"
#endif


namespace Trig {
  
  class TrigBtagEmulationChain {    
  public:

    /// Constructor with signature name 
    TrigBtagEmulationChain(const std::vector<std::string>& chainDefinition, ToolHandle<Trig::TrigDecisionTool>& trigDec);    

    /// Trigger decision ingredient definition
    void addDecisionIngredient(std::string decision);
    void addDecisionIngredient(BaseTrigBtagEmulationChainJetIngredient* decision);

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
    std::string getName() const;
    // Method for accessing configuration outcome
    bool isCorrectlyConfigured() const;
    bool isAutoConfigured() const;
    std::vector< std::string > retrieveAutoConfiguration() const;

    bool addJet(std::string item,std::vector< struct TrigBtagEmulationJet >& jets);

  protected:
    bool parseChainName( std::string,std::vector< BaseTrigBtagEmulationChainJetIngredient* >& );
    std::vector< BaseTrigBtagEmulationChainJetIngredient* > processL1trigger (std::string);
    std::vector< BaseTrigBtagEmulationChainJetIngredient* > processHLTtrigger (std::string);

  private:
    // Chain name
    std::string m_name;

    // Chain selections
    std::vector< std::string > m_ingredientsDecision;
    std::vector< BaseTrigBtagEmulationChainJetIngredient* >  m_ingredientsJet;

    // Trigger decision
    ToolHandle<Trig::TrigDecisionTool>& m_trigDec;

    bool m_correctlyConfigured;
    bool m_autoConfigured;
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
    
    StatusCode addEmulatedChain(const std::vector<std::string>&);
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

#ifdef XAOD_STANDALONE
    std::string m_TrigDecToolName; //! 
    std::string m_xAODConfToolName; //!

#elif defined( XAOD_ANALYSIS )
    ServiceHandle<StoreGateSvc> m_storeGate;

#else
    ServiceHandle<StoreGateSvc> m_storeGate;

    ToolHandle< Analysis::IBTagTool > m_bTagTool;
    ToolHandle< Analysis::IBTagTrackAssociation > m_bTagTrackAssocTool;
    ToolHandle< Analysis::IBTagSecVertexing > m_bTagSecVtxTool;
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
    Trig::JetManager *m_manager_ef;
    Trig::JetManager *m_manager_split;
    Trig::JetManager *m_manager_gsc;
    Trig::JetManager *m_manager_ef_gsc;
    Trig::JetManager *m_manager_split_gsc;

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

} //namespace

#endif
