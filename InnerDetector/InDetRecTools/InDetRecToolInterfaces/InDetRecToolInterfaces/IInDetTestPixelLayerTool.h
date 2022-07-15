/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IInDetTestPixelLayerTool_H
#define IInDetTestPixelLayerTool_H
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ThreadLocalContext.h" //for Gaudi::Hive::currentContext()
#include "TrkParameters/TrackParameters.h"
#include <string>
#include <vector>

namespace Trk {
class Track;
class TrackParticleBase;
}
namespace InDet {
class TrackStateOnPixelLayerInfo;
}

class EventContext;

namespace InDet {

static const InterfaceID
  IID_IInDetTestPixelLayerTool("InDet::IInDetTestPixelLayerTool", 1, 0);

class IInDetTestPixelLayerTool : virtual public IAlgTool
{

public:
  static const InterfaceID& interfaceID()
  {
    return IID_IInDetTestPixelLayerTool;
  };

  virtual bool expectHitInPixelLayer(const EventContext& ctx,
                                     const Trk::TrackParticleBase*,
                                     int pixel_layer) const = 0;
  virtual bool expectHitInPixelLayer(const EventContext& ctx,
                                     const Trk::Track*,
                                     int pixel_layer,
                                     bool checkBarrelOnly = false) const = 0;
  virtual bool expectHitInPixelLayer(const EventContext& ctx,
                                     const Trk::TrackParameters* trackpar,
                                     int pixel_layer,
                                     bool checkBarrelOnly = false) const = 0;

  bool expectHitInInnermostPixelLayer(const EventContext& ctx,
                                      const Trk::Track* track) const
  {
    return expectHitInPixelLayer(ctx, track, 0, true);
  }
  bool expectHitInInnermostPixelLayer(const Trk::Track* track) const
  {
    return expectHitInInnermostPixelLayer(
      Gaudi::Hive::currentContext(), track);
  }
  bool expectHitInInnermostPixelLayer(const Trk::TrackParameters* trackpar) const
  {
    return expectHitInPixelLayer(
      Gaudi::Hive::currentContext(), trackpar, 0, true);
  }

  bool expectHitInNextToInnermostPixelLayer(const EventContext& ctx,
                                            const Trk::Track* track) const
  {
    return expectHitInPixelLayer(ctx, track, 1, true);
  }
  bool expectHitInNextToInnermostPixelLayer(const Trk::Track* track) const
  {
    return expectHitInInnermostPixelLayer(
      Gaudi::Hive::currentContext(), track);
  }
  bool expectHitInNextToInnermostPixelLayer(const Trk::TrackParameters* trackpar) const
  {
    return expectHitInPixelLayer(
      Gaudi::Hive::currentContext(), trackpar, 1, true);
  }


  virtual bool expectHit(const Trk::TrackParameters* trackpar) const = 0;

  virtual bool getTrackStateOnPixelLayerInfo(
    const Trk::TrackParticleBase*,
    std::vector<TrackStateOnPixelLayerInfo>& infoList) const = 0;
  virtual bool getTrackStateOnPixelLayerInfo(
    const Trk::Track*,
    std::vector<TrackStateOnPixelLayerInfo>& infoList) const = 0;
  virtual bool getTrackStateOnPixelLayerInfo(
    const Trk::TrackParameters* trackpar,
    std::vector<TrackStateOnPixelLayerInfo>& infoList,
    int pixel_layer = -1,
    bool checkBarrelOnly = false) const = 0;

  bool getTrackStateOnInnermostPixelLayerInfo(const Trk::TrackParameters* trackpar,
					      std::vector<TrackStateOnPixelLayerInfo>& infoList) const
  {
    return getTrackStateOnPixelLayerInfo(trackpar, infoList, 0, true);
  }

  virtual double getFracGood(const Trk::TrackParticleBase* trackpar,
                             int pixel_layer) const = 0;
  virtual double getFracGood(const Trk::TrackParameters* trackpar,
                             int pixel_layer) const = 0;
};

} // end namespace

#endif
