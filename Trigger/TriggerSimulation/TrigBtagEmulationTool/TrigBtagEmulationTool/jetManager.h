/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Jet_Manager_H
#define Jet_Manager_H

#include "TrigBtagEmulationTool/feature.h"
#include "TrigBtagEmulationTool/baseTrigBtagEmulationChainJetIngredient.h"

// EDM
#include "xAODEventInfo/EventInfo.h"

#include "xAODTrigger/JetRoIContainer.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAttributes.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"

#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"

#include "TrigDecisionTool/TrigDecisionTool.h"


// If running Standalone
#ifdef XAOD_STANDALONE
#include "TrigConfxAOD/xAODConfigTool.h"
// If using Analysis Release
#elif defined( XAOD_ANALYSIS )
#include "StoreGate/StoreGateSvc.h"
// If using Athena
#else
#include "StoreGate/StoreGateSvc.h"
#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/BTagSecVertexing.h"
#include "BTagging/BTagTool.h"
#endif




namespace Trig {

  class jetManager {
  public:
    jetManager(ToolHandle<Trig::TrigDecisionTool>&,std::string,std::string,std::string);
    jetManager(const jetManager&);
    ~jetManager();

    void setKeys(std::string,std::string,std::string);

    StatusCode retrieveByNavigation();

#ifdef XAOD_STANDALONE
    StatusCode retrieveByContainer(asg::SgTEvent*);
#else
    StatusCode retrieveByContainer(ServiceHandle<StoreGateSvc>&);
#endif

    std::vector< struct TrigBtagEmulationJet >& getJets();

    StatusCode retagCopy(bool useNavigation=false,bool tagOffline=false,bool tagOnline=false);
    StatusCode retagOffline();
    StatusCode retagOnline();

    jetManager& operator+=(const jetManager&);
    jetManager& merge(const std::vector<const xAOD::Jet*>&, double minPt = 0, double maxPt = 0);

  private:
    bool clear();
    template<typename T> 
      bool getFromCombo(std::vector<const T*>&,const Trig::Combination&,std::string key="");
    bool getTPfromCombo(std::vector<const xAOD::TrackParticleContainer*>&,const Trig::Combination&,std::string);

#ifdef XAOD_STANDALONE
    StatusCode retrieveByContainerWithMatching(asg::SgTEvent*);
#else
    StatusCode retrieveByContainerWithMatching(ServiceHandle<StoreGateSvc>&);
#endif

    void jetCopy();
    bool isMatched(const xAOD::Jet*,const xAOD::Jet*);

  public:
    const std::string m_chain;
    const std::string m_jetContainer;
    const std::string m_btagContainer;

    std::string m_jet_key;
    std::string m_primaryVertex_key;
    std::string m_trackParticle_key;

    std::vector<const xAOD::Jet*> m_jet_Containers;
    std::vector<const xAOD::Vertex*> m_primaryVertex_Containers;
    std::vector<const xAOD::TrackParticleContainer*> m_trackParticle_Containers;
    std::vector<const xAOD::BTagging*> m_btagging_Containers;

    std::vector<TrigBtagEmulationJet> m_outputJets;

  private:
    ToolHandle<Trig::TrigDecisionTool> m_trigDec;

#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)
  public:
    static ToolHandle< Analysis::IBTagTool >* m_bTagTool;
    static ToolHandle< Analysis::IBTagTrackAssociation >* m_bTagTrackAssocTool;
    static ToolHandle< Analysis::IBTagSecVertexing >* m_bTagSecVtxTool;
#endif
  };

}//namespace

#endif
