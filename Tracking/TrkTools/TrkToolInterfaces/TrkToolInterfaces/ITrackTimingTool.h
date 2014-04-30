/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKITRACKTIMINGTOOL_H
#define TRKITRACKTIMINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkSegment/SegmentCollection.h"
#include <vector>

namespace Trk {

  class Track;
  class TrackParticleBase;
  class Segment;
  class SpaceTimePoint;

static const InterfaceID IID_ITrackTimingTool("Trk::ITrackTimingTool", 1, 0);

/** @class ITrackTimingTool
    @brief Interface for providing time information for a track or track particle

    @author Dominique Fortin, Christian Ohm
*/

class ITrackTimingTool : virtual public IAlgTool {
  public:
  static const InterfaceID& interfaceID( ) ;

  /** returns time measurements for a given track. Implementations in the Calo, ID and Muon spectrometer in 
      dedicated tools
      @param  track input track
      @return vector of time measurement pointers
  */
  virtual std::vector<SpaceTimePoint*> timeMeasurements
    ( const Track& track,
      const Trk::SegmentCollection* segments = 0 ) const = 0;

  /** returns time measurements for a given track particle (base). Implementations in the Calo, ID and Muon spectrometer in 
      dedicated tools
      @param  trackParticle input track particle base
      @return vector of time measurement pointers
  */
  virtual std::vector<SpaceTimePoint*> timeMeasurements( const TrackParticleBase& trackParticle ) const = 0;

  /** return time measurements for a vector of segments - mainly aimed at MS and tagged segments.
      Not all detectors will implement this interface !
      @param segments input vector of segments
      @return vector of time measurment pointers
   */
  virtual std::vector<SpaceTimePoint*> timeMeasurements( std::vector<const Segment*> segments) const = 0;
};



inline const InterfaceID& Trk::ITrackTimingTool::interfaceID()
{ 
  return IID_ITrackTimingTool; 
}


}
#endif 
