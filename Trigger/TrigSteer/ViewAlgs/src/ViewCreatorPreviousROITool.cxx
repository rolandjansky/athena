
/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ViewCreatorPreviousROITool.h"

using namespace TrigCompositeUtils;

ViewCreatorPreviousROITool::ViewCreatorPreviousROITool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent) 
  {}

StatusCode ViewCreatorPreviousROITool::attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext&) const {
  // Locate "roi" for each Decision object from previous step, re-attach the ElementLink as "roi" to the current Decision object.
  // I.e. don't update the "roi", just re-use the one from the previous Step.
  for ( Decision* outputDecision : decisions ) { 
    const std::vector<LinkInfo<TrigRoiDescriptorCollection>> myROI = findLinks<TrigRoiDescriptorCollection>(outputDecision, roiString(), TrigDefs::lastFeatureOfType);

    if (myROI.size() != 1) {
      ATH_MSG_ERROR("Did not find exactly one '" << roiString() << "' for Decision object index " << outputDecision->index()
        << ", found " << myROI.size() << ". Unable to supply single ROI to spawn EventView on.");
      for (const auto& li : myROI) {
        ATH_MSG_ERROR("  -- " << li.link.dataID() << ":" << li.link.index() << ". Dump:" << *(li.source)); 
      }
      return StatusCode::FAILURE;
    }

    outputDecision->setObjectLink(roiString(), myROI.at(0).link);
  }
	return StatusCode::SUCCESS;
}
