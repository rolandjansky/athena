/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetTestBLayerTool_H
#define InDetTestBLayerTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ThreadLocalContext.h" //for Gaudi::Hive::currentContext()
#include "GaudiKernel/ToolHandle.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "PixelReadoutGeometry/IPixelReadoutManager.h"
#include "InDetReadoutGeometry/SiDetectorElementStatus.h"
#include "InDetRecToolInterfaces/IInDetTestBLayerTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkParameters/TrackParameters.h"
#include <memory>
#include <string>
#include <vector>

namespace Trk {
class Track;
}

class AtlasDetectorID;
class PixelID;
class TrackStateOnBLayerInfo;
class EventContext;

namespace InDet {

class InDetTestBLayerTool final
  : virtual public IInDetTestBLayerTool
  , public AthAlgTool
{

public:
  InDetTestBLayerTool(const std::string& name,
                      const std::string& n,
                      const IInterface* p);
  virtual ~InDetTestBLayerTool();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual bool expectHitInBLayer(const EventContext& ctx,
                                 const Trk::Track*,
                                 bool recompute = false) const override final;

  virtual bool expectHitInBLayer(
    const Trk::TrackParameters* trackpar) const override final;

  //// return false if extrapolation failed
  virtual bool getTrackStateOnBlayerInfo(
    const Trk::Track*,
    std::vector<TrackStateOnBLayerInfo>& infoList) const override;

  virtual bool getTrackStateOnBlayerInfo(
    const Trk::TrackParameters* trackpar,
    std::vector<TrackStateOnBLayerInfo>& infoList) const override;

  virtual bool expectHitInInnermostPixelLayer(
    const EventContext& ctx,
    const Trk::Track* track,
    bool recompute = false) const override final;

  virtual bool expectHitInInnermostPixelLayer(
    const Trk::TrackParameters* trackpar) const override final;

  virtual bool getTrackStateOnInnermostPixelLayerInfo(
    const Trk::Track*,
    std::vector<TrackStateOnBLayerInfo>& infoList) const override final;

  virtual bool getTrackStateOnInnermostPixelLayerInfo(
    const Trk::TrackParameters* trackpar,
    std::vector<TrackStateOnBLayerInfo>& infoList) const override final;

  virtual bool expectHitInNextToInnermostPixelLayer(
    const EventContext& ctx,
    const Trk::Track*,
    bool recompute = false) const override final;

  virtual bool expectHitInNextToInnermostPixelLayer(
    const Trk::TrackParameters* trackpar) const override final;

  virtual bool getTrackStateOnNextToInnermostPixelLayerInfo(
    const Trk::Track*,
    std::vector<TrackStateOnBLayerInfo>& infoList) const override final;

  virtual bool getTrackStateOnNextToInnermostPixelLayerInfo(
    const Trk::TrackParameters* trackpar,
    std::vector<TrackStateOnBLayerInfo>& infoList) const override final;

private:

  bool expectHitInPixelLayer(const EventContext& ctx,
                             const Trk::Track*,
                             int layer,
                             bool recompute = false) const;

  bool expectHitInPixelLayer(const EventContext& ctx,
                             const Trk::TrackParameters* trackpar,
                             int layer) const;

  bool expectHitInPixelLayer(const Trk::TrackParameters* trackpar,
                             int layer) const
  {
    return expectHitInPixelLayer(
      Gaudi::Hive::currentContext(), trackpar, layer);
  }

  bool getTrackStateOnPixelLayerInfo(
    const Trk::Track*,
    std::vector<TrackStateOnBLayerInfo>& infoList,
    int layer) const;

  bool getTrackStateOnPixelLayerInfo(
    const Trk::TrackParameters* trackpar,
    std::vector<TrackStateOnBLayerInfo>& infoList,
    int layer) const;

  bool isActive(const Trk::TrackParameters* trackpar) const;

  bool getPixelLayerParameters(
    const EventContext& ctx,
    const Trk::TrackParameters* trackpar,
    std::vector<std::unique_ptr<const Trk::TrackParameters>>& blayerParam,
    int layer) const;

  bool getPixelLayerParameters(
    const Trk::TrackParameters* trackpar,
    std::vector<std::unique_ptr<const Trk::TrackParameters>>& blayerParam,
    int layer) const
  {
    return getPixelLayerParameters(
      Gaudi::Hive::currentContext(), trackpar, blayerParam, layer);
  }

  double getFracGood(const Trk::TrackParameters* trackpar,
                     double phiRegionSize,
                     double etaRegionSize,
                     const InDet::SiDetectorElementStatus *pixelDetElStatus) const;

  SG::ReadHandle<InDet::SiDetectorElementStatus> getPixelDetElStatus(const EventContext& ctx) const;

  /** Handle to Extrapolator AlgTool*/
  ToolHandle<Trk::IExtrapolator> m_extrapolator{
    this,
    "Extrapolator",
    {},
    "Extrapolator used to extrapolate to layers"
  };

  /** Handles to IConditionsSummaryServices for Pixels */
  ToolHandle<IInDetConditionsTool> m_pixelCondSummaryTool{
    this,
    "PixelSummaryTool",
    "PixelConditionsSummaryTool/InDetPixelConditionsSummaryTool",
    "Tool to retrieve Pixel Conditions summary"
  };

  /** @brief Optional read handle to get status data to test whether a pixel detector element is good.
   * If set to e.g. PixelDetectorElementStatus the event data will be used instead of the pixel conditions summary tool.
   */
  SG::ReadHandleKey<InDet::SiDetectorElementStatus> m_pixelDetElStatus
     {this, "PixelDetElStatus", "", "Key of SiDetectorElementStatus for Pixel"};

  ServiceHandle<InDetDD::IPixelReadoutManager> m_pixelReadout
    {this, "PixelReadoutManager", "PixelReadoutManager", "Pixel readout manager" };

  /** detector helper*/
  const AtlasDetectorID* m_idHelper;

  /**ID pixel helper*/
  const PixelID* m_pixelId;

  bool m_checkActiveAreas;
  bool m_configured;

  bool m_checkDeadRegions;
  double m_phiRegionSize;
  double m_etaRegionSize;
  double m_goodFracCut;
  bool m_checkAtLeastNearestNeighbors;
  static const std::string s_layerNames[2];
};

SG::ReadHandle<InDet::SiDetectorElementStatus> InDetTestBLayerTool::getPixelDetElStatus(const EventContext& ctx) const {
   SG::ReadHandle<InDet::SiDetectorElementStatus> pixelDetElStatus;
   if (!m_pixelDetElStatus.empty()) {
      pixelDetElStatus = SG::ReadHandle<InDet::SiDetectorElementStatus>(m_pixelDetElStatus,ctx);
      if (!pixelDetElStatus.isValid()) {
         std::stringstream msg;
         msg << "Failed to get " << m_pixelDetElStatus.key() << " from StoreGate in " << name();
         throw std::runtime_error(msg.str());
      }
   }
   return pixelDetElStatus;
}

} // end namespace

#endif
