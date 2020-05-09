/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigBjetEtHypoAlgMT.h"
#include "AthViews/ViewHelper.h"
#include "CxxUtils/phihelper.h"

TrigBjetEtHypoAlgMT::TrigBjetEtHypoAlgMT( const std::string& name, 
					  ISvcLocator* pSvcLocator ) : 
  TrigBjetHypoAlgBaseMT( name, pSvcLocator ) {}


StatusCode TrigBjetEtHypoAlgMT::initialize() {

  ATH_MSG_DEBUG(  "declareProperty review:"    );
  ATH_MSG_DEBUG(  "   " << m_roiLink           );
  ATH_MSG_DEBUG(  "   " << m_prmVtxLink        );

  ATH_CHECK( m_hypoTools.retrieve()   );

  CHECK( m_inputJetsKey.initialize()       );
  CHECK( m_inputRoIKey.initialize()        );
  CHECK( m_inputPrmVtx.initialize()        );

  renounce( m_inputJetsKey );
  renounce( m_inputRoIKey );
  if ( m_retrieveVertexFromEventView == true ) renounce( m_inputPrmVtx );

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT::execute( const EventContext& context ) const {
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  // ========================================================================================================================== 
  //    ** Retrieve Ingredients
  // ========================================================================================================================== 

  // Read in previous Decisions made before running this Hypo Alg.
  // The decisions come from the Jet group
  const TrigCompositeUtils::DecisionContainer *prevDecisionContainer = nullptr;
  CHECK( retrievePreviousDecisionContainer( context,prevDecisionContainer ) );
  ATH_MSG_DEBUG( "Found a total of " << prevDecisionContainer->size() << " previous decisions!" );

  // Retrieve Jets
  // These are the jets coming from the Jet selector in which RoIs have been constructed
  // with PV constraint
  ElementLinkVector< xAOD::JetContainer > jetELs;
  CHECK( retrieveObjectFromEventView( context,jetELs,m_inputJetsKey,prevDecisionContainer ) );

  ATH_MSG_DEBUG( "Found " << jetELs.size()<< " jets."  );
  for ( const ElementLink< xAOD::JetContainer >& jetLink : jetELs ) 
    ATH_MSG_DEBUG( "   -- Jet pt=" << (*jetLink)->p4().Et() 
		   <<" eta="<< (*jetLink)->eta() 
		   << " phi="<< (*jetLink)->phi() );
  
  // The following is just for debugging. 
  // Retrieve RoI (to be linked to the output decision)
  ElementLinkVector< TrigRoiDescriptorCollection > roiELs;
  CHECK( retrieveObjectFromEventView( context,roiELs,m_inputRoIKey,prevDecisionContainer ) );
  
  ATH_MSG_DEBUG( "Found " << roiELs.size() << " rois." );
  for ( const ElementLink< TrigRoiDescriptorCollection >& roiLink : roiELs )
    ATH_MSG_DEBUG( "   -- RoI eta=" << (*roiLink)->eta() << 
		   " phi=" << (*roiLink)->phi() );

  // The following is just for debugging.
  // Retrieve PVs 
  ElementLinkVector< xAOD::VertexContainer > vertexELs;
  if ( m_retrieveVertexFromEventView == true )
    CHECK( retrieveObjectFromEventView( context,vertexELs,m_inputPrmVtx,prevDecisionContainer ) );
  else
    CHECK( retrieveObjectFromStoreGate( context,vertexELs,m_inputPrmVtx ) );

  ATH_MSG_DEBUG( "Found " << vertexELs.size() <<" vertexes." );
  for ( const ElementLink< xAOD::VertexContainer >& vertexLink : vertexELs )
    ATH_MSG_DEBUG( "  -- PV x="<<  (*vertexLink)->x() << 
		   " y=" << (*vertexLink)->y() << 
		   " z=" << (*vertexLink)->z() );

  // ========================================================================================================================== 
  //    ** Prepare Outputs
  // ========================================================================================================================== 

  // Decisions
  SG::WriteHandle< TrigCompositeUtils::DecisionContainer > handle = TrigCompositeUtils::createAndStore( decisionOutput(), context ); 
  TrigCompositeUtils::DecisionContainer *outputDecisions = handle.ptr();

  // ==========================================================================================================================
  //    ** Compute Decisions
  // ==========================================================================================================================

  ATH_MSG_DEBUG("Creating Output Decisions and Linking Stuff to it");
  std::vector< TrigCompositeUtils::Decision* > newDecisions;
  
  for ( const TrigCompositeUtils::Decision* inputDecision : *prevDecisionContainer ) {
    
    // Retrieve jet link stored in the navigation upstream and check if the b-jet code is selecting it
    // If so, create an output decision and attach the new jet to the output decision
    // If not, do not create the output decision and go to the next input decision
    ATH_MSG_DEBUG( "Retrieving input jet from the upstream code!" );
    std::vector< TrigCompositeUtils::LinkInfo< xAOD::JetContainer > > inputJetELInfo = TrigCompositeUtils::findLinks< xAOD::JetContainer >( inputDecision, TrigCompositeUtils::featureString(), TrigDefs::lastFeatureOfType );
    ATH_MSG_DEBUG( "Found a jet Link vector of size: " << inputJetELInfo.size() );
    ATH_CHECK( inputJetELInfo.size() == 1 );
    ATH_CHECK( inputJetELInfo.at(0).isValid() );

    const xAOD::Jet *inputJet = *(inputJetELInfo.at(0).link);
    ATH_MSG_DEBUG( "   -- pt=" <<  inputJet->p4().Et() <<
		   " eta=" << inputJet->eta() <<
		   " phi=" << inputJet->phi() );

    // Do geometrical matching to check if the jet is amidst the selected ones (b-jet applies ID acceptance requirements)
    int Jetindex = -1;
    double minDr = 9999.;
    
    for ( unsigned int jeti(0); jeti < jetELs.size(); jeti++ ) {
      const ElementLink< xAOD::JetContainer >& jetLink = jetELs.at( jeti );

      double deltaEta = std::abs( inputJet->eta() - (*jetLink)->eta() );
      double deltaPhi = CxxUtils::wrapToPi( inputJet->phi() - (*jetLink)->phi() );
      double dR = sqrt( pow(deltaEta,2) + pow(deltaPhi,2) );

      if ( dR < minDr && dR < 0.1 ) {
	minDr = dR;
	Jetindex = jeti;
      }
    }

    if ( Jetindex < 0 ) {
      ATH_MSG_DEBUG( "No jet matching! The jet was not selected by the b-jet code due to ID acceptance cut." );
      continue;
    } else {
      ATH_MSG_DEBUG( "The jet has been selected by the b-jet code (ID acceptance)!" );
      ATH_MSG_DEBUG( "Output decision will be created." );
    }
    // ==========================

    TrigCompositeUtils::Decision *toAdd = TrigCompositeUtils::newDecisionIn( outputDecisions,
                                                                             inputDecision,
                                                                             "", context );

    // Retrieve now the primary vertex to be attached to the decision.
    // This is only one, but the container has size > 1!
    int PVindex = -1;

    for ( unsigned int pvi(0); pvi < vertexELs.size(); pvi++ ) {
      const ElementLink< xAOD::VertexContainer >& vertexLink = vertexELs.at( pvi );
      if ( (*vertexLink)->vertexType() != xAOD::VxType::VertexType::PriVtx ) continue;
      PVindex = pvi;
      break;
    }

    if ( PVindex == -1 ) {
      ATH_MSG_DEBUG( "Primary Vertex could not be found! Using dummy vertex!" );
      PVindex = 0;
    }
    // ========================== 

    // Adding Links 
    CHECK( attachLinksToDecision( context,*toAdd,Jetindex,PVindex ) ); 
    newDecisions.push_back( toAdd );    
  }

  ATH_MSG_DEBUG( "Received " << prevDecisionContainer->size()  << " input decisions from upstream!" );
  ATH_MSG_DEBUG( "Created " << newDecisions.size() << " output decisions!" );
  
  // ==========================================================================================================================
  //    ** Prepare input to Hypo Tools
  // ========================================================================================================================== 

  std::vector< TrigBjetEtHypoTool::TrigBjetEtHypoToolInfo > bJetHypoInputs;

  for ( TrigCompositeUtils::Decision* newDecision : newDecisions ) {
    // Check navigation is set properly
    if ( not newDecision->hasObjectCollectionLinks( TrigCompositeUtils::seedString() ) ) {
      ATH_MSG_ERROR( "Trying to extract the IDs of the b-jet chains which are active." );
      ATH_MSG_ERROR( "But output decision has no link to seed collection." );
      return StatusCode::FAILURE;
    }
    // Check jets are attached properly
    if ( not newDecision->hasObjectLink( TrigCompositeUtils::featureString() ) ) {
      ATH_MSG_ERROR( "Trying to extract the associated jet from the output decisions." );
      ATH_MSG_ERROR( "But output decision has no link to jet." );
      return StatusCode::FAILURE;
    }
    // Check vertex is attached properly
    if ( not newDecision->hasObjectLink( m_prmVtxLink.value() ) ) {
      ATH_MSG_ERROR( "Trying to extract the associated primary vertex from the output decisions." );
      ATH_MSG_ERROR( "But output decision has no link to primary vertex" );
      return StatusCode::FAILURE;
    }    

    // Get Parent Decision
    const ElementLinkVector< TrigCompositeUtils::DecisionContainer > mySeeds = newDecision->objectCollectionLinks< TrigCompositeUtils::DecisionContainer >( TrigCompositeUtils::seedString() );
    // Get Associated Jet
    const ElementLink< xAOD::JetContainer > myJet = newDecision->objectLink< xAOD::JetContainer >( TrigCompositeUtils::featureString() ); 
    CHECK( myJet.isValid() );
    // Get primary vertex
    const ElementLink< xAOD::VertexContainer > myVertex = newDecision->objectLink< xAOD::VertexContainer >( m_prmVtxLink.value() );
    CHECK( myVertex.isValid() );

    // Extract the IDs of the b-jet chains which are active.
    // Previous decision IDs.
    const TrigCompositeUtils::Decision *previousDecision = *mySeeds.back();

    TrigCompositeUtils::DecisionIDContainer previousDecisionIDs {
      TrigCompositeUtils::decisionIDs( previousDecision ).begin(),
	TrigCompositeUtils::decisionIDs( previousDecision ).end()
	};

    TrigBjetEtHypoTool::TrigBjetEtHypoToolInfo infoToAdd;
    infoToAdd.previousDecisionIDs = previousDecisionIDs;
    infoToAdd.jetEL = myJet; 
    infoToAdd.vertexEL = myVertex;
    infoToAdd.decision = newDecision;
    bJetHypoInputs.push_back( infoToAdd );
  }

  // ========================================================================================================================== 

  // Run on Trigger Chains
  for ( const ToolHandle< TrigBjetEtHypoTool >& tool : m_hypoTools ) 
    CHECK( tool->decide( bJetHypoInputs ) );
  
  ATH_MSG_DEBUG( "Exiting with " << handle->size() << " decisions" );
  ATH_MSG_DEBUG( "CHECKING OUTPUT DECISION HANDLE" );
  CHECK( hypoBaseOutputProcessing(handle) );

  // ==========================================================================================================================  

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT::attachLinksToDecision( const EventContext& context,
						       TrigCompositeUtils::Decision& toAdd,
						       int index,
						       int indexPrmVertex ) const {

  // Do the linking...
  // Link Jet
  CHECK( attachObjectLinkToDecisionFromEventView( context,
						  toAdd,
						  m_inputJetsKey,
						  TrigCompositeUtils::featureString(),
						  index ) );

  // Link RoI
  CHECK( attachObjectLinkToDecisionFromEventView( context,
						  toAdd,
						  m_inputRoIKey,
						  m_roiLink.value(),
						  index ) );

  // Link Vertex
  if ( m_retrieveVertexFromEventView )
    CHECK( attachObjectLinkToDecisionFromEventView( context,
						    toAdd,
						    m_inputPrmVtx,
						    m_prmVtxLink.value(),
						    indexPrmVertex ) );
  else
    CHECK( attachObjectLinkToDecisionFromStoreGate( toAdd,
						    m_inputPrmVtx,
						    m_prmVtxLink.value(),
						    indexPrmVertex ) );

  ATH_MSG_DEBUG("   ** Added object links to output decision");
  return StatusCode::SUCCESS;
}

