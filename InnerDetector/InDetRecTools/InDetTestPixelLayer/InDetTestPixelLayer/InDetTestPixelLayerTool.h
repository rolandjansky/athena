/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "InDetTestPixelLayer/PixelIDLayerComp.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

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

namespace InDet {

typedef std::map<Identifier, const Trk::ResidualPull*, PixelIDLayerComp>
  PixelLayerResidualPullMap;
typedef std::vector<Identifier> PixelIDVec;

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
    const Trk::TrackParticleBase*,
    int pixel_layer,
    bool recompute = false) const override final;
  virtual bool expectHitInPixelLayer(
    const Trk::Track*,
    int pixel_layer,
    bool recompute = false) const override final;
  virtual bool expectHitInPixelLayer(const Trk::TrackParameters* trackpar,
                                     int pixel_layer) const override final;

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
    std::vector<TrackStateOnPixelLayerInfo>& infoList) const override final;

  virtual double getFracGood(const Trk::TrackParticleBase* trackpar,
                             int pixel_layer) const override final;
  virtual double getFracGood(const Trk::TrackParameters* trackpar,
                             int pixel_layer) const override final;

private:

  bool IsInCorrectLayer(Identifier&, PixelIDVec&, int) const;
  bool IsInSameLayer(Identifier&, Identifier&) const;

  bool isActive(const Trk::TrackParameters* trackpar) const;
  bool getPixelLayerParameters(
    const Trk::TrackParameters* trackpar,
    std::vector<std::unique_ptr<const Trk::TrackParameters>>& pixelLayerParam)
    const;
  double getFracGood(const Trk::TrackParameters* trackpar,
                     double phiRegionSize,
                     double etaRegionSize) const;

  /** Pointer to Extrapolator AlgTool*/
  ToolHandle<Trk::IExtrapolator> m_extrapolator;

  /** Handles to IConditionsSummaryServices for Pixels */
  ToolHandle<IInDetConditionsTool> m_pixelCondSummaryTool{
    this,
    "PixelSummaryTool",
    "PixelConditionsSummaryTool/InDetPixelConditionsSummaryTool",
    "Tool to retrieve Pixel Conditions summary"
  };


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
};

} // end namespace

#endif
