/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_VIEWCREATORCENTREDONCLUSTERROITOOL_H
#define DECISIONHANDLING_VIEWCREATORCENTREDONCLUSTERROITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/WriteHandleKey.h"
#include "DecisionHandling/IViewCreatorROITool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

/**
 * @class ViewCreatorCentredOnClusterROITool
 * Creates a new ROI centred on one or more xAOD::TrigEMCluster contained nominally within a L1 ROI.
 *
 * By default the highest ET cluster is used (should there be multiple). Alternatively with AllowMultipleClusters,
 * a superROI may be formed if multiple clusters are reconstructed inside the (L1) ROI.
 *
 * Stores this new ROI in the output container, and links it to the Decision Object
 *
 * The new EventView spawned by the parent EventViewCreatorAlgorithm of this tool will process in this new ROI.
 *
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

  Gaudi::Property< bool > m_allowMultipleClusters{this, "AllowMultipleClusters", true,
    "If true, the tool will not take the highest energy cluster. Instead it will add them all to a superRoI"};

  Gaudi::Property< double > m_roiEtaWidth{this,"RoIEtaWidth",0.05,
    "Extent of the ROI in eta from its centre"};

  Gaudi::Property< double > m_roiPhiWidth{this,"RoIPhiWidth",0.05,
    "Extent of the ROI in phi from its centre"};

};

#endif //> !DECISIONHANDLING_VIEWCREATORCENTREDONCLUSTERROITOOL_H
