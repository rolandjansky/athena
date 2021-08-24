//Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "FlavourTaggingTests/FTAGValidationAlgorithm.h"

namespace FTAGValidation {

  FTAGValidationAlgorithm::FTAGValidationAlgorithm( const std::string& name,
						    ISvcLocator* pSvcLocator ) :
    ::AthAlgorithm( name, pSvcLocator ) {}

  FTAGValidationAlgorithm::~FTAGValidationAlgorithm() {}

  StatusCode FTAGValidationAlgorithm::initialize() {
    ATH_MSG_INFO( "Inizializing " << name() << " ..." );
    ATH_CHECK( m_eventInfoKey.initialize() );
    return StatusCode::SUCCESS;
  }

  /* ===================================================================== */

  bool FTAGValidationAlgorithm::passJetKinematicSelection( const xAOD::Jet* jet ) const {

    if ( jet->p4().Et() / Gaudi::Units::GeV < m_minJetPt ) return false;
    if ( std::fabs( jet->eta() ) < m_minJetEta ) return false;
    if ( std::fabs( jet->eta() ) > m_maxJetEta ) return false;

    return true;
  }

  bool FTAGValidationAlgorithm::passTriggerJetKinematicSelection( const xAOD::Jet* jet ) const {

    if ( jet->p4().Et() / Gaudi::Units::GeV < m_minTrigJetPt ) return false;
    if ( std::fabs( jet->eta() ) > m_maxTrigJetEta ) return false;

    return true;
  }

  bool FTAGValidationAlgorithm::passJetQualitySelection( const xAOD::Jet* jet ) const {

    // This part is inspired from 'PhysicsAnalysis/JetTagging/JetTagMonitoring/src/JetTagMonitorAlgorithm.cxx'

    float jetQuality = 0;
    if ( !jet->getAttribute(xAOD::JetAttribute::LArQuality,jetQuality) ) {
      ATH_MSG_ERROR( "JetAttribute 'xAOD::JetAttribute::LArQuality' is not available." );
      return false;
    }

    float jetTime = 0;
    if ( !jet->getAttribute(xAOD::JetAttribute::Timing,jetTime) ) {
      ATH_MSG_ERROR( "JetAttribute 'xAOD::JetAttribute::Timing' is not available." );
      return false;
    }

    float hecq = 0;
    if ( !jet->getAttribute(xAOD::JetAttribute::HECQuality,hecq) ) {
      ATH_MSG_ERROR( "JetAttribute 'xAOD::JetAttribute::HECQuality' is not available." );
      return false;
    }

    float negE = 0;
    if ( !jet->getAttribute(xAOD::JetAttribute::NegativeE,negE) ) {
      ATH_MSG_ERROR( "JetAttribute 'xAOD::JetAttribute::NegativeE' is not available." );
      return false;
    }

    std::vector< float > SumPtTrkPt1000;
    if ( !jet->getAttribute(xAOD::JetAttribute::SumPtTrkPt1000,SumPtTrkPt1000) ) {
      ATH_MSG_ERROR( "JetAttribute 'xAOD::JetAttribute::SumPtTrkPt1000' is not available." );
      return false;
    }
    float chf             = SumPtTrkPt1000.size() > 0 ? SumPtTrkPt1000.at(0)/jet->pt() : -1;

    float emf = 0;
    if ( !jet->getAttribute(xAOD::JetAttribute::EMFrac,emf) ) {
      ATH_MSG_ERROR( "JetAttribute 'xAOD::JetAttribute::EMFrac' is not available." );
      return false;
    }

    float hecf = 0;
    if ( !jet->getAttribute(xAOD::JetAttribute::HECFrac,hecf) ) {
      ATH_MSG_ERROR( "JetAttribute 'xAOD::JetAttribute::HECFrac' is not available." );
      return false;
    }

    float fracSamplingMax = 0;
    if ( !jet->getAttribute(xAOD::JetAttribute::FracSamplingMax,fracSamplingMax) ) {
      ATH_MSG_ERROR( "JetAttribute 'xAOD::JetAttribute::FracSamplingMax' is not available." );
      return false;
    }

    if ( hecf > 0.5 && fabs( hecq ) > 0.5 ) return false;
    if ( fabs( negE ) > 60*Gaudi::Units::GeV ) return false;
    if ( emf > 0.95 && fabs( jetQuality ) > 0.8 && fabs( jet->eta() ) < 2.8 ) return false;
    if ( fabs( jetTime ) > 25 ) return false;
    if ( emf < 0.05 && chf < 0.05 && fabs( jet->eta() ) < 2 ) return false;
    if ( emf< 0.05 && fabs( jet->eta() ) >= 2 ) return false;
    if ( fracSamplingMax > 0.99 && fabs( jet->eta() ) < 2 ) return false;

    return true;
  }

  bool FTAGValidationAlgorithm::passJetJVTSelection( const xAOD::Jet* jet,
						     const std::string& jetType ) const {

    if ( jet->isAvailable< float >( "Jvt" ) == false ) {
      ATH_MSG_ERROR( "Jet auxdata 'Jvt' is not available." );
      return false;
    }

    float jvt = jet->auxdata< float >( "Jvt" );
    double jvtCut = 0.59;
    if ( jetType == "AntiKt4EMPFlowJets" )
      jvtCut = 0.2;

    if ( jet->pt() / Gaudi::Units::GeV >= 60. ) 
      return true; 
    if ( fabs( jet->eta() ) >= 2.4 )
      return true;
    if ( jvt > jvtCut ) 
      return true;

    return false;
  }

  const xAOD::Vertex* FTAGValidationAlgorithm::getPrimaryVertex( const xAOD::VertexContainer* vertexContainer ) const {
    
    if ( vertexContainer->size() == 0 ) {
      ATH_MSG_WARNING( "Vertex Container has size 0! This can't be right!" );
      return nullptr;
    }

    for ( unsigned int i(0); i<vertexContainer->size(); i++ ) {
      const xAOD::Vertex *vertex = vertexContainer->at(i);
      if ( vertex->vertexType() != xAOD::VxType::VertexType::PriVtx ) continue;
      return vertex;
    }
    
    ATH_MSG_DEBUG( "None of the vertexes in the vertex container is a primary vertex!" );
    ATH_MSG_DEBUG( "Using Dummy Vertex." );
    return vertexContainer->at(0);
  }
  
  int FTAGValidationAlgorithm::getMatchedOfflineJetIndex( const xAOD::Jet* onJet, std::vector< const xAOD::Jet* > offJets ) const {
    int matchedOffJetIndex = -1;
    double minDr = 0;

    TLorentzVector onJet_4vec, offJet_4vec;
    onJet_4vec.SetPtEtaPhiM(onJet->pt(), onJet->eta(), onJet->phi(), onJet->m());

    for ( unsigned int index=0; index<offJets.size(); index++ ) {
      const xAOD::Jet *offJet = offJets.at( index );

      offJet_4vec.SetPtEtaPhiM(offJet->pt(), offJet->eta(), offJet->phi(), offJet->m());
      double deltaR = onJet_4vec.DeltaR( offJet_4vec );
      ATH_MSG_DEBUG("deltaR: " << deltaR );

      if ( deltaR > 0.1 ) continue;
      if ( deltaR < minDr || matchedOffJetIndex == -1 ) {
        minDr = deltaR;
        matchedOffJetIndex = index;
      }
    }
    ATH_MSG_DEBUG("minDr: " << minDr );
    ATH_MSG_DEBUG("matchedOffJetIndex: " << matchedOffJetIndex );
    return matchedOffJetIndex;
  }

}




