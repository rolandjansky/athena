/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigBjetEtHypoAlgMT.h"
#include "TrigSteeringEvent/PhiHelper.h"

TrigBjetEtHypoAlgMT::TrigBjetEtHypoAlgMT( const std::string& name, 
					  ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ) {}

TrigBjetEtHypoAlgMT::~TrigBjetEtHypoAlgMT()
{}

StatusCode TrigBjetEtHypoAlgMT::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  ATH_MSG_DEBUG(  "declareProperty review:"   );
  ATH_MSG_DEBUG(  "   " << m_jetsKey          );
  ATH_MSG_DEBUG(  "   " << m_outputJetsKey    );
  ATH_MSG_DEBUG(  "   " << m_imposeZconstraint);
  ATH_MSG_DEBUG(  "   " << m_etaHalfWidth     );
  ATH_MSG_DEBUG(  "   " << m_phiHalfWidth     );
  ATH_MSG_DEBUG(  "   " << m_zHalfWidth       );
  ATH_MSG_DEBUG(  "   " << m_minJetEt         );
  ATH_MSG_DEBUG(  "   " << m_maxJetEta        );

  ATH_MSG_DEBUG( "Initializing Tools" );
  ATH_CHECK( m_hypoTools.retrieve() );

  ATH_MSG_DEBUG( "Initializing HandleKeys" );
  CHECK( m_jetsKey.initialize() );
  CHECK( m_outputJetsKey.initialize() );
  CHECK( m_outputRoiKey.initialize() );
  // FOLLOWING WILL CHANGE -- JUST FOR TEMPORARY DEBUGGING
  CHECK( m_trackParticleContainerKey.initialize() ); // TMP
  CHECK( m_roiKey.initialize() ); // TMP

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT::execute_r( const EventContext& context ) const {  
  ATH_MSG_INFO ( "Executing " << name() << "..." );

  // ========================================================================================================================== 
  //    ** Retrieve Ingredients
  // ========================================================================================================================== 

  // Taken from Jet code
  // Read in previous Decisions made before running this Hypo Alg.
  // The container should have only one such Decision in case we are cutting on 'j' threshold (for L1)
  SG::ReadHandle< TrigCompositeUtils::DecisionContainer > prevDecisionHandle = SG::makeHandle( decisionInput(),context );
  CHECK( prevDecisionHandle.isValid() );
  const TrigCompositeUtils::DecisionContainer *prevDecisionContainer = prevDecisionHandle.get();
  ATH_MSG_DEBUG( "Running with "<< prevDecisionContainer->size() <<" previous decisions");

  // =============================================================  
  // Just for debugging -- WILL BE REMOVED

  // Retrieve ROIs from RoIBuilder // TMP
  SG::ReadHandle< TrigRoiDescriptorCollection > superRoiHandle = SG::makeHandle( m_roiKey,context ); // TMP
  CHECK( superRoiHandle.isValid() ); // TMP
  const TrigRoiDescriptorCollection *superRoi = superRoiHandle.get(); // TMP
  ATH_MSG_DEBUG( "Retrieved Super RoI Container with size " << superRoi->size() ); // TMP
  for ( auto *roi : *superRoi ) // TMP
    ATH_MSG_DEBUG( "   ** roi : eta=" << roi->eta() <<" phi="<< roi->phi() ); // TMP

  // Retrieve Track Particles // TMP
  SG::ReadHandle< xAOD::TrackParticleContainer > recoTracksContainerHandle = SG::makeHandle( m_trackParticleContainerKey,context ); // TMP
  CHECK( recoTracksContainerHandle.isValid() ); // TMP
  const xAOD::TrackParticleContainer *recoTracksContainer = recoTracksContainerHandle.get(); // TMP
  ATH_MSG_DEBUG( "Retrieved " << recoTracksContainer->size() << " Track Particles from FTF step" ); // TMP
  for ( auto *particle : *recoTracksContainer ) // TMP
    ATH_MSG_DEBUG( "   ** pt=" << particle->p4().Et()<<" eta="<< particle->eta() <<" phi="<< particle->phi()  ); // TMP

  // =============================================================

  // Retrieve Jet Container ( Calo Jets )
  SG::ReadHandle< xAOD::JetContainer > jetContainerHandle = SG::makeHandle( m_jetsKey,context );
  ATH_MSG_DEBUG( "Retrieved jets from : " << m_jetsKey.key() );
  CHECK( jetContainerHandle.isValid() );

  const xAOD::JetContainer *jetCollection = jetContainerHandle.get();
  ATH_MSG_DEBUG( "Found " << jetCollection->size()<< " jets."  );
  for ( const xAOD::Jet *jet : * jetCollection ) 
    ATH_MSG_INFO("   -- Jet pt=" << jet->p4().Et() <<" eta="<< jet->eta() << " phi="<< jet->phi() );

  // Retrieve Primary Vertex
  // Right now vertexing is not available. Using dummy vertex at (0,0,0) // TMP
  const xAOD::VertexContainer *vertexContainer = nullptr; // TMP
  const Amg::Vector3D *primaryVertex = nullptr; // TMP

  if ( m_imposeZconstraint ) {
    ATH_MSG_DEBUG( "Retrieving primary vertex." );
    // Here we should retrieve the primary vertex // TO-DO
    primaryVertex = new Amg::Vector3D( 0,0,0 ); // TMP
    // Add protection against failure during primary vertex retrieval. // TO-DO
    ATH_MSG_DEBUG( "  ** PV = (" << primaryVertex->x() <<   
		   "," << primaryVertex->y() << 
		   "," << primaryVertex->z() << ")" ); 
  }


  // ========================================================================================================================== 
  //    ** Prepare Outputs
  // ========================================================================================================================== 

  // Prepare Output 
  // Output RoIs -- WILL CHANGE -- TMP
  std::unique_ptr< TrigRoiDescriptorCollection > roiContainer( new TrigRoiDescriptorCollection() ); // TMP

  // Output Jet Collection
  std::unique_ptr< xAOD::JetContainer > outputJets( new xAOD::JetContainer() );
  std::unique_ptr< xAOD::JetAuxContainer > outputJetsAux( new xAOD::JetAuxContainer() );
  outputJets->setStore( outputJetsAux.get() );

  // Decisions
  std::unique_ptr< TrigCompositeUtils::DecisionContainer > decisions = std::make_unique< TrigCompositeUtils::DecisionContainer >();
  std::unique_ptr< TrigCompositeUtils::DecisionAuxContainer > aux = std::make_unique< TrigCompositeUtils::DecisionAuxContainer >();
  decisions->setStore( aux.get() );

  // ==========================================================================================================================
  //    ** Creating ShortList of Jet Container
  // ==========================================================================================================================
  
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
    double phiMinus = HLT::wrapPhi(jet->phi() - m_phiHalfWidth);
    double phiPlus  = HLT::wrapPhi(jet->phi() + m_phiHalfWidth);

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

  // ==========================================================================================================================
  //    ** Compute Decisions
  // ==========================================================================================================================

  // Taken from Jet Code here 
  const TrigCompositeUtils::Decision *prevDecision = prevDecisionContainer->at(0);
  TrigCompositeUtils::Decision *newDecision = TrigCompositeUtils::newDecisionIn( decisions.get() );
  // Link Jet Collection to decision so that I can use it in the following b-jet trigger steps (?)
  newDecision->setObjectLink( "SplitJets", ElementLink< xAOD::JetContainer >( m_jetsKey.key(),0 ) );
  ATH_MSG_DEBUG( "Linking 'SplitJets' to output decisions" );

  const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs { 
    TrigCompositeUtils::decisionIDs( prevDecision ).begin(),
      TrigCompositeUtils::decisionIDs( prevDecision ).end() 
      };

  // Decide (Hypo Tool)
  for ( const ToolHandle< TrigBjetEtHypoTool >& tool : m_hypoTools ) {
    const HLT::Identifier  decisionId = tool->getId();
    // Check previous decision is 'passed'
    if ( not TrigCompositeUtils::passed( decisionId.numeric() , previousDecisionIDs ) )
      continue;
    bool pass = false;
    CHECK( tool->decide( outputJets.get(),pass ) );
    //    CHECK( tool->decide( jetCollection,pass ) );
    if ( pass ) TrigCompositeUtils::addDecisionID( decisionId,newDecision );
  }

  // ==========================================================================================================================
  //    ** Store Output
  // ==========================================================================================================================

  // Save Output Decisions
  SG::WriteHandle< TrigCompositeUtils::DecisionContainer > handle =  SG::makeHandle( decisionOutput(), context );
  CHECK( handle.record( std::move( decisions ), std::move( aux ) ) );
  ATH_MSG_DEBUG( "Exiting with " << handle->size() << " decisions" );

  // Save Output Jet Contaienr
  SG::WriteHandle< xAOD::JetContainer > outputJetHandle = SG::makeHandle( m_outputJetsKey,context );
  ATH_MSG_DEBUG( "Saving jet collection " << m_outputJetsKey.key() << " with " << outputJets->size() << " elements " );
  CHECK( outputJetHandle.record( std::move( outputJets ), std::move( outputJetsAux ) ) );

  // Output RoI Container -- WILL CHANGE -- TMP
  SG::WriteHandle< TrigRoiDescriptorCollection > roiContainerHandle = SG::makeHandle( m_outputRoiKey,context ); // TMP
  ATH_MSG_DEBUG( "Sabing roi collection " << m_outputRoiKey.key() << " with " << roiContainer->size() <<" elements " ); // TMP
  CHECK( roiContainerHandle.record( std::move( roiContainer ) ) ); // TMP

  return StatusCode::SUCCESS;
}




