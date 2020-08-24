
/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ViewCreatorInitialROITool.h"

using namespace TrigCompositeUtils;

ViewCreatorInitialROITool::ViewCreatorInitialROITool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent) 
  {}

StatusCode ViewCreatorInitialROITool::attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext&) const {
  // Locate "initialRoI" for each Decision object, re-attach the ElementLink as "roi" to the current Decision object.
  for ( Decision* outputDecision : decisions ) { 
    const std::vector<LinkInfo<TrigRoiDescriptorCollection>> myROI = findLinks<TrigRoiDescriptorCollection>(outputDecision, initialRoIString(), TrigDefs::lastFeatureOfType);

    if (myROI.size() != 1) {
      ATH_MSG_ERROR("Did not find exactly one '" << initialRoIString() << "' for Decision object index " << outputDecision->index()
        << ", found " << myROI.size() << ". Unable to supply single ROI to spawn EventView on.");
      for (const auto& li : myROI) {
        ATH_MSG_ERROR("  -- " << li.link.dataID() << ":" << li.link.index() << ". Dump:" << *(li.source)); 
      }
    }

    outputDecision->setObjectLink(roiString(), myROI.at(0).link);
  }
	return StatusCode::SUCCESS;
}
