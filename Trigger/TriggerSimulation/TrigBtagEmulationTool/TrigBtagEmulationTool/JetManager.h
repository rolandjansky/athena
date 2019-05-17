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

#include "xAODJet/JetAttributes.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

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
    void setKeys(std::string,std::string,std::string,std::string);

    StatusCode retrieveByNavigation();
    StatusCode retrieveByContainer();

    std::vector< std::unique_ptr< TrigBtagEmulationJet > > getJets();

    StatusCode retagCopy(bool useNavigation=false,bool tagOffline=false,bool tagOnline=false);
    StatusCode retagOffline();
    StatusCode retagOnline();

    JetManager& merge(const std::unique_ptr< JetManager >&);
    JetManager& merge(const std::unique_ptr< JetManager >&,double minPt,double maxPt = 0);
    JetManager& merge(std::unique_ptr< xAOD::JetContainer >&, double minPt = 0, double maxPt = 0);

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
      bool getFromCombo(std::unique_ptr< DataVector< T > >&,const Trig::Combination&,std::string key="");
    bool getTPfromCombo(std::vector< std::unique_ptr< xAOD::TrackParticleContainer > >&,const Trig::Combination&,std::string);

    StatusCode retrieveJetContainer();

    void jetCopy( std::unique_ptr< xAOD::JetContainer >& );
    void jetCopy( std::unique_ptr< xAOD::JetRoIContainer>& );
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
    std::unique_ptr< xAOD::JetContainer > m_jet_Containers;
    std::unique_ptr< xAOD::JetRoIContainer > m_jetRoI_Containers;
    std::unique_ptr< xAOD::BTaggingContainer > m_btagging_Containers;
    std::unique_ptr< xAOD::VertexContainer > m_primaryVertex_Containers;
    std::vector< std::unique_ptr< xAOD::TrackParticleContainer > > m_trackParticle_Containers;

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

  template<typename T,typename U> static T retrieveAuxData(const U *,const std::string&,T defaultValue = T() );
  template<typename T,typename U> static const T retrieveAuxDataConst( const U * ,const std::string&, const T defaultValue = T() );

}//namespace

#endif
