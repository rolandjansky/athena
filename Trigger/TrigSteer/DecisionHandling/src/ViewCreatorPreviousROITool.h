/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VIEWALGS_VIEWCREATORPREVIOUSROITOOL_H
#define VIEWALGS_VIEWCREATORPREVIOUSROITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/IViewCreatorROITool.h"

/**
 * @class ViewCreatorPreviousROITool
 * Basic ROI provider tool which retrieves and re-attaches an existing "roi" ElementLink.
 * This implies running the EventView spawned for the Decision object on the ROIDescriptor originating from the previous step.
 *
 * This basic tool should be replaced with more advanced slice-specific ones where appropriate.
 **/
class ViewCreatorPreviousROITool: public extends<AthAlgTool, IViewCreatorROITool>
{
public:
  ViewCreatorPreviousROITool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~ViewCreatorPreviousROITool() = default;

/**
 * @brief Tool interface method. Context not used in this tool implementation.
 **/
  virtual StatusCode attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const override;

};

#endif //> !VIEWALGS_VIEWCREATORPREVIOUSROITOOL_H
