/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VIEWALGS_IVIEWCREATORROITOOL_H
#define VIEWALGS_IVIEWCREATORROITOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"
#include "Gaudi/Property.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

/**
 * @class IViewCreatorROITool
 * MT Trigger Tool interface used to determine the correct ROI to spawn an EventView on for a given Decision object.
 **/
class IViewCreatorROITool : virtual public IAlgTool
{
public:

  DeclareInterfaceID(IViewCreatorROITool, 1, 0);

  virtual ~IViewCreatorROITool() = default;

/**
 * @brief Supply ROIs for the Decision objects in the mutable decisions container.
 * For each entry in decisions, the tool is expected to either locate an existing ROI in the history of the Decision object
 * (e.g. the initialRoI). Or, to create a new ROIDescriptor (e.g. by getting the "feature" from the Decision object and 
 * constructing an ROI around it) and to store this new ROI in an output collection. In this later case, the concrete tool
 * should register a SG::WriteHandle<TrigRoiDescriptorCollection> in which to store these new ROI.
 *
 * Either way, the new ROI should be linked to the Decision object for use in the parent EventViewCreatorAlgorithm via
 * decision->setObjectLink(TrigCompositeUtils::roiString(), roiElementLink);
 **/
  virtual StatusCode attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const = 0;

};

#endif //> !VIEWALGS_IVIEWCREATORROITOOL_H
