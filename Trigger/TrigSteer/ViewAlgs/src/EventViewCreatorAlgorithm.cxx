/*
  General-purpose view creation algorithm <bwynne@cern.ch>
  
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EventViewCreatorAlgorithm.h"
#include "AthLinks/ElementLink.h"
#include "AthViews/ViewHelper.h"
#include "AthViews/View.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include <sstream>

using namespace TrigCompositeUtils;

EventViewCreatorAlgorithm::EventViewCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
: InputMakerBase( name, pSvcLocator ) {}

EventViewCreatorAlgorithm::~EventViewCreatorAlgorithm(){}

StatusCode EventViewCreatorAlgorithm::initialize() {
  ATH_MSG_DEBUG("Will produce views=" << m_viewsKey << " roIs=" << m_inViewRoIs );
  ATH_CHECK( m_viewsKey.initialize() );
  ATH_CHECK( m_inViewRoIs.initialize() );
  ATH_CHECK( m_roiTool.retrieve() );
  ATH_CHECK( m_cachedViewsKey.initialize(SG::AllowEmpty) );
  return StatusCode::SUCCESS;
}


StatusCode EventViewCreatorAlgorithm::execute( const EventContext& context ) const {

   // create the output decisions from the input collections
  ATH_MSG_DEBUG("Starting to merge " << decisionInputs().size() << " inputs to the " << decisionOutputs().key() << " output.");
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore( decisionOutputs(), context );
  ATH_CHECK(outputHandle.isValid());
  ATH_CHECK(decisionInputToOutput(context, outputHandle));
  ATH_MSG_DEBUG("Merging complete");

  // make the views
  auto viewsHandle = SG::makeHandle( m_viewsKey, context ); 
  ATH_CHECK( viewsHandle.record( std::make_unique<ViewContainer>() ) );
  auto viewVector = viewsHandle.ptr();

  // Check for an optional input handle to use as a source of cached, already-executed, views.
  const DecisionContainer* cachedViews = nullptr;
  if (!m_cachedViewsKey.empty()) {
    SG::ReadHandle<DecisionContainer> cachedRH = SG::makeHandle(m_cachedViewsKey, context);
    ATH_CHECK(cachedRH.isValid());
    cachedViews = cachedRH.ptr();
  }

  // Keep track of the ROIs we spwan a View for, do not spawn duplicates.
  // For many cases, this will be covered by the Merging operation preceding this.
  ElementLinkVector<TrigRoiDescriptorCollection> RoIsFromDecision;

  if( outputHandle->size() == 0) {
    ATH_MSG_WARNING( "Have no decisions in output handle "<< outputHandle.key() << ". Handle is valid but container is empty. "
      << "Check why this EventViewCreatorAlgorithm was unlocked by a Filter, if the Filter then gave it no inputs.");
  } else {
    ATH_MSG_DEBUG( "Have output " << outputHandle.key() << " with " << outputHandle->size() << " elements" );
  }

  // Find and link to the output Decision objects the ROIs to run over
  ATH_CHECK( m_roiTool->attachROILinks(*outputHandle, context) );

  for ( Decision* outputDecision : *outputHandle ) { 

    if (!outputDecision->hasObjectLink(roiString(), ClassID_traits<TrigRoiDescriptorCollection>::ID())) {
      ATH_MSG_ERROR("No '" << roiString() << "'link was attached by the ROITool. Decision object dump:" << *outputDecision);
      return StatusCode::FAILURE;
    }
    const ElementLink<TrigRoiDescriptorCollection> roiEL = outputDecision->objectLink<TrigRoiDescriptorCollection>(roiString());
    ATH_CHECK(roiEL.isValid());

    // We do one of three things here, either... 
    // a) We realise that an identically configured past EVCA has already run a View on an equivilant ROI. If so we can re-use this.
    // b) We encounter a new ROI and hence need to spawn a new view.
    // c) We encounter a ROI that we have already seen in looping over this outputHandle, we can re-use a view.
      
    // cachedIndex and useCached are to do with a)
    size_t cachedIndex = std::numeric_limits<std::size_t>::max();
    const bool useCached = checkCache(cachedViews, outputDecision, cachedIndex);

    // roiIt is to do with b) and c)
    auto roiIt = find(RoIsFromDecision.begin(), RoIsFromDecision.end(), roiEL);

    if (useCached) {

      // Re-use an aready processed view from a previously executed EVCA instance
      const Decision* cached = cachedViews->at(cachedIndex);
      ElementLink<ViewContainer> cachedViewEL = cached->objectLink<ViewContainer>(viewString());
      ElementLink<TrigRoiDescriptorCollection> cachedROIEL = cached->objectLink<TrigRoiDescriptorCollection>(roiString());
      ATH_CHECK(cachedViewEL.isValid());
      ATH_CHECK(cachedROIEL.isValid());
      ATH_MSG_DEBUG("Re-using cached existing view from " << cachedViewEL.dataID() << ", index:" << cachedViewEL.index() 
        << " on ROI " << **cachedROIEL);
      outputDecision->setObjectLink( viewString(), cachedViewEL );
      outputDecision->setObjectLink( roiString(), cachedROIEL );
      // Note: This overwrites the link created in the above tool with what should be a spatially identical ROI (check?)

    } else if ( roiIt == RoIsFromDecision.end() ) {

      // We have not yet spawned an ROI on this View. Do it now.
      RoIsFromDecision.push_back(roiEL);
      ATH_MSG_DEBUG("Found RoI:" << **roiEL << " FS=" << (*roiEL)->isFullscan() << ". Making View.");
      SG::View* newView = ViewHelper::makeView( name()+"_view", viewVector->size() /*view counter*/, m_viewFallThrough );
      viewVector->push_back( newView );
      // Use a fall-through filter if one is provided
      if ( m_viewFallFilter.size() ) {
        newView->setFilter( m_viewFallFilter );
      }
      // Set parent view, if required. Note: Must be called before we link the new view to outputDecision.
      ATH_CHECK(linkViewToParent(outputDecision, newView));
      // Add the single ROI into the view to seed it.
      ATH_CHECK(placeRoIInView(roiEL, viewVector->back(), context));
      // Link the view to the Decision object
      outputDecision->setObjectLink( viewString(), ElementLink<ViewContainer>(m_viewsKey.key(), viewVector->size()-1 ));
      ATH_MSG_DEBUG( "Made new View, storing view in viewVector " << m_viewsKey.key() << " index:" << viewVector->size()-1 );

    } else {

      // We have already spawned a ROI in this View. Link it here too.
      const size_t existingIndex = std::distance(RoIsFromDecision.begin(), roiIt);
      ATH_MSG_DEBUG("Found existing View, stored in view in viewVector " << m_viewsKey.key() << " index:" << existingIndex );
      outputDecision->setObjectLink( viewString(), ElementLink<ViewContainer>(m_viewsKey.key(), existingIndex )); //adding View link to Decision

    }
  } // loop over output decisions   

  // launch view execution
  ATH_MSG_DEBUG( "Launching execution in " << viewVector->size() << " unique views" );
  ATH_CHECK( ViewHelper::scheduleViews( viewVector, // Vector containing views
    m_viewNodeName,                                 // CF node to attach views to
    context,                                        // Source context
    getScheduler(),                                 // Scheduler to launch with
    m_reverseViews ) );                             // Debug option
  
  if (msgLvl(MSG::DEBUG)) {
    debugPrintOut(context, outputHandle);
  }
  return StatusCode::SUCCESS;
}

bool EventViewCreatorAlgorithm::checkCache(const DecisionContainer* cachedViews, const Decision* outputDecision, size_t& cachedIndex) const {
  if (cachedViews == nullptr) {
    return false; // No cached input configured, which is fine.
  }
  return matchInCollection(cachedViews, outputDecision, cachedIndex);
}


StatusCode EventViewCreatorAlgorithm::linkViewToParent( const TrigCompositeUtils::Decision* outputDecision, SG::View* newView ) const {
  if (!m_requireParentView) {
    ATH_MSG_DEBUG("Parent view linking not required");
    return StatusCode::SUCCESS;
  }
  // We must call this BEFORE having added the new link, check
  if (outputDecision->hasObjectLink(viewString())) {
    ATH_MSG_ERROR("Called linkViewToParent on a Decision object which already has been given a '" 
      << viewString << "' link. Call this fn BEFORE linking the new View.");
    return StatusCode::FAILURE;
  }
  std::vector<LinkInfo<ViewContainer>> parentViews = findLinks<ViewContainer>(outputDecision, viewString(), TrigDefs::lastFeatureOfType);
  if (parentViews.size() == 0) {
    ATH_MSG_ERROR("Could not find the parent View, but 'RequireParentView' is true.");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Will link " << parentViews.size() << " parent view(s)" );
  for (const LinkInfo<ViewContainer>& parentViewLI : parentViews) {
    ATH_CHECK(parentViewLI.isValid());
    newView->linkParent( *(parentViewLI.link) );
    ATH_MSG_DEBUG( "Parent view linked (" << parentViewLI.link.dataID() << ", index:" << parentViewLI.link.index() << ")" );
  }
  return StatusCode::SUCCESS;
}

StatusCode EventViewCreatorAlgorithm::placeRoIInView( const ElementLink<TrigRoiDescriptorCollection>& roiEL, SG::View* view, const EventContext& context ) const {
  // fill the RoI output collection
  auto oneRoIColl = std::make_unique< ConstDataVector<TrigRoiDescriptorCollection> >();
  oneRoIColl->clear( SG::VIEW_ELEMENTS ); //Don't delete the RoIs
  oneRoIColl->push_back( *roiEL );

  view->setROI(roiEL);

  //store the RoI in the view
  auto handle = SG::makeHandle( m_inViewRoIs, context );
  ATH_CHECK( handle.setProxyDict( view ) );
  ATH_CHECK( handle.record( std::move( oneRoIColl ) ) );
  return StatusCode::SUCCESS;
}
