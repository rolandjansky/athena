/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_VIEWCREATORPREVIOUSROITOOL_H
#define DECISIONHANDLING_VIEWCREATORPREVIOUSROITOOL_H

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

  virtual StatusCode initialize() override;

/**
 * @brief Tool interface method. Context not used in this tool implementation.
 **/
  virtual StatusCode attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const override;

  Gaudi::Property< std::string > m_roiSGKey{this, "RoISGKey", "",
    "Optional StorgeGate key of ROI collection. Use this when an ambiguous situation needs resolving, e.g. after merging different reconstructon paths"};

  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roisWriteHandleKey {this,"RoisWriteHandleKey", "",
    "Optional StoreGate key, will cause this Tool to create a new output ROI collection, instead of re-using the existing links."};

};

#endif //> !DECISIONHANDLING_VIEWCREATORPREVIOUSROITOOL_H
