/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ITRKTRACKSCORINGTOOL_H
#define ITRKTRACKSCORINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/TrackScore.h"

static const InterfaceID IID_ITrackScoringTool("Trk::ITrackScoringTool", 1, 0);

namespace Trk {

  class Track; 
  class TrackSummary;

/** @brief Interface for tool to return a score from a given track.

    @author Edward.Moyse@cern.ch */
  class ITrackScoringTool : virtual public IAlgTool
  {
    public:

      static const InterfaceID& interfaceID( ) ;
  /** create a score based on how good the passed track is
      @param track must be a valid reference (i.e. the track must exist)
      @return the returned TrackScore should be non-zero for a valid track.*/
      virtual TrackScore score( const Track& track , const bool suppressHoleSearch = false )=0;
      virtual TrackScore simpleScore( const Track& track, const TrackSummary& summary )=0;
  };

}//end of ns

inline const InterfaceID& Trk::ITrackScoringTool::interfaceID()
{
  return IID_ITrackScoringTool;
}

#endif 
