/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///
/// General-purpose view creation algorithm <bwynne@cern.ch>
///

#include "EventViewCreatorAlgorithmWithMuons.h"
#include "AthLinks/ElementLink.h"
#include "AthViews/ViewHelper.h"
#include "AthViews/View.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HLTIdentifier.h"

using namespace TrigCompositeUtils;

EventViewCreatorAlgorithmWithMuons::EventViewCreatorAlgorithmWithMuons( const std::string& name, ISvcLocator* pSvcLocator )
  : EventViewCreatorAlgorithm( name, pSvcLocator ) {

  declareProperty("RoIEtaWidth", m_roiEtaWidth=0.1);
  declareProperty("RoIPhiWidth", m_roiPhiWidth=0.1);
}

EventViewCreatorAlgorithmWithMuons::~EventViewCreatorAlgorithmWithMuons() {}

StatusCode EventViewCreatorAlgorithmWithMuons::initialize() {

  ATH_CHECK( EventViewCreatorAlgorithm::initialize() );
  ATH_CHECK( m_inViewMuons.initialize() );
  ATH_CHECK( m_inViewMuonCandidates.initialize() );
  ATH_CHECK( m_roisWriteHandleKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode EventViewCreatorAlgorithmWithMuons::execute( const EventContext& context ) const {
  // create the output decisions, similar to inputs (copy basic links)
  std::vector< SG::WriteHandle<TrigCompositeUtils::DecisionContainer> > outputHandles;
  ATH_CHECK (decisionInputToOutput(context, outputHandles));

  // make the views
  auto viewsHandle = SG::makeHandle( m_viewsKey, context ); 
  auto viewVector1 = std::make_unique< ViewContainer >();
  ATH_CHECK( viewsHandle.record(  std::move( viewVector1 ) ) );
  auto viewVector = viewsHandle.ptr();

  auto contexts = std::vector<EventContext>( );
  unsigned int viewCounter = 0;
  unsigned int conditionsRun = Atlas::getExtendedEventContext(context).conditionsRun();

  //map all RoIs that are stored
  std::vector <ElementLink<TrigRoiDescriptorCollection> > RoIsFromDecision;

  // Store new ROI Descriptors
  SG::WriteHandle<TrigRoiDescriptorCollection> roisWriteHandle = TrigCompositeUtils::createAndStoreNoAux(m_roisWriteHandleKey, context);

  for (auto outputHandle: outputHandles) {
    if( not outputHandle.isValid() ) {
      ATH_MSG_DEBUG( "Got no decisions from output "<< outputHandle.key() << " because handle not valid");
      continue;
    }
    if( outputHandle->size() == 0){ // input filtered out
      ATH_MSG_ERROR( "Got no decisions from output "<< outputHandle.key()<<": handle is valid but container is empty. Is this expected?");
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG( "Got output "<< outputHandle.key()<<" with " << outputHandle->size() << " elements" );

    if(m_doFSRoI){	
      //create one RoI over full detector
      auto roi = new TrigRoiDescriptor(true);
      roisWriteHandle->push_back( roi );
      const auto roiEL = ElementLink<TrigRoiDescriptorCollection>(*roisWriteHandle, roisWriteHandle->size() - 1, context);

      // make the view
      ATH_MSG_DEBUG( "Making the View "<<name()<<"_view" );
      auto newView = ViewHelper::makeView( name()+"_view", viewCounter++, m_viewFallThrough );
      viewVector->push_back( newView );
      contexts.emplace_back( context );
      Atlas::setExtendedEventContext (contexts.back(),
				      Atlas::ExtendedEventContext( viewVector->back(), conditionsRun, roi ) );
      ATH_CHECK( placeRoIInView( roiEL, viewVector->back(), contexts.back() ) );
    }

    // loop over output decisions in container of outputHandle, follow link to inputDecision
    for ( auto outputDecision : *outputHandle){ 
      ElementLinkVector<DecisionContainer> inputLinks = getLinkToPrevious(outputDecision);

      if(m_doFSRoI){	
	// link decision to this view
	outputDecision->setObjectLink( "view", ElementLink< ViewContainer >(m_viewsKey.key(), viewVector->size()-1 ));//adding view to TC
      }
      else{

	//create one RoI per muon

	// loop over input links as predecessors
	for (auto input: inputLinks){
	  const Decision* inputDecision = *input;

	  // Retrieve muons ...
	  ATH_MSG_DEBUG( "Checking there are muons linked to decision object" );
	  TrigCompositeUtils::LinkInfo< xAOD::MuonContainer > muonELInfo = TrigCompositeUtils::findLink< xAOD::MuonContainer >( inputDecision,m_muonsLink );
	  ATH_CHECK( muonELInfo.isValid() );
	  const xAOD::Muon *muon = *muonELInfo.link;
	  ATH_MSG_DEBUG( "Placing xAOD::MuonContainer " );
	  ATH_MSG_DEBUG( "   -- pt="<< muon->p4().Et() <<" eta="<< muon->eta() << " muon="<< muon->phi() );

	  // create the RoI around muon
	  auto roi = new TrigRoiDescriptor(muon->eta(), muon->eta()-m_roiEtaWidth, muon->eta()+m_roiEtaWidth, muon->phi(), muon->phi()-m_roiPhiWidth, muon->phi()+m_roiPhiWidth);
	  roisWriteHandle->push_back( roi );
	  const auto roiEL = ElementLink<TrigRoiDescriptorCollection>(*roisWriteHandle, roisWriteHandle->size() - 1, context);
	  ATH_CHECK( roiEL.isValid() );
	  ATH_MSG_DEBUG("Created roi around muon: "<<*roi);
	  // make the view
	  ATH_MSG_DEBUG( "Making the View "<<name()<<"_view" );
	  auto newView = ViewHelper::makeView( name()+"_view", viewCounter++, m_viewFallThrough ); //pointer to the view
	  viewVector->push_back( newView );
	  contexts.emplace_back( context );
	  Atlas::setExtendedEventContext (contexts.back(),
					  Atlas::ExtendedEventContext( viewVector->back(), conditionsRun, roi ) );
	  // link decision to this view
	  outputDecision->setObjectLink( TrigCompositeUtils::viewString(), ElementLink< ViewContainer >(m_viewsKey.key(), viewVector->size()-1 ));//adding view to TC
	  ATH_MSG_DEBUG( "Adding new view to new decision; storing view in viewVector component " << viewVector->size()-1 );
	  ATH_CHECK( linkViewToParent( inputDecision, viewVector->back() ) );
	  ATH_CHECK( placeRoIInView( roiEL, viewVector->back(), contexts.back() ) );
	  ATH_CHECK( placeMuonInView( muon, viewVector->back(), contexts.back() ) );
	}// loop over previous inputs
      }//Not FS view
    } // loop over decisions   
  }// loop over output keys

  ATH_MSG_DEBUG( "Launching execution in " << viewVector->size() << " views" );
  ATH_CHECK( ViewHelper::ScheduleViews( viewVector,           // Vector containing views
                                        m_viewNodeName,       // CF node to attach views to
                                        context,              // Source context
                                        getScheduler(),
                                        m_reverseViews ) );

  if (msgLvl(MSG::DEBUG)) debugPrintOut(context, outputHandles);
  return StatusCode::SUCCESS;
}

StatusCode EventViewCreatorAlgorithmWithMuons::placeMuonInView( const xAOD::Muon* theObject, SG::View* view, const EventContext& context ) const {
  // fill the Muon output collection  
  ATH_MSG_DEBUG( "Adding Muon To View : " << m_inViewMuons.key()<<" and "<<m_inViewMuonCandidates.key() );
  auto oneObjectCollection = std::make_unique< ConstDataVector< xAOD::MuonContainer > >();
  oneObjectCollection->clear( SG::VIEW_ELEMENTS ); 
  oneObjectCollection->push_back( theObject );

  auto muonCandidate = std::make_unique< ConstDataVector< MuonCandidateCollection > >();
  muonCandidate->clear( SG::VIEW_ELEMENTS ); 
  auto msLink = theObject->muonSpectrometerTrackParticleLink();
  auto extTrackLink = theObject->extrapolatedMuonSpectrometerTrackParticleLink();
  if(msLink.isValid() && extTrackLink.isValid()) muonCandidate->push_back( new MuonCombined::MuonCandidate(msLink, (*extTrackLink)->trackLink()) );

  //store in the view 
  auto handle = SG::makeHandle( m_inViewMuons,context );
  ATH_CHECK( handle.setProxyDict( view ) );
  ATH_CHECK( handle.record( std::move( oneObjectCollection ) ) ); 

  auto handleCandidate = SG::makeHandle( m_inViewMuonCandidates,context );
  ATH_CHECK( handleCandidate.setProxyDict( view ) );
  ATH_CHECK( handleCandidate.record( std::move( muonCandidate ) ) ); 

  return StatusCode::SUCCESS;
}

