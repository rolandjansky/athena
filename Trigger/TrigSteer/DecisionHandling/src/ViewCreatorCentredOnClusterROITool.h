/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DESICIONHANDLING_VIEWCREATORCENTREDONCLUSTERROITOOL_H
#define DESICIONHANDLING_VIEWCREATORCENTREDONCLUSTERROITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/WriteHandleKey.h"
#include "DecisionHandling/IViewCreatorROITool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

/**
 * @class ViewCreatorCentredOnClusterROITool
 * Creates a new ROI centred on an xAOD::TrigEMCluster.
 *
 * Stores this new ROI in the output container, and links it to the Decision Object
 *
 * The new EventView spawned by the parent EventViewCreatorAlgorithm of this tool will process in this new ROI.
 *
 * In the majority of cases, this tool will be used to create the new ROI centred on the "feature" from the previous Step.
 **/
class ViewCreatorCentredOnClusterROITool: public extends<AthAlgTool, IViewCreatorROITool>
{
public:
  ViewCreatorCentredOnClusterROITool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~ViewCreatorCentredOnClusterROITool() = default;

   virtual StatusCode initialize() override;

  /**
   * @brief Tool interface method.
   **/
  virtual StatusCode attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const override;

  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roisWriteHandleKey {this,"RoisWriteHandleKey","",
    "Name of the ROI collection produced by this tool."};

  Gaudi::Property< std::string > m_clusterLinkName{this,"ClusterLinkName","feature",
    "Name of linked Cluster object to centre the new ROI on. Normally the 'feature' from the previous Step."};

  Gaudi::Property< double > m_roiEtaWidth{this,"RoIEtaWidth",0.05,
    "Extent of the ROI in eta from its centre"};

  Gaudi::Property< double > m_roiPhiWidth{this,"RoIPhiWidth",0.05,
    "Extent of the ROI in phi from its centre"};

};

#endif //> !DESICIONHANDLING_VIEWCREATORCENTREDONCLUSTERROITOOL_H
