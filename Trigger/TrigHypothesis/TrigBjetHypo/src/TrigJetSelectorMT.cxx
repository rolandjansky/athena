/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigJetSplitterMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// ************************************************

#include "TrigJetSelectorMT.h"
#include "CxxUtils/phihelper.h"

// ----------------------------------------------------------------------------------------------------------------- 

TrigJetSelectorMT::TrigJetSelectorMT(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {}

// ----------------------------------------------------------------------------------------------------------------- 

StatusCode TrigJetSelectorMT::initialize() {

  ATH_MSG_DEBUG( "Initializing " << name() << "... " );

  ATH_MSG_DEBUG( "declareProperty review:" );
  ATH_MSG_DEBUG( "   ** " << m_minJetEt );
  ATH_MSG_DEBUG( "   ** " << m_maxJetEta );
  ATH_MSG_DEBUG( "   ** " << m_etaHalfWidth );
  ATH_MSG_DEBUG( "   ** " << m_phiHalfWidth );
  ATH_MSG_DEBUG( "   ** " << m_zHalfWidth );

  ATH_MSG_DEBUG( "Initializing HandleKeys" );
  CHECK( m_inputJetsKey.initialize() );
  CHECK( m_inputVertexKey.initialize() );
  CHECK( m_outputJetKey.initialize() );
  CHECK( m_outputRoiKey.initialize() );

  return StatusCode::SUCCESS;
}


// ----------------------------------------------------------------------------------------------------------------- 


StatusCode TrigJetSelectorMT::execute() {

  ATH_MSG_DEBUG( "Executing " << name() );
  const EventContext& context = getContext();

  // ==============================================================================================================================
  //    ** Retrieve Ingredients
  // ==============================================================================================================================

  // Jets
  SG::ReadHandle< xAOD::JetContainer > inputJetContainerHandle = SG::makeHandle( m_inputJetsKey,context );
  ATH_MSG_DEBUG( "Retrieved jets from : " << m_inputJetsKey.key() );
  CHECK( inputJetContainerHandle.isValid() );

  const xAOD::JetContainer *inputJetCollection = inputJetContainerHandle.get();
  ATH_MSG_DEBUG( "Found " << inputJetCollection->size() << " jets."  );
  for ( const xAOD::Jet *jet : * inputJetCollection )
    ATH_MSG_DEBUG("   -- Jet pt=" << jet->p4().Et() <<" eta="<< jet->eta() << " phi="<< jet->phi() );

  // Primary Vertex
  SG::ReadHandle< xAOD::VertexContainer > inputVertexContainer = SG::makeHandle( m_inputVertexKey,context );
  ATH_MSG_DEBUG( "Retrieving primary vertex from : " << m_inputVertexKey.key() );
  CHECK( inputVertexContainer.isValid() );

  const xAOD::VertexContainer *vertexContainer = inputVertexContainer.get();
  ATH_MSG_DEBUG( "Found Vertex container with " << vertexContainer->size() << " elements"  );
  for ( const xAOD::Vertex *vertex : *vertexContainer )
    ATH_MSG_DEBUG( "  ** Vertex = (" << vertex->x() <<
                   "," << vertex->y() <<
                   "," << vertex->z() << ")" );

  const xAOD::Vertex *primaryVertex = getPrimaryVertex( vertexContainer );  
  if ( primaryVertex == nullptr ) {
    ATH_MSG_ERROR( "No primary vertex has been found for this event!" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "  ** PV = (" << primaryVertex->x() <<
		 "," << primaryVertex->y() <<
		 "," << primaryVertex->z() << ")" );

  // ==============================================================================================================================
  //    ** Prepare the output collections
  // ==============================================================================================================================

  std::unique_ptr< xAOD::JetContainer > outputJetContainer = std::make_unique< xAOD::JetContainer >();
  std::unique_ptr< xAOD::JetAuxContainer > outputJetAuxContainer = std::make_unique< xAOD::JetAuxContainer >();
  outputJetContainer->setStore( outputJetAuxContainer.get() );

  std::unique_ptr< TrigRoiDescriptorCollection > outputRoiContainer = std::make_unique< TrigRoiDescriptorCollection >();

  // ==============================================================================================================================
  //    ** Do the computation
  // ==============================================================================================================================

  for ( const xAOD::Jet *jet : *inputJetCollection ) {
    double jetPt = jet->p4().Et() * 1e-3;
    double jetEta = jet->eta();
    double jetPhi = jet->phi();

    ATH_MSG_DEBUG( "** Jet :: Et " << jetPt <<"; Eta " << jetEta << "; Phi " << jetPhi );

    if ( jetPt < m_minJetEt ) {
      ATH_MSG_DEBUG( "** Jet below the " << m_minJetEt.value() << " GeV threshold; Et " << jetPt <<"; Skipping this Jet."  );
      continue;
    }

    if ( fabs( jetEta ) > m_maxJetEta ) {
      ATH_MSG_DEBUG( "** Jet outside the |eta| < " << m_maxJetEta.value() << " requirement (ID acceptance); Eta = " << jetEta << "; Skipping this Jet." );
      continue;
    }

    // Copy Jet
    xAOD::Jet *toBeAdded = new xAOD::Jet();
    outputJetContainer->push_back( toBeAdded );
    *toBeAdded = *jet;

    // Create RoI
    ATH_MSG_DEBUG( "  ** Imposing Z constraint while building RoI" );
    double phiMinus = CxxUtils::wrapToPi( jetPhi - m_phiHalfWidth );
    double phiPlus  = CxxUtils::wrapToPi( jetPhi + m_phiHalfWidth );

    double etaMinus = jetEta - m_etaHalfWidth;
    double etaPlus  = jetEta + m_etaHalfWidth;

    double zMinus = primaryVertex->z() - m_zHalfWidth;
    double zPlus  = primaryVertex->z() + m_zHalfWidth;

    TrigRoiDescriptor *newRoI = new TrigRoiDescriptor( jet->eta(),etaMinus, etaPlus,
						       jet->phi(), phiMinus, phiPlus,
						       primaryVertex->z(),zMinus,zPlus );
    outputRoiContainer->push_back( newRoI );
  }


  // ==============================================================================================================================
  //    ** Store object collections
  // ==============================================================================================================================

  SG::WriteHandle< xAOD::JetContainer > outputJetContainerHandle =  SG::makeHandle( m_outputJetKey, context );
  CHECK( outputJetContainerHandle.record( std::move( outputJetContainer ), std::move( outputJetAuxContainer ) ) );
  ATH_MSG_DEBUG( "Exiting with " << outputJetContainerHandle->size() << " shortlisted jets" );

  SG::WriteHandle< TrigRoiDescriptorCollection > outputRoIContainerHandle = SG::makeHandle( m_outputRoiKey,context );
  CHECK( outputRoIContainerHandle.record( std::move( outputRoiContainer ) ) );
  ATH_MSG_DEBUG( "Exiting with " << outputRoIContainerHandle->size() << " RoIs" );

  return StatusCode::SUCCESS;
}

const xAOD::Vertex* TrigJetSelectorMT::getPrimaryVertex( const xAOD::VertexContainer* vertexContainer ) const {
  // In case we need more complex selection
  if ( vertexContainer->size() == 0 ) {
    ATH_MSG_WARNING( "Vertex Container has size 0! This can't be right!" );
    return nullptr;
  }

  for ( const xAOD::Vertex *vertex : *vertexContainer ) {
    if ( vertex->vertexType() != xAOD::VxType::VertexType::PriVtx ) continue;
    return vertex;
  }

  ATH_MSG_DEBUG( "None of the vertexes in the vertex container is a primary vertex!" );
  ATH_MSG_DEBUG( "Using dummy vertex!" );
  return vertexContainer->front();
}
