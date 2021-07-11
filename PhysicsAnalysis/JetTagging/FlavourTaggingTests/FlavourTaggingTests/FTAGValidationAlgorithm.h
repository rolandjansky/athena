//Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef FTAGVALIDATION_FTAGVALIDATIONALGORITHM_H
#define FTAGVALIDATION_FTAGVALIDATIONALGORITHM_H 1

#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODEventInfo/EventInfo.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

namespace FTAGValidation {

  class FTAGValidationAlgorithm :
    public ::AthAlgorithm {

  public:
    FTAGValidationAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~FTAGValidationAlgorithm();

    virtual StatusCode initialize();

  protected:
    template< class CONTAINER >
      StatusCode retrieveCollectionfromStoreGate( const EventContext&,
						  const CONTAINER*&,
						  const SG::ReadHandleKey< CONTAINER >& ) const;


    bool passJetKinematicSelection( const xAOD::Jet* ) const;
    bool passTriggerJetKinematicSelection( const xAOD::Jet* ) const;
    bool passJetQualitySelection( const xAOD::Jet* ) const;
    bool passJetJVTSelection( const xAOD::Jet*,const std::string& jetType="AntiKt4EMTopoJets" ) const;

    const xAOD::Vertex* getPrimaryVertex( const xAOD::VertexContainer* ) const;

    int getMatchedOfflineJetIndex( const xAOD::Jet*, std::vector< const xAOD::Jet* > ) const;

  private:
    FTAGValidationAlgorithm();

  protected:
    SG::ReadHandleKey< xAOD::EventInfo > m_eventInfoKey { this,"EventInfoKey","EventInfo",
	"Key to read EventInfo."};

    Gaudi::Property< float > m_minJetEta { this,"MinJetEta",0.,
        "Minimum Eta range for jets"};

    Gaudi::Property< float > m_maxJetEta { this,"MaxJetEta",2.5,
        "Maximum Eta range for jets"};

    Gaudi::Property< float > m_minJetPt { this,"MinJetPt", 20.,
        "Minimum Jet Pt [GeV]" };

    Gaudi::Property< float > m_maxTrigJetEta { this,"MaxTrigJetEta",2.5,
        "Maximum Trigger Eta range for jets"};

    Gaudi::Property< float > m_minTrigJetPt { this,"MinTrigJetPt", 20.,
        "Minimum Trigger Jet Pt [GeV]" };

  };

  #include "FlavourTaggingTests/FTAGValidationAlgorithm.icc"
  
} //> end namespace FTAGValidation

#endif
