
/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ViewCreatorNamedROITool.h"

using namespace TrigCompositeUtils;

ViewCreatorNamedROITool::ViewCreatorNamedROITool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent) 
  {}

StatusCode ViewCreatorNamedROITool::attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext&) const {
  // Locate "NamedRoI" for each Decision object, re-attach the ElementLink as "roi" to the current Decision object.
  for ( Decision* outputDecision : decisions ) { 
    const std::vector<LinkInfo<TrigRoiDescriptorCollection>> myROI = findLinks<TrigRoiDescriptorCollection>(outputDecision, m_roiLinkName, TrigDefs::lastFeatureOfType);

    if (myROI.size() != 1) {
      ATH_MSG_ERROR("Did not find exactly one '" << m_roiLinkName << "' for Decision object index " << outputDecision->index()
        << ", found " << myROI.size() << ". Unable to supply single ROI to spawn EventView on.");
    }

    outputDecision->setObjectLink(roiString(), myROI.at(0).link);
  }
	return StatusCode::SUCCESS;
}
