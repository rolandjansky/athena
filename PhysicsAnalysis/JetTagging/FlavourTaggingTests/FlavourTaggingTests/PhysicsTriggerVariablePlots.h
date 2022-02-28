//Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef FTAGVALIDATION_PHYSICSTRIGGERVARIABLEPLOTS_H
#define FTAGVALIDATION_PHYSICSTRIGGERVARIABLEPLOTS_H 1

#include "FlavourTaggingTests/FTAGValidationAlgorithm.h"
#include "FlavourTaggingTests/PhysicsVariablePlots.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "xAODTrigger/TrigComposite.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TH2.h"
#include "map"
#include "tuple"

namespace FTAGValidation {

  class PhysicsTriggerVariablePlots :
    public PhysicsVariablePlots {

  public:
    PhysicsTriggerVariablePlots( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~PhysicsTriggerVariablePlots(); 
    
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    PhysicsTriggerVariablePlots();

    int m_nTotal_events;
    int m_nTotal_noPV;
    int m_nTotal_noMatchedOffJet;
    int m_nTotal_otherFlavour;
    int m_nTotal_qualityOnlineJets;

  private:
    SG::ReadHandleKey< xAOD::VertexContainer > m_trigVertexKey { this, "TrigVertexCollectionKey", "",
	"Input Vertex collection name" };
    SG::ReadHandleKey< xAOD::TrackParticleContainer > m_trigTrackKey { this, "TrigTrackParticleCollectionKey", "",
	"Input Track collection name" };
    SG::ReadHandleKey< xAOD::JetContainer >  m_offJetKey  { this, "OfflineJetCollectionKey", "",   
	"Input offline Jet collection name" };
    ToolHandle<Trig::TrigDecisionTool> m_trigDec { this, "TrigDecisionTool", "",
        "Input TrigDecision Tool" };
    std::vector<std::string> m_bJetChains;

  };

} //> end namespace FTAGValidation

#endif

