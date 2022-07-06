/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_VIEWCREATORDVROITOOL_H
#define DECISIONHANDLING_VIEWCREATORDVROITOOL_H

#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/WriteHandleKey.h"
#include "DecisionHandling/IViewCreatorROITool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

/**
 * @class ViewCreatorDVROITool
 * Creates a new (super)-ROI combining windows centred on HitDV seed jets passing eta/pt selection
 * criteria, extracted from a single decision object (which would normally link the FSRoI).
 *
 * Stores this new ROI in the output container, and links it to the Decision Object
 *
 * The new EventView spawned by the parent EventViewCreatorAlgorithm of this tool will process in this new ROI.
 *
 * This tool is mainly intended to create a view for fast tracking preselection operations, beginning from
 * the jets used for pure calorimeter-based preselection.
 **/

class ViewCreatorDVROITool : public extends<AthAlgTool, IViewCreatorROITool>
{
public:
   ViewCreatorDVROITool(const std::string& type, const std::string& name, const IInterface* parent);

   virtual ~ViewCreatorDVROITool() = default;

   virtual StatusCode initialize() override;

   /**
    * @brief Tool interface method.
    **/
   virtual StatusCode attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const override;

public:
   SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roisWriteHandleKey {this,"RoisWriteHandleKey","",
      "Name of the ROI collection produced by this tool."};

   Gaudi::Property< std::string > m_featureLinkName{this,"FeatureLinkName","HitDVSeedLink",
      "The name of the decoration holding the list of jets used for preselection"};

   Gaudi::Property< double > m_roiEtaWidth{this,"RoIEtaWidth",0.4,
      "Extent of the ROI in eta from its centre"};

   Gaudi::Property< double > m_roiPhiWidth{this,"RoIPhiWidth",0.4,
      "Extent of the ROI in phi from its centre"};

   // Need to get beamspot position from somewhere to recentre?
   Gaudi::Property< double > m_roiZWidth {this,"RoIZWidth",1500.0,
      "Z Half Width in mm"};

};

#endif //> !DECISIONHANDLING_VIEWCREATORDVROITOOL_H
