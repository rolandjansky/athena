/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigJetSplitterMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// ************************************************

#include "TrigJetSplitterMT.h"
#include "CxxUtils/phihelper.h"

// ----------------------------------------------------------------------------------------------------------------- 

TrigJetSplitterMT::TrigJetSplitterMT(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {}

// ----------------------------------------------------------------------------------------------------------------- 

StatusCode TrigJetSplitterMT::initialize() {
  ATH_MSG_DEBUG(  "declareProperty review:"   );
  ATH_MSG_DEBUG(  "   " << m_inputJetsKey     );
  ATH_MSG_DEBUG(  "   " << m_outputJetsKey    );
  ATH_MSG_DEBUG(  "   " << m_imposeZconstraint);
  ATH_MSG_DEBUG(  "   " << m_etaHalfWidth     );
  ATH_MSG_DEBUG(  "   " << m_phiHalfWidth     );
  ATH_MSG_DEBUG(  "   " << m_zHalfWidth       );
  ATH_MSG_DEBUG(  "   " << m_minJetEt         );
  ATH_MSG_DEBUG(  "   " << m_maxJetEta        );

  ATH_MSG_DEBUG( "Initializing HandleKeys" );
  CHECK( m_inputJetsKey.initialize() );
  CHECK( m_inputVertexKey.initialize() );

  CHECK( m_outputJetsKey.initialize() );
  CHECK( m_outputRoiKey.initialize() );

  CHECK( m_inputRoIKey.initialize() ); // TMP
  return StatusCode::SUCCESS;
}


// ----------------------------------------------------------------------------------------------------------------- 


StatusCode TrigJetSplitterMT::execute() {
  ATH_MSG_DEBUG( "Executing " << name() );

  // ==============================================================================================================================
  //    ** Retrieve Ingredients
  // ==============================================================================================================================

  const EventContext& context = getContext();

  SG::ReadHandle< xAOD::JetContainer > inputJetContainerHandle = SG::makeHandle( m_inputJetsKey,context );
  ATH_MSG_DEBUG( "Retrieved jets from : " << m_inputJetsKey.key() );
  CHECK( inputJetContainerHandle.isValid() );

  const xAOD::JetContainer *inputJetCollection = inputJetContainerHandle.get();
  ATH_MSG_DEBUG( "Found " << inputJetCollection->size() << " jets."  );
  for ( const xAOD::Jet *jet : * inputJetCollection )
    ATH_MSG_DEBUG("   -- Jet pt=" << jet->p4().Et() <<" eta="<< jet->eta() << " phi="<< jet->phi() );

  // Retrieve Primary Vertex
  SG::ReadHandle< xAOD::VertexContainer > inputVertexContainer = SG::makeHandle( m_inputVertexKey,context );
  ATH_MSG_DEBUG( "Retrieving primary vertex from : " << m_inputVertexKey.key() );
  CHECK( inputVertexContainer.isValid() );

  const xAOD::VertexContainer *vertexContainer = inputVertexContainer.get();
  ATH_MSG_DEBUG( "Found PV container with " << vertexContainer->size() << " elements"  );
  for ( const xAOD::Vertex *vertex : *vertexContainer )
    ATH_MSG_DEBUG( "  ** PV = (" << vertex->x() <<
                   "," << vertex->y() <<
                   "," << vertex->z() << ")" );    

  if ( vertexContainer->size() == 0 ) return StatusCode::FAILURE;
  const xAOD::Vertex *primaryVertex = vertexContainer->at(0);

  if ( m_imposeZconstraint ) 
    ATH_MSG_DEBUG( "  ** PV = (" << primaryVertex->x() <<
                   "," << primaryVertex->y() <<
                   "," << primaryVertex->z() << ")" );
  
  // ==============================================================================================================================
  //    ** Prepare the outputs
  // ==============================================================================================================================

  std::unique_ptr< TrigRoiDescriptorCollection > outputRoiContainer( new TrigRoiDescriptorCollection() );

  std::unique_ptr< xAOD::JetContainer > outputJetContainer( new xAOD::JetContainer() );
  std::unique_ptr< xAOD::JetAuxContainer > outputJetAuxContainer( new xAOD::JetAuxContainer() );
  outputJetContainer->setStore( outputJetAuxContainer.get() );

  // ==============================================================================================================================
  //    ** Creating ShortList of Jet Container
  // ==============================================================================================================================

  CHECK( shortListJets( inputJetCollection,outputJetContainer,outputRoiContainer,primaryVertex ) );

  // ==============================================================================================================================
  //   ** Store the outputs
  // ==============================================================================================================================

  SG::WriteHandle< xAOD::JetContainer > outputJetContainerHandle =  SG::makeHandle( m_outputJetsKey, context );
  CHECK( outputJetContainerHandle.record( std::move( outputJetContainer ), std::move( outputJetAuxContainer ) ) );
  ATH_MSG_DEBUG( "Exiting with " << outputJetContainerHandle->size() << " shortlisted jets" );

  SG::WriteHandle< TrigRoiDescriptorCollection > outputRoIContainerHandle = SG::makeHandle( m_outputRoiKey,context );
  CHECK( outputRoIContainerHandle.record( std::move( outputRoiContainer ) ) );
  ATH_MSG_DEBUG( "Exiting with " << outputRoIContainerHandle->size() << " RoIs" );

  return StatusCode::SUCCESS;
}



StatusCode TrigJetSplitterMT::shortListJets( const xAOD::JetContainer* jetCollection,
					     std::unique_ptr< xAOD::JetContainer >& outputJets,
					     std::unique_ptr< TrigRoiDescriptorCollection >& roiContainer,
					     const xAOD::Vertex* primaryVertex ) const {

  // Make a copy of the jet containers
  for ( const xAOD::Jet *jet : *jetCollection ) {
    // We select Jets above a specific eta and pt range
    if ( jet->p4().Et() < m_minJetEt ) {
      ATH_MSG_DEBUG( "** Jet below the " << m_minJetEt.value() << " GeV threshold; Et " << jet->p4().Et() <<"; Skipping this Jet."  );
      continue;
    }
    if ( fabs( jet->eta() ) > m_maxJetEta ) {
      ATH_MSG_DEBUG( "** Jet outside the |eta| < " << m_maxJetEta.value() << " requirement; Eta = " << jet->eta() << "; Skipping this Jet." );
      continue;
    }
    ATH_MSG_DEBUG( "** Jet :: Et " << jet->p4().Et() <<"; Eta " << jet->eta() << "; Phi " << jet->phi() );

    // Protection in case there is not a Primary vertex but the Z contraint option is set to True
    if ( m_imposeZconstraint && primaryVertex == nullptr ) {
      // Not sure here what the best solution is. We can't change the m_imposeZconstraint value being const (and not thread safe)
      ATH_MSG_ERROR( "Option for imposing Z constraint is set to True, but no primary vertex has been found." );
      return StatusCode::FAILURE;
    }

    // Copy Jet
    xAOD::Jet *toBeAdded = new xAOD::Jet();
    outputJets->push_back( toBeAdded );
    *toBeAdded = *jet;

    // Create RoI (we may require here PVz constraint)
    double phiMinus = CxxUtils::wrapToPi(jet->phi() - m_phiHalfWidth);
    double phiPlus  = CxxUtils::wrapToPi(jet->phi() + m_phiHalfWidth);

    double etaMinus = jet->eta() - m_etaHalfWidth;
    double etaPlus  = jet->phi() + m_etaHalfWidth;

    // Impose Z matching (if enabled)
    ATH_MSG_DEBUG( "Building RoI" );
    TrigRoiDescriptor *newRoI = nullptr;
    if ( not m_imposeZconstraint ) {
      newRoI = new TrigRoiDescriptor( jet->eta(),etaMinus, etaPlus,
                                      jet->phi(), phiMinus, phiPlus );
    } else {
      ATH_MSG_DEBUG( "  ** Imposing Z constraint while building RoI" );
      double zMinus = primaryVertex->z() - m_zHalfWidth;
      double zPlus  = primaryVertex->z() + m_zHalfWidth;

      newRoI = new TrigRoiDescriptor( jet->eta(),etaMinus, etaPlus,
                                      jet->phi(), phiMinus, phiPlus,
                                      primaryVertex->z(),zMinus,zPlus );
    }
    ATH_MSG_DEBUG( "  -- RoI : eta=" << newRoI->eta() << " phi=" << newRoI->phi() );

    // Put protection against nullpointer // TO-DO
    roiContainer->push_back( newRoI );
  }

  return StatusCode::SUCCESS;
}
