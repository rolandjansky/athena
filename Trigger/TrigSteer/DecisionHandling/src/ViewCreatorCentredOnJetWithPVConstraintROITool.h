/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DESICIONHANDLING_VIEWCREATORCENTREDONJETWITHPVCONSTRAINTROITOOL_H
#define DESICIONHANDLING_VIEWCREATORCENTREDONJETWITHPVCONSTRAINTROITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/WriteHandleKey.h"
#include "DecisionHandling/IViewCreatorROITool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

/**
 * @class ViewCreatorCentredOnJetWithPVConstraintROITool
 * Creates a new ROI centred on an object deriving from xAOD::Jet adding a PV constraint.
 *
 * Stores this new ROI in the output container, and links it to the Decision Object
 *
 * The new EventView spawned by the parent EventViewCreatorAlgorithm of this tool will process in this new ROI.
 *
 * In the majority of cases, this tool will be used to create the new ROI centred on the "feature" from the previous Step.
 **/
  class ViewCreatorCentredOnJetWithPVConstraintROITool : public extends<AthAlgTool, IViewCreatorROITool>
{
public:
  ViewCreatorCentredOnJetWithPVConstraintROITool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~ViewCreatorCentredOnJetWithPVConstraintROITool() = default;

   virtual StatusCode initialize() override;

  /**
   * @brief Tool interface method.
   **/
  virtual StatusCode attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const override;


 protected:
  const xAOD::Vertex* getPrimaryVertex( const xAOD::VertexContainer* ) const;

 public:
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roisWriteHandleKey {this,"RoisWriteHandleKey","",
    "Name of the ROI collection produced by this tool."};

  Gaudi::Property< double > m_roiEtaWidth{this,"RoIEtaWidth",0.4,
    "Extent of the ROI in eta from its centre"};

  Gaudi::Property< double > m_roiPhiWidth{this,"RoIPhiWidth",0.4,
    "Extent of the ROI in phi from its centre"};

  Gaudi::Property< double > m_roiZWidth {this,"RoIZWidth",10.0,
      "Z Half Width in mm"};
  
  SG::ReadHandleKey< xAOD::VertexContainer > m_vertexReadHandleKey {this,"VertexReadHandleKey","",
      "Name of the Vertex collection required as input for constructing the ROI"};

  Gaudi::Property< std::string > m_prmVtxLink {this,"PrmVtxLink","",
      "PrmVtx link to attach to the output decision"};

};

#endif //> !DESICIONHANDLING_VIEWCREATORCENTREDONJETWITHPVCONSTRAINTROITOOL_H
