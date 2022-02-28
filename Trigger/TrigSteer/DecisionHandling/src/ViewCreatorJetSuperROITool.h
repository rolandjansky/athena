/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_VIEWCREATORJETSUPERROITOOL_H
#define DECISIONHANDLING_VIEWCREATORJETSUPERROITOOL_H

#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/WriteHandleKey.h"
#include "DecisionHandling/IViewCreatorROITool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

/**
 * @class ViewCreatorJetSuperROITool
 * Creates a new (super)-ROI combining windows centred on jets passing eta/pt selection
 * criteria, extracted from a single decision object (which would normally link the FSRoI).
 *
 * Stores this new ROI in the output container, and links it to the Decision Object
 *
 * The new EventView spawned by the parent EventViewCreatorAlgorithm of this tool will process in this new ROI.
 *
 * This tool is mainly intended to create a view for fast tracking preselection operations, beginning from
 * the jets used for pure calorimeter-based preselection.
 **/
  class ViewCreatorJetSuperROITool : public extends<AthAlgTool, IViewCreatorROITool>
{
public:
  ViewCreatorJetSuperROITool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~ViewCreatorJetSuperROITool() = default;

   virtual StatusCode initialize() override;

  /**
   * @brief Tool interface method.
   **/
  virtual StatusCode attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const override;


 protected:

 public:
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roisWriteHandleKey {this,"RoisWriteHandleKey","",
    "Name of the ROI collection produced by this tool."};

  Gaudi::Property< std::string > m_featureLinkName{this,"FeatureLinkName","PreselJets",
    "The name of the decoration holding the list of jets used for preselection"};

  Gaudi::Property< double > m_jetMinPt{this,"JetMinPt",20*Gaudi::Units::GeV,
    "Minimum jet pT used for RoI creation"};

  // Use rapidity?
  Gaudi::Property< double > m_jetMaxAbsEta{this,"JetMaxAbsEta",2.5,
    "Maximum absolute jet pseudorapidity used for RoI creation"};

  Gaudi::Property< double > m_roiEtaWidth{this,"RoIEtaWidth",0.4,
    "Extent of the ROI in eta from its centre"};

  Gaudi::Property< double > m_roiPhiWidth{this,"RoIPhiWidth",0.4,
    "Extent of the ROI in phi from its centre"};

  // Need to get beamspot position from somewhere to recentre?
  Gaudi::Property< double > m_roiZWidth {this,"RoIZWidth",150.0,
      "Z Half Width in mm"};

};

#endif //> !DECISIONHANDLING_VIEWCREATORJETSUPERROITOOL_H
