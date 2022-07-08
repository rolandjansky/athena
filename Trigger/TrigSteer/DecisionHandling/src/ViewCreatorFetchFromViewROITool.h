/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_VIEWCREATORFETCHFROMVIEWROITOOL_H
#define DECISIONHANDLING_VIEWCREATORFETCHFROMVIEWROITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/WriteHandleKey.h"
#include "DecisionHandling/IViewCreatorROITool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

/**
 * @class ViewCreatorFetchFromViewROITool
 * Allows a Decision Object to process the current Step over an ROI
 * which was created during a previous Step.
 *
 * This tools is to be used when this ROI from the previous Step was created inside an EventView.
 *
 * Exactly ONE ROI must have been written to the TrigRoiDescriptorCollection inside the EventView.
 *
 * As we cannot write out to BS or POOL ROIDescriptor collections from inside EventViews for technical
 * reasons, the located ROI is copied into a new TrigRoiDescriptorCollection which is produced by this tool
 **/
class ViewCreatorFetchFromViewROITool: public extends<AthAlgTool, IViewCreatorROITool>
{
public:
  ViewCreatorFetchFromViewROITool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~ViewCreatorFetchFromViewROITool() = default;

   virtual StatusCode initialize() override;

  /**
   * @brief Tool interface method.
   **/
  virtual StatusCode attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const override;

  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roisWriteHandleKey {this,"RoisWriteHandleKey","",
    "Name of the ROI collection produced by this tool."};

  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_inViewRoIKey{this,"InViewRoIs","",
    "Name of the ROI collection within the most recent EventView of the input Decision objects"};

  Gaudi::Property< std::string > m_viewToFetchFrom {this,"ViewToFetchFrom","", 
      "Optional name of EventView to fetch ROI from. Must be in the history of the DecisionObject. If not supplied, the most recent EventView will be used."};

  Gaudi::Property< bool > m_doResize{this,"doResize",false,
      "Resize RoI if updated widths are given."};

  Gaudi::Property< double > m_roiEtaWidth{this,"RoIEtaWidth",-1.0,
      "New extent of the ROI in eta from its centre"};

  Gaudi::Property< double > m_roiPhiWidth{this,"RoIPhiWidth",-1.0,
      "New extent of the ROI in phi from its centre"};

  Gaudi::Property< double > m_roiZedWidth{this,"RoIZedWidth",-1.0,
      "New extent of the ROI in zed from its centre"};

  private:
    std::string m_viewToFetchFromProbe;
};

#endif //> !DECISIONHANDLING_VIEWCREATORFETCHFROMVIEWROITOOL_H
