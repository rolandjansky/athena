/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackSelectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKAMBITRACKSELECTIONTOOL_H
#define ITRKAMBITRACKSELECTIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/TrackScore.h"
#include <vector>

namespace Trk 
{
  class Track;
  class PrepRawData;
  class PRDtoTrackMap;
  
  /** @class Trk::IAmbiTrackSelectionTool
      @brief Interface for building new tracks using information about shared and already associated hits.

      @author Martin Siebel <http://consult.cern.ch/xwho>
   */

  class IAmbiTrackSelectionTool : virtual public IAlgTool {
  public:
    DeclareInterfaceID( IAmbiTrackSelectionTool, 1, 0 );

    /** Performs cleaning of a track from already used hits.
        @param track the input track to be checked and cleaned.
        @param prd_to_track_map a map to identify shared hits.
        @param score the score tha twas given to the input track
        @return tuple where the first element is a potiner to a new track or a nullptr and the second element is a flag which is set to false if the input track is to be rejected.
        The second element of the returned tuple is false if the input input track is to be rejected.
        The input track is rejected if it does not fulfil quality criteria or if a new cleaned track is created
        replacing the input track. The second element of the returned tuple is true if the input track does not
        reuire cleaning, fulfils the quality criteria and should be kept.
    */
    virtual std::tuple<Trk::Track*,bool> getCleanedOutTrack(const Trk::Track *track,
                                                            const Trk::TrackScore score,
                                                            Trk::PRDtoTrackMap &prd_to_track_map) const =0;
  };

} // end of namespace

#endif 
