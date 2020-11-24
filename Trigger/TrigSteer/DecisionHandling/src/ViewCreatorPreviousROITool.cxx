
/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ViewCreatorPreviousROITool.h"

#include <map>

using namespace TrigCompositeUtils;

ViewCreatorPreviousROITool::ViewCreatorPreviousROITool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent) 
  {}

StatusCode ViewCreatorPreviousROITool::initialize()  {
  ATH_CHECK(m_roisWriteHandleKey.initialize( SG::AllowEmpty ));
  return StatusCode::SUCCESS;
}

StatusCode ViewCreatorPreviousROITool::attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const {
  // Locate "roi" for each Decision object from previous step, re-attach the ElementLink as "roi" to the current Decision object.
  // I.e. don't update the "roi", just re-use the one from the previous Step.

  for ( Decision* outputDecision : decisions ) { 
    const std::vector<LinkInfo<TrigRoiDescriptorCollection>> myROIs = findLinks<TrigRoiDescriptorCollection>(outputDecision, roiString(), TrigDefs::lastFeatureOfType);

    int location = -1;

    if (myROIs.size() == 0) {
      ATH_MSG_ERROR("Found zero '" << roiString() << "' for Decision object index " << outputDecision->index());
    } else if (myROIs.size() == 1) {
      location = 0;
    } else if (not m_roiSGKey.empty()) { // Note: Also requires that size be > 1
      for (size_t i = 0; i < myROIs.size(); ++i) {
        if (myROIs.at(i).link.dataID() == m_roiSGKey) {
          ATH_MSG_DEBUG("Found " << myROIs.size() << " RoI links. However, successfully resolved the ambiguity using roiSGKey=" << m_roiSGKey << ", taking index " << location);
          location = i;
          break;
        }
      }
    }

    if (location == -1) {
      ATH_MSG_ERROR("Did not find exactly one '" << roiString() << "' for Decision object index " << outputDecision->index()
        << ", found " << myROIs.size() << ". Unable to supply single ROI to spawn EventView on.");
      for (const auto& li : myROIs) {
        ATH_MSG_ERROR("  -- " << li.link.dataID() << ":" << li.link.index() << ". Dump:" << *(li.source)); 
      }
      return StatusCode::FAILURE;
    }

    outputDecision->setObjectLink(roiString(), myROIs.at(location).link);
  }

  // If this property is set, we actually want to copy the ROIs into a new container. For disambiguation purposes
  if (!m_roisWriteHandleKey.empty()) {
    std::map<ElementLink<TrigRoiDescriptorCollection>, ElementLink<TrigRoiDescriptorCollection>> oldToNew;
    SG::WriteHandle<TrigRoiDescriptorCollection> roisWriteHandle = createAndStoreNoAux(m_roisWriteHandleKey, ctx);

    for ( Decision* outputDecision : decisions ) { 

      // Only copy each unique ROI once into the output collection.
      ElementLink<TrigRoiDescriptorCollection> oldRoI = outputDecision->objectLink<TrigRoiDescriptorCollection>(roiString());
      ElementLink<TrigRoiDescriptorCollection> newRoI;
      if (oldToNew.count(oldRoI) == 1) {
        newRoI = oldToNew[oldRoI];
      } else {
        roisWriteHandle->push_back( new TrigRoiDescriptor( **oldRoI ) );
        newRoI = ElementLink<TrigRoiDescriptorCollection>(*roisWriteHandle, roisWriteHandle->size() - 1, ctx);
        oldToNew[oldRoI] = newRoI;
      }

      // Update the link. Note: overwries the link set of L54
      outputDecision->setObjectLink(roiString(), newRoI);
    }
  }


	return StatusCode::SUCCESS;
}
