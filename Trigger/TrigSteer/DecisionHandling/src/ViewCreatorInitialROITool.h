/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DESICIONHANDLING_VIEWCREATORINITIALROITOOL_H
#define DESICIONHANDLING_VIEWCREATORINITIALROITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/IViewCreatorROITool.h"

/**
 * @class ViewCreatorInitialROITool
 * Basic ROI provider tool which retrieves and re-attaches an existing "initialRoI" ElementLink.
 * This implies running the EventView spawned for the Decision object on the ROIDescriptor originating from L1.
 *
 * Every Decision object in decisions must have a single "initialRoI" element link added at the root of 
 * the navigation graph for the Decision object.
 *
 * This basic tool should be replaced with more advanced slice-specific ones where appropriate.
 **/
class ViewCreatorInitialROITool: public extends<AthAlgTool, IViewCreatorROITool>
{
public:
  ViewCreatorInitialROITool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~ViewCreatorInitialROITool() = default;

/**
 * @brief Tool interface method. Context not used in this tool implementation.
 **/
  virtual StatusCode attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const override;

};

#endif //> !DESICIONHANDLING_VIEWCREATORINITIALROITOOL_H
