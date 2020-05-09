
/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthViews/ViewHelper.h"
#include "ViewCreatorFetchFromViewROITool.h"

using namespace TrigCompositeUtils;

ViewCreatorFetchFromViewROITool::ViewCreatorFetchFromViewROITool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent) 
  {}


StatusCode ViewCreatorFetchFromViewROITool::initialize() {
  ATH_CHECK(m_roisWriteHandleKey.initialize());
  ATH_CHECK(m_inViewRoIKey.initialize());
  renounce(m_inViewRoIKey); // Will be read from in-View
  return StatusCode::SUCCESS;
}


StatusCode ViewCreatorFetchFromViewROITool::attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const {
  SG::WriteHandle<TrigRoiDescriptorCollection> roisWriteHandle = createAndStoreNoAux(m_roisWriteHandleKey, ctx);
  
  for ( Decision* outputDecision : decisions ) { 
    const std::vector<LinkInfo<ViewContainer>> myView = findLinks<ViewContainer>(outputDecision, viewString(), TrigDefs::lastFeatureOfType);
    
    if (myView.size() != 1) {
      ATH_MSG_ERROR("Did not find exactly one most-recent '" << viewString() << "' for Decision object index " << outputDecision->index()
        << ", found " << myView.size());
      if (myView.size() > 1) {
        ATH_MSG_ERROR("Was this Decision Object was merged after having followed different reconstruction paths in previous Steps?");
        ATH_MSG_ERROR("Need more information about which of these Views to look in to find the desired '" << m_inViewRoIKey.key() << "' TrigRoiDescriptorCollection");
      }
      return StatusCode::FAILURE;
    }
    ATH_CHECK(myView.at(0).isValid());

    SG::ReadHandle<TrigRoiDescriptorCollection> roiReadHandle = ViewHelper::makeHandle(*myView.at(0).link, m_inViewRoIKey, ctx);
    ATH_CHECK(roiReadHandle.isValid());

    if (roiReadHandle->size() != 1) {
      ATH_MSG_ERROR("In-view ROI collection '" << m_inViewRoIKey.key() << "' for Decision object index " << outputDecision->index()
        << " has " << roiReadHandle->size() << " TrigRoiDescriptor inside. We require exactly one.");
      return StatusCode::FAILURE;
    }

    // This ROI was created in a View, it cannot currently be written out.
    // Make a copy in the full-event context, which can be recorded.
    roisWriteHandle->push_back( new TrigRoiDescriptor( *(roiReadHandle->at(0)) ) );
    const ElementLink<TrigRoiDescriptorCollection> newRoiEL = ElementLink<TrigRoiDescriptorCollection>(*roisWriteHandle, roisWriteHandle->size() - 1, ctx);

    outputDecision->setObjectLink(roiString(), newRoiEL);
  }

	return StatusCode::SUCCESS;
}
