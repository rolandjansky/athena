/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Jet_Manager_H
#define Jet_Manager_H

#include "TrigBtagEmulationTool/TriggerFeature.h"
#include "TrigBtagEmulationTool/BaseTrigBtagEmulationChainJetIngredient.h"

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

  class JetManager : public asg::AsgTool { 
  public:
    JetManager(std::string,ToolHandle<Trig::TrigDecisionTool>&,const std::string chain = "");
    ~JetManager();

    void setContainers(std::string,std::string);
    void setKeys(std::string,std::string,std::string);

    StatusCode retrieveByNavigation();
    StatusCode retrieveByContainer();

    std::vector< std::unique_ptr< TrigBtagEmulationJet > > getJets();

    StatusCode retagCopy(bool useNavigation=false,bool tagOffline=false,bool tagOnline=false);
    StatusCode retagOffline();
    StatusCode retagOnline();

    JetManager& merge(const std::unique_ptr< JetManager >&);
    JetManager& merge(const std::vector<const xAOD::Jet*>&, double minPt = 0, double maxPt = 0);

    void use4x4( bool set4x4 = true );

    std::string chainName() const;
    std::string jetContainerName() const;
    std::string btaggingContainerName() const;
    std::string jetKeyName() const;
    std::string btaggingKeyName() const;
    std::string primaryVertexKeyName() const;
    std::string trackParticleKeyName() const;

    unsigned int jetSize() const;
    unsigned int jetRoISize() const;
    unsigned int btaggingSize() const;
    unsigned int primaryVertexSize() const;
    unsigned int trackParticleSize() const;

  private:
    bool clear();
    template<typename T> 
      bool getFromCombo(std::vector<const T*>&,const Trig::Combination&,std::string key="");
    bool getTPfromCombo(std::vector<const xAOD::TrackParticleContainer*>&,const Trig::Combination&,std::string);


    StatusCode retrieveJetContainer();

    void jetCopy( std::vector< const xAOD::Jet* >& );
    void jetCopy( std::vector< const xAOD::JetRoI* >&);
    bool matchedSPLITjet(const xAOD::Jet*,const xAOD::Jet*);

    // ========================== //
    // ======= Attributes ======= //
    // ========================== //

  public:
    enum EventElement { JET=0, BTAG=1, PRIM_VTX=2, TRK_PARTICLE=3};

  protected:
    // Chain: source of unbiased b-jets 
    const std::string m_chain;

    // Containers (we use only jet and bTag containers)
    std::tuple< std::string,std::string > m_containers;
    // Keys for Navigation
    std::tuple< std::string,std::string,std::string,std::string > m_keys;

    // For jetRoI, see if 4x4 should be used instead of 8x8
    bool m_uses4x4;

    // Local containers
    std::vector<const xAOD::Jet*> m_jet_Containers;
    std::vector<const xAOD::JetRoI*> m_jetRoI_Containers;
    std::vector<const xAOD::Vertex*> m_primaryVertex_Containers;
    std::vector<const xAOD::TrackParticleContainer*> m_trackParticle_Containers;
    std::vector<const xAOD::BTagging*> m_btagging_Containers;

    std::vector< std::unique_ptr< TrigBtagEmulationJet > > m_outputJets;

  private:
    ToolHandle< Trig::TrigDecisionTool > m_trigDec;

#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)
  public:
    static ToolHandle< Analysis::IBTagTool >* m_bTagTool;
    static ToolHandle< Analysis::IBTagTrackAssociation >* m_bTagTrackAssocTool;
    static ToolHandle< Analysis::IBTagSecVertexing >* m_bTagSecVtxTool;
#endif
  };

}//namespace

#endif
