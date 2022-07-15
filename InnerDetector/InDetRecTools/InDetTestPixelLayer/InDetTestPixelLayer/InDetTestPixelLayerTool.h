/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetTestPixelLayerTool_H
#define InDetTestPixelLayerTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/IInDetTestPixelLayerTool.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "InDetTestPixelLayer/TrackStateOnPixelLayerInfo.h"

#include "InDetConditionsSummaryService/IInDetConditionsTool.h"

#include "PixelReadoutGeometry/IPixelReadoutManager.h"
#include "InDetReadoutGeometry/SiDetectorElementStatus.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ThreadLocalContext.h" //for Gaudi::Hive::currentContext()

#include <mutex>
#include <string>
#include <vector>

namespace Trk {
class Track;
class IExtrapolator;
class TrackParticleBase;
class IResidualPullCalculator;
}
namespace Rec {
class TrackParticle;
}
class AtlasDetectorID;
class Identifier;
class PixelID;
class EventContext;

namespace InDet {

class InDetTestPixelLayerTool
  : virtual public IInDetTestPixelLayerTool
  , public AthAlgTool
{

public:
  InDetTestPixelLayerTool(const std::string& name,
                          const std::string& n,
                          const IInterface* p);

  virtual StatusCode initialize() override final;

  //    bool expectHitInPixelLayer(const Rec::TrackParticle*) const ;
  virtual bool expectHitInPixelLayer(
    const EventContext& ctx,
    const Trk::TrackParticleBase*,
    int pixel_layer) const override final;
  virtual bool expectHitInPixelLayer(
    const EventContext& ctx,
    const Trk::Track*,
    int pixel_layer,
    bool checkBarrelOnly = false) const override final;
  virtual bool expectHitInPixelLayer(const EventContext& ctx,
                                     const Trk::TrackParameters* trackpar,
                                     int pixel_layer,
                                     bool checkBarrelOnly = false) const override final;

  virtual bool expectHit(
    const Trk::TrackParameters* trackpar) const override final;

  //// return false if extrapolation failed
  virtual bool getTrackStateOnPixelLayerInfo(
    const Trk::TrackParticleBase*,
    std::vector<TrackStateOnPixelLayerInfo>& infoList) const override final;
  virtual bool getTrackStateOnPixelLayerInfo(
    const Trk::Track*,
    std::vector<TrackStateOnPixelLayerInfo>& infoList) const override final;
  virtual bool getTrackStateOnPixelLayerInfo(
    const Trk::TrackParameters* trackpar,
    std::vector<TrackStateOnPixelLayerInfo>& infoList,
    int pixel_layer = -1,
    bool checkBarrelOnly = false) const override final;

  virtual double getFracGood(const Trk::TrackParticleBase* trackpar,
                             int pixel_layer) const override final;
  virtual double getFracGood(const Trk::TrackParameters* trackpar,
                             int pixel_layer) const override final;

private:

  bool isActive(const Trk::TrackParameters* trackpar) const;
  bool getPixelLayerParameters(
    const EventContext& ctx,
    const Trk::TrackParameters* trackpar,
    std::vector<std::unique_ptr<const Trk::TrackParameters>>& pixelLayerParam)
    const;
  bool getPixelLayerParameters(
    const Trk::TrackParameters* trackpar,
    std::vector<std::unique_ptr<const Trk::TrackParameters>>& pixelLayerParam)
    const
  {
    return getPixelLayerParameters(
      Gaudi::Hive::currentContext(), trackpar, pixelLayerParam);
  }

  double getFracGood(const Trk::TrackParameters* trackpar,
                     double phiRegionSize,
                     double etaRegionSize,
                     const InDet::SiDetectorElementStatus *pixelDetElStatus) const;

  SG::ReadHandle<InDet::SiDetectorElementStatus> getPixelDetElStatus(const EventContext& ctx) const;

  /** Pointer to Extrapolator AlgTool*/
  PublicToolHandle<Trk::IExtrapolator> m_extrapolator{
    this,
    "Extrapolator",
    "Trk::Extrapolator/InDetExtrapolator",
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
  bool m_checkDisabledFEs;
  double m_phiRegionSize;
  double m_etaRegionSize;
  double m_goodFracCut;
  double m_outerRadius;
};


SG::ReadHandle<InDet::SiDetectorElementStatus> InDetTestPixelLayerTool::getPixelDetElStatus(const EventContext& ctx) const {
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
