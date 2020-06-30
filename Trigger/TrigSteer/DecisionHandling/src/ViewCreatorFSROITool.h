/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DESICIONHANDLING_VIEWCREATORFSROITOOL_H
#define DESICIONHANDLING_VIEWCREATORFSROITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/WriteHandleKey.h"
#include "DecisionHandling/IViewCreatorROITool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

/**
 * @class ViewCreatorFSROITool
 * Tool to create an output collection containing a single Full Scan ROI descriptor.
 *
 * Every Decision Object passed to the tool in decisions is set to use this FS ROI.
 *
 **/
class ViewCreatorFSROITool: public extends<AthAlgTool, IViewCreatorROITool>
{
public:
  ViewCreatorFSROITool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~ViewCreatorFSROITool() = default;

   virtual StatusCode initialize() override;

  /**
   * @brief Tool interface method.
   **/
  virtual StatusCode attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const override;

  /**
   * @brief ROI Write Handle key for the FS ROI colletion.
   **/
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roisWriteHandleKey {this,"RoisWriteHandleKey","",
       "Name of the ROI collection produced by this tool."};

};

#endif //> !DESICIONHANDLING_VIEWCREATORFSROITOOL_H
