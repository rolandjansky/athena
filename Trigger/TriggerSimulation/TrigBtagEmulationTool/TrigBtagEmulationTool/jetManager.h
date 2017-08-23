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

#ifndef ROOTCORE
#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/BTagSecVertexing.h"
#include "BTagging/BTagTool.h"
#include "StoreGate/StoreGateSvc.h"
#else
#include "TrigConfxAOD/xAODConfigTool.h"
#endif

#include "TrigDecisionTool/TrigDecisionTool.h"

namespace Trig {

  class jetManager {
  public:
    jetManager(ToolHandle<Trig::TrigDecisionTool>&,std::string,std::string,std::string);
    jetManager(const jetManager&);
    ~jetManager();

    void setKeys(std::string,std::string,std::string);

    StatusCode retrieveByNavigation();

#ifdef ROOTCORE
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

#ifdef ROOTCORE
    StatusCode retrieveByContainerWithMatching(asg::SgTEvent*);
#else
    StatusCode retrieveByContainerWithMatching(ServiceHandle<StoreGateSvc>&);
#endif

    void jetCopy();
    bool isMatched(const xAOD::Jet*,const xAOD::Jet*);

  public:
    const std::string _chain;
    const std::string _jetContainer;
    const std::string _btagContainer;

    std::string _jet_key;
    std::string _primaryVertex_key;
    std::string _trackParticle_key;

    std::vector<const xAOD::Jet*> _jet_Containers;
    std::vector<const xAOD::Vertex*> _primaryVertex_Containers;
    std::vector<const xAOD::TrackParticleContainer*> _trackParticle_Containers;
    std::vector<const xAOD::BTagging*> _btagging_Containers;

    std::vector<TrigBtagEmulationJet> _outputJets;

  private:
    ToolHandle<Trig::TrigDecisionTool> m_trigDec;

#ifndef ROOTCORE
  public:
    static ToolHandle< Analysis::IBTagTool >* m_bTagTool;
    static ToolHandle< Analysis::IBTagTrackAssociation >* m_bTagTrackAssocTool;
    static ToolHandle< Analysis::IBTagSecVertexing >* m_bTagSecVtxTool;
#endif
  };

}//namespace

#endif
