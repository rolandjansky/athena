/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIAS_TRIGCOUNTSPACEPOINTSMT_H
#define TRIGMINBIAS_TRIGCOUNTSPACEPOINTSMT_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include <string>

class TrigCountSpacePointsMT : public AthReentrantAlgorithm
{
public:
  TrigCountSpacePointsMT(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &context) const override;

private:
  Gaudi::Property<bool> m_doOnlyBLayer{this, "doOnlyBLayer", false, " "};
  Gaudi::Property<int> m_pixModuleThreshold{this, "pixModuleThreshold", 100, "Dont take modules with SP Count higher than this threshold in Pixel Det."};
  Gaudi::Property<int> m_sctModuleHigherThreshold{this, "sctModuleHigherThreshold", 2000000, "Dont take modules with SP Count higher than this threshold in SCT"};
  Gaudi::Property<int> m_sctModuleLowerThreshold{this, "sctModuleLowerThreshold", 0, "Dont take modules with SP Count lowerer than this threshold in SCT"};
  //The need for these flags have to be reviewed

  /*Gaudi::Property<bool>      m_sct_barrel_a_side{this, "sct_barrel_a_side",true, "SCT Barrel side A"};
  Gaudi::Property<bool>      m_sct_barrel_c_side{this, "sct_barrel_c_side",true, "SCT Barrel side C"};
  Gaudi::Property<bool>      m_sct_endcap_a_side{this, "sct_endcap_a_side",true, "SCT Endacp side A"};
  Gaudi::Property<bool>      m_sct_endcap_c_side{this, "sct_endcap_c_side",true, "SCT Endcap side C"};
  Gaudi::Property<bool>      m_pixel_barrel{this, "pixel_barrel",true, "Pixel Barrel"};
  Gaudi::Property<bool>      m_pixel_b_layer{this, "pixel_b_layer",true, "Pixel B Layer"};
  Gaudi::Property<bool>      m_pixel_disk{this, "pixel_disk",true, "Pixel Disk"};*/

  Gaudi::Property<int> m_pixelClusToTCut{this, "pixelClusToTCut", 20, "ToT Cut for Pixel Clusters"};

  SG::ReadHandleKey<SpacePointContainer> m_pixelSpKey{this, "PixelSpKey", "PixelTrigSpacePoints", " "};
  SG::ReadHandleKey<SpacePointContainer> m_sctSpKey{this, "SCTSpKey", "SCT_TrigSpacePoints", " "};
  SG::ReadHandleKey<PixelID> m_pixelHelperKey{this, "pixelHelperKey", "DetectorStore+PixelID", " "};
  SG::ReadHandleKey<SCT_ID> m_sctHelperKey{this, "sctHelperKey", "DetectorStore+SCT_ID", " "};
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_spacePointsKey{this, "SpacePointsKey", "Undefined", ""};
  SG::WriteHandleKey<xAOD::TrigCompositeAuxContainer> m_spacePointsAuxKey{this, "SpacePointsAuxKey", "Undefined", ""};

  ToolHandle<GenericMonitoringTool> m_monTool{this, "MonTool", "", "Monitoring tool"};
};

#endif // TRIGMINBIAS_TRIGCOUNTSPACEPOINTSMT_H
