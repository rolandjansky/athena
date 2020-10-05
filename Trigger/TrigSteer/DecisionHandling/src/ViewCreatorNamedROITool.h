/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DESICIONHANDLING_VIEWCREATORNAMEDROITOOL_H
#define DESICIONHANDLING_VIEWCREATORNAMEDROITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/IViewCreatorROITool.h"

/**
 * @class ViewCreatorNamedROITool
 * Basic ROI provider tool which retrieves and re-attaches an existing named ElementLink.
 *
 * Every Decision object in decisions must have a single most-recent element link with name configured via "ROILinkName" property
 *
 **/
class ViewCreatorNamedROITool: public extends<AthAlgTool, IViewCreatorROITool>
{
public:
  ViewCreatorNamedROITool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~ViewCreatorNamedROITool() = default;

/**
 * @brief Tool interface method. Context not used in this tool implementation.
 **/
  virtual StatusCode attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const override;

  Gaudi::Property< std::string > m_roiLinkName{this,"ROILinkName","UNSPECIFIED",
    "Name of linked ROI from a previous step."};

};

#endif //> !DESICIONHANDLING_VIEWCREATORNAMEDROITOOL_H
