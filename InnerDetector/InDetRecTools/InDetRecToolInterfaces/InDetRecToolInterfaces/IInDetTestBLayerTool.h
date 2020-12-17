/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IInDetTestBLayerTool_H
#define IInDetTestBLayerTool_H
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ThreadLocalContext.h" //for Gaudi::Hive::currentContext()
#include "TrkParameters/TrackParameters.h"
#include <vector>

namespace Trk {
  class Track;
  class IResidualPullCalculator;
  class ResidualPull;
}

namespace InDet {
class TrackStateOnBLayerInfo;
}

class EventContext;

namespace InDet {


  static const InterfaceID IID_IInDetTestBLayerTool("InDet::IInDetTestBLayerTool", 1, 0);

  class IInDetTestBLayerTool : virtual public IAlgTool
  {
    
  public:

    static const InterfaceID& interfaceID() { return IID_IInDetTestBLayerTool; };

    virtual ~IInDetTestBLayerTool() {};

    virtual bool expectHitInBLayer(const EventContext& ctx,
                                   const Trk::Track* track,
                                   bool recompute = false) const = 0;
    bool expectHitInBLayer(const Trk::Track* track, bool recompute = false) const
    {
      return expectHitInBLayer(Gaudi::Hive::currentContext(), track, recompute);
    }
    virtual bool expectHitInBLayer(
      const Trk::TrackParameters* trackpar) const = 0;

    virtual const Trk::ResidualPull* bLayerHitResidual(const Trk::Track* ) const=0;
 
    virtual bool getTrackStateOnBlayerInfo(const Trk::Track*, 
				   std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;
    virtual bool getTrackStateOnBlayerInfo(const Trk::TrackParameters* trackpar, 
				   std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;

    virtual bool expectHitInInnermostPixelLayer(
      const EventContext& ctx,
      const Trk::Track* track,
      bool recompute = false) const = 0;
    bool expectHitInInnermostPixelLayer(const Trk::Track* track,
                                        bool recompute = false) const
    {
      return expectHitInInnermostPixelLayer(
        Gaudi::Hive::currentContext(), track, recompute);
    }
    virtual bool expectHitInInnermostPixelLayer(
      const Trk::TrackParameters* trackpar) const = 0;

    virtual const Trk::ResidualPull* innermostPixelLayerHitResidual(const Trk::Track* ) const=0;
 
    virtual bool getTrackStateOnInnermostPixelLayerInfo(const Trk::Track*, 
				   std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;
    virtual bool getTrackStateOnInnermostPixelLayerInfo(const Trk::TrackParameters* trackpar, 
				   std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;

    virtual bool expectHitInNextToInnermostPixelLayer(
      const EventContext& ctx,
      const Trk::Track* track,
      bool recompute = false) const = 0;
    bool expectHitInNextToInnermostPixelLayer(const Trk::Track* track,
                                              bool recompute = false) const
    {
      return expectHitInInnermostPixelLayer(
        Gaudi::Hive::currentContext(), track, recompute);
    }
    virtual bool expectHitInNextToInnermostPixelLayer(
      const Trk::TrackParameters* trackpar) const = 0;

    virtual const Trk::ResidualPull* nextToInnermostPixelLayerHitResidual(
      const Trk::Track*) const = 0;
    
    virtual bool getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::Track*, 
							std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;
    virtual bool getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::TrackParameters* trackpar, 
							std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;


  };
  
  
}// end namespace

#endif
