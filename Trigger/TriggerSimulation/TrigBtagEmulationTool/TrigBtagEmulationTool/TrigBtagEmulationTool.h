/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */

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

#ifndef ROOTCORE
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
    void addDecisionIngredient(std::string decision) {
      m_ingredientsDecision.push_back(decision);
    }
    
    /// L1 Jet ingredient definition
    void addL1JetIngredient(std::string triggerName,float min_pt, float min_eta, float max_eta, unsigned int min_mult, float min_ht, unsigned int topEt,float min_invm,bool isCF = false) {
      TrigBtagEmulationChainJetIngredient_L1 *ing = new TrigBtagEmulationChainJetIngredient_L1(triggerName,min_pt,min_eta,max_eta,min_mult);

      feature_ht *ht_feat;
      if (topEt == 0) ht_feat = new feature_ht("L1","HT",min_ht);
      else ht_feat = new feature_ht_top("L1","HT",min_ht,topEt);
      ht_feat->setCuts(min_pt,min_eta,max_eta);

      feature_invm *invm_feat;
      if (!isCF) invm_feat = new feature_invm("L1","MJJ",min_invm);
      else invm_feat = new feature_invm_CF("L1","MJJ",min_invm);
      invm_feat->setCuts(min_pt,min_eta,max_eta);
      if (min_invm != 0) ing->addFeature( "MJJ",invm_feat );

      if (min_ht != 0) ing->addFeature( "HT", ht_feat );
      m_ingredientsL1Jet.push_back(ing);
    }

    /// L1 Jet ingredient definition
    void addL1JJJetIngredient(std::string triggerName,float min_pt, float min_eta, float max_eta, unsigned int min_mult, float min_ht, unsigned int topEt,float min_invm,bool isCF = false) {
      TrigBtagEmulationChainJetIngredient_L1_JJ *ing = new TrigBtagEmulationChainJetIngredient_L1_JJ(triggerName,min_pt,min_eta,max_eta,min_mult);

      feature_ht *ht_feat;
      if (topEt == 0) ht_feat = new feature_ht("L1","HT",min_ht);
      else ht_feat = new feature_ht_top("L1","HT",min_ht,topEt);
      ht_feat->setCuts(min_pt,min_eta,max_eta);

      feature_invm *invm_feat;
      if (!isCF) invm_feat = new feature_invm("L1","MJJ",min_invm);
      else invm_feat = new feature_invm_CF("L1","MJJ",min_invm);
      invm_feat->setCuts(min_pt,min_eta,max_eta);
      if (min_invm != 0) ing->addFeature( "MJJ",invm_feat );

      if (min_ht != 0) ing->addFeature( "HT", ht_feat );
      m_ingredientsL1Jet.push_back(ing);
    }

    /// HLT Jet ingredient definition
    void addHLTJetIngredient(std::string triggerName,float min_pt, float min_eta, float max_eta, unsigned int min_mult, float min_ht, float min_gsc, float min_invm) {
      TrigBtagEmulationChainJetIngredient_HLT *ing = nullptr;
      if (min_gsc == 0) ing = new TrigBtagEmulationChainJetIngredient_HLT(triggerName,min_pt,min_eta,max_eta,min_mult);
      else ing = new TrigBtagEmulationChainJetIngredient_GSC(triggerName,min_pt,min_gsc,min_eta,max_eta,min_mult);
      
      feature_ht *ht_feat = new feature_ht("HLT","HT",min_ht);
      ht_feat->setCuts(min_pt,min_eta,max_eta);
      if (min_ht != 0) ing->addFeature( "HT", ht_feat);

      feature_invm *invm_feat = new feature_invm("HLT","INVM",min_invm);
      invm_feat->setCuts(min_pt,min_eta,max_eta);
      if (min_invm != 0) ing->addFeature( "INVM",invm_feat );

      m_ingredientsHLTJet.push_back(ing);
    }

    /// HLT Bjet ingredient definition
    void addHLTBjetIngredient(std::string triggerName,float min_pt, float min_eta, float max_eta,std::string tagger, float min_weight, unsigned int min_mult, float min_ht, float min_gsc, float anti_weight, float min_invm) {
      TrigBtagEmulationChainJetIngredient_HLT *ing = nullptr;
      if (min_gsc == 0) ing = new TrigBtagEmulationChainJetIngredient_HLT(triggerName,min_pt,min_eta,max_eta,min_mult);
      else ing = new TrigBtagEmulationChainJetIngredient_GSC(triggerName,min_pt,min_gsc,min_eta,max_eta,min_mult);

      if (min_weight != -1000) ing->addFeature("BTAG",new feature_btag( tagger.c_str(),min_weight) );
      if (anti_weight != 1000) ing->addFeature("ANTI-BTAG",new feature_antibtag( tagger.c_str(),anti_weight) );

      feature_ht *ht_feat = new feature_ht("HLT","HT",min_ht);
      ht_feat->setCuts(min_pt,min_eta,max_eta); 
      if (min_ht != 0) ing->addFeature( "HT", ht_feat);

      feature_invm *invm_feat = new feature_invm("HLT","INVM",min_invm);
      invm_feat->setCuts(min_pt,min_eta,max_eta);
      if (min_invm != 0) ing->addFeature( "INVM",invm_feat );

      m_ingredientsHLTJet.push_back(ing);
    }

    /// Jet Evaluation
    void evaluate()
    {
      for (unsigned int index(0); index<m_ingredientsL1Jet.size(); index++)
	m_ingredientsL1Jet.at(index)->evaluate();

      for (unsigned int index(0); index<m_ingredientsHLTJet.size(); index++)
	m_ingredientsHLTJet.at(index)->evaluate();
    }

    /// Chain evaluation
    bool isPassed() {
      std::vector<std::string>::iterator dec, decEnd=m_ingredientsDecision.end(); 
      for(dec=m_ingredientsDecision.begin(); dec!=decEnd; dec++) 
	if(!m_trigDec->isPassed(*dec)) return false;   

      for (unsigned int index(0); index<m_ingredientsL1Jet.size(); index++)
        if ( !m_ingredientsL1Jet.at(index)->isPassed() ) return false;

      for (unsigned int index(0); index<m_ingredientsHLTJet.size(); index++)
        if ( !m_ingredientsHLTJet.at(index)->isPassed() ) return false;

      return true;
    }

    // Dump
    void Print()
    {
      std::cout<<std::endl<<"### TRIGGER EMULATION ###"<<std::endl;
      std::cout<<"###     L1"<<std::endl;
      for (unsigned int i=0; i<m_ingredientsL1Jet.size();i++)
        m_ingredientsL1Jet.at(i)->Print();
      std::cout<<"###     HLT"<<std::endl;
      for (unsigned int i=0; i<m_ingredientsHLTJet.size();i++)
        m_ingredientsHLTJet.at(i)->Print();
    }

    // Utilities
    bool hasFeature(std::string feature)
    {
      for (unsigned int index(0); index<m_ingredientsL1Jet.size(); index++)
        if ( m_ingredientsL1Jet.at(index)->hasFeature(feature) ) return true;

      for (unsigned int index(0); index<m_ingredientsHLTJet.size(); index++)
        if ( m_ingredientsHLTJet.at(index)->hasFeature(feature) ) return true;

      return false;
    }

    /// Event cleanup
    void clear() {

      for (unsigned int index(0); index<m_ingredientsL1Jet.size(); index++)
        m_ingredientsL1Jet.at(index)->clear();

      for (unsigned int index(0); index<m_ingredientsHLTJet.size(); index++)
	m_ingredientsHLTJet.at(index)->clear();

    }
  
    /// Name
    std::string name() {return m_name;}

    /// Configuration accessors
    bool isSplit() {return (m_bjetConfig=="SPLIT");}
    bool isIP3DSV1() {return (m_bjetTagger=="IP3DSV1");}
    bool isCOMB() {return (m_bjetTagger=="COMB");}
    bool isMV2c20() {return (m_bjetTagger=="MV2c20");}
    std::string tagger() {return m_bjetTagger;}
    
    bool addJet(std::string item,std::vector< struct TrigBtagEmulationJet >& jets) {
      for (unsigned int index(0); index<m_ingredientsL1Jet.size(); index++)
	if (m_ingredientsL1Jet.at(index)->needsJet(item)) m_ingredientsL1Jet.at(index)->addJet(item,jets);
      
      for (unsigned int index(0); index<m_ingredientsHLTJet.size(); index++)
	if (m_ingredientsHLTJet.at(index)->needsJet(item)) m_ingredientsHLTJet.at(index)->addJet(item,jets);

      return true;
    }

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

    // SERVICES
    ToolHandle<Trig::TrigDecisionTool> m_trigDec;

#ifndef ROOTCORE
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

#ifdef ROOTCORE
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

    // jet Managers
    Trig::jetManager *manager_ef;
    Trig::jetManager *manager_split;
    Trig::jetManager *manager_gsc;
    Trig::jetManager *manager_ef_gsc;
    Trig::jetManager *manager_split_gsc;

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

    bool hasSplit() {return m_splitTrigger;}
    bool hasBtag() {return m_btagTrigger;}
    bool hasHT() {return m_htTrigger;}
    bool hasGSC() {return m_gscTrigger;}

  };

}//namespace

#endif
