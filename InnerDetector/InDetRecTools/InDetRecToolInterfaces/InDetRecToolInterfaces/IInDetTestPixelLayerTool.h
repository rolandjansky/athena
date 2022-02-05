/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IInDetTestPixelLayerTool_H
#define IInDetTestPixelLayerTool_H
#include "GaudiKernel/AlgTool.h"
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

  virtual bool expectHitInPixelLayer(const Trk::TrackParticleBase*,
                                     int pixel_layer,
                                     bool recompute = false) const = 0;
  virtual bool expectHitInPixelLayer(const Trk::Track*,
                                     int pixel_layer,
                                     bool recompute = false) const = 0;
  virtual bool expectHitInPixelLayer(const Trk::TrackParameters* trackpar,
                                     int pixel_layer) const = 0;
  virtual bool expectHit(const Trk::TrackParameters* trackpar) const = 0;

  virtual bool getTrackStateOnPixelLayerInfo(
    const Trk::TrackParticleBase*,
    std::vector<TrackStateOnPixelLayerInfo>& infoList) const = 0;
  virtual bool getTrackStateOnPixelLayerInfo(
    const Trk::Track*,
    std::vector<TrackStateOnPixelLayerInfo>& infoList) const = 0;
  virtual bool getTrackStateOnPixelLayerInfo(
    const Trk::TrackParameters* trackpar,
    std::vector<TrackStateOnPixelLayerInfo>& infoList) const = 0;

  virtual double getFracGood(const Trk::TrackParticleBase* trackpar,
                             int pixel_layer) const = 0;
  virtual double getFracGood(const Trk::TrackParameters* trackpar,
                             int pixel_layer) const = 0;
};

} // end namespace

#endif
