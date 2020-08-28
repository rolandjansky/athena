/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EXTENDTRACKTOLAYERTOOL_H
#define TRIGEFMISSINGET_EXTENDTRACKTOLAYERTOOL_H

#include "TrigEFMissingET/IExtendTrackToLayerTool.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "CaloGeoHelpers/CaloSampling.h"

/**
 * @brief Extrapolate tracks using the calo extension tool
 * @author Jon Burr
 * 
 * This is the slowest approach, but probably the most correct...
 */
class ExtendTrackToLayerTool : public virtual IExtendTrackToLayerTool, public asg::AsgTool
{
public:
  ASG_TOOL_CLASS(ExtendTrackToLayerTool, IExtendTrackToLayerTool);

  ExtendTrackToLayerTool(const std::string &name);

  virtual StatusCode initialize() override;

  virtual TrackExtension extendTrack(
    const EventContext& ctx, 
    const xAOD::TrackParticle &track) const override;
private:
  ToolHandle<Trk::IParticleCaloExtensionTool> m_extensionTool{
      this, "ExtensionTool", "", "The track extension tool"};
  Gaudi::Property<std::vector<std::string>> m_caloLayerNames{
      this, "CaloLayerNames", {"EMB2", "EME2"}, "The (ordered) names of layers to which to extend."};

  // Internals
  /// The calo layer enums
  std::vector<CaloSampling::CaloSample> m_caloLayers;
}; //> end class ExtendTrackToLayerTool

#endif //> !TRIGEFMISSINGET_EXTENDTRACKTOLAYERTOOL_H
