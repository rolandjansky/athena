/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_ROIUPDATERTOOL_H
#define DECISIONHANDLING_ROIUPDATERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "HLTSeeding/IRoiUpdaterTool.h"

/**
 * @class RoiUpdaterTool
 * Tool to create a new RoiDescriptor from an existing input5 RoiDescriptor
 *

 **/
class RoiUpdaterTool: public extends<AthAlgTool, IRoiUpdaterTool>
{
public:

  RoiUpdaterTool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~RoiUpdaterTool() = default;
  
  virtual StatusCode initialize() override;

  std::unique_ptr<TrigRoiDescriptor> execute( const EventContext& ctx ) const override;

  std::unique_ptr<TrigRoiDescriptor> execute( const IRoiDescriptor* iroi, const EventContext& ctx ) const override;

  /// whether to update the RoiDescritor or not - determiuned from whether any of 
  /// update parameters are set
  bool m_update;

  /** 
   * don't want these parameters used if not set
   **/
  Gaudi::Property<double> m_etaWidth { this, "EtaWidth", -999, "FS Roi eta half width" };
  Gaudi::Property<double> m_phiWidth { this, "PhiWidth", -999, "FS Roi phi half width" };
  Gaudi::Property<double> m_zedWidth { this, "ZedWidth", -999, "FS Roi zed half width" };

  Gaudi::Property<bool>   m_useBeamspot { this, "useBeamSpot", false, "use beamspot for zed width" };
  /// default settings - should be retuned with data
  Gaudi::Property<double> m_nsigma      { this, "NSigma",       3,   "width (in simga) for the beamspot Roi width" };
  Gaudi::Property<double> m_fence       { this, "Fance",       10,   "fence width for the beamspot Roi width" }; /// do we need an mm unit here ? 

  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamspotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

};

#endif //> !DECISIONHANDLING_ROIUPDATERTOOL_H
