/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthViews/ViewHelper.h"
#include "CxxUtils/phihelper.h"
#include "ViewCreatorCentredOnJetWithPVConstraintROITool.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

ViewCreatorCentredOnJetWithPVConstraintROITool::ViewCreatorCentredOnJetWithPVConstraintROITool( const std::string& type, 
												const std::string& name, 
												const IInterface* parent)
  : base_class(type, name, parent)
{}


StatusCode ViewCreatorCentredOnJetWithPVConstraintROITool::initialize()  {
  ATH_CHECK( m_roisWriteHandleKey.initialize() );
  ATH_CHECK( m_vertexReadHandleKey.initialize() );

  // FIXME: We have to renounce to the Vertex RedHandleKey due to problems in the scheduler's sanity checks 
  // 'Strongly connected components found in DF realm' (see ATR-21298)
  // Short term way of avoiding the issue
  renounce( m_vertexReadHandleKey );

  return StatusCode::SUCCESS;
}


StatusCode ViewCreatorCentredOnJetWithPVConstraintROITool::attachROILinks( TrigCompositeUtils::DecisionContainer& decisions, 
										 const EventContext& ctx ) const {
  
  // ===================================================================================== // 
  // Retrieve Primary Vertex... only 1 in the event
  SG::ReadHandle< xAOD::VertexContainer > vertexContainerHandle = SG::makeHandle( m_vertexReadHandleKey,ctx );
  ATH_CHECK( vertexContainerHandle.isValid() );

  const xAOD::VertexContainer *vertexContainer = vertexContainerHandle.get();
  ATH_MSG_DEBUG( "I'm working with " << vertexContainer->size() << " vertices" );

  const xAOD::Vertex* primaryVertex = getPrimaryVertex( vertexContainer );
  if ( primaryVertex == nullptr ) {
    ATH_MSG_ERROR( "No primary vertex has been found for this event!" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "  ** PV = (" << primaryVertex->x() <<
                 "," << primaryVertex->y() <<
                 "," << primaryVertex->z() << ")" );

  // ===================================================================================== //                                                                                                                                       
  // ===================================================================================== // 
  // Create output RoI collection

  SG::WriteHandle<TrigRoiDescriptorCollection> roisWriteHandle =  TrigCompositeUtils::createAndStoreNoAux(m_roisWriteHandleKey, ctx);
  // ===================================================================================== // 

  for ( TrigCompositeUtils::Decision* outputDecision : decisions ) { 
    const std::vector< TrigCompositeUtils::LinkInfo< xAOD::JetContainer >> myFeature = TrigCompositeUtils::findLinks< xAOD::JetContainer >(outputDecision, TrigCompositeUtils::featureString(), TrigDefs::lastFeatureOfType);
    
    if (myFeature.size() != 1) {
      ATH_MSG_ERROR("Did not find exactly one most-recent xAOD::Jet '" << TrigCompositeUtils::featureString() << "' for Decision object index " << outputDecision->index()
		    << ", found " << myFeature.size());
      return StatusCode::FAILURE;
    }
    ATH_CHECK(myFeature.at(0).isValid());
    
    const xAOD::Jet *myJet = *(myFeature.at(0).link);
    double jetPt = myJet->p4().Et();
    double jetEta = myJet->eta();
    double jetPhi = myJet->phi();

    ATH_MSG_DEBUG("JET  -- pt=" << jetPt <<
                 " eta=" << jetEta <<
                 " phi=" << jetPhi );

    // create ROIs
    ATH_MSG_DEBUG("Creating RoI");
    ATH_MSG_DEBUG( "  ** Imposing Z constraint while building RoI" );
    double etaMinus = jetEta - m_roiEtaWidth;
    double etaPlus  = jetEta + m_roiEtaWidth;
    
    double phiMinus = CxxUtils::wrapToPi( jetPhi - m_roiPhiWidth );
    double phiPlus  = CxxUtils::wrapToPi( jetPhi + m_roiPhiWidth );

    double zMinus = primaryVertex->z() - m_roiZWidth;
    double zPlus  = primaryVertex->z() + m_roiZWidth;

    TrigRoiDescriptor *newROI = new TrigRoiDescriptor( jetEta,etaMinus, etaPlus,
                                                       jetPhi, phiMinus, phiPlus,
						       primaryVertex->z(),zMinus,zPlus );

    roisWriteHandle->push_back( newROI );
    const ElementLink< TrigRoiDescriptorCollection > roiEL = ElementLink< TrigRoiDescriptorCollection >( *roisWriteHandle, roisWriteHandle->size() - 1, ctx );

    outputDecision->setObjectLink( TrigCompositeUtils::roiString(), roiEL );
    outputDecision->setObjectLink( m_prmVtxLink.value(),
				   ElementLink< xAOD::VertexContainer >( m_vertexReadHandleKey.key(),primaryVertex->index() ) );

    ATH_MSG_DEBUG("PRINTING DECISION");
    ATH_MSG_DEBUG( *outputDecision );
  }

  return StatusCode::SUCCESS;
}

const xAOD::Vertex* ViewCreatorCentredOnJetWithPVConstraintROITool::getPrimaryVertex( const xAOD::VertexContainer* vertexContainer ) const {
  // In case we need more complex selection
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
  ATH_MSG_DEBUG( "Using dummy vertex!" );
  return vertexContainer->at(0);
}
