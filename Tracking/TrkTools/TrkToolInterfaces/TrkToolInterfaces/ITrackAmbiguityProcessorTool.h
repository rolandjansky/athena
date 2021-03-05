/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/



#ifndef ITRKAMBIGUITYPROCESSORTOOL_H
#define ITRKAMBIGUITYPROCESSORTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h" // typedef
#include "AthenaKernel/CLASS_DEF.h"

static const InterfaceID IID_ITrackAmbiguityProcessorTool("Trk::ITrackAmbiguityProcessorTool", 1, 0);

namespace Trk {

  class PRDtoTrackMap;

  typedef std::vector<std::pair<const Track*, float>> TracksScores;


/** @brief Interface for resolving hit assoication ambiguities in a given track collection.

The TrkAmbiguityProcessor is responsible for resolving ambiguities in the passed TrackCollection. To do this, it attempts to improve the 'score' of an event, where the score of an event is the summed scores of all the tracks it contains. 

Currently it only implements the 'fallback' or O(n^2) algorithm. Eventually it will implement the full ambiguity solving algorithm.
@author Edward.Moyse@cern.ch */
class ITrackAmbiguityProcessorTool : virtual public IAlgTool
{
	public:

	static const InterfaceID& interfaceID( ) ;
	/** (in concrete object) Returns a processed TrackCollection from the passed 'tracks'
	@param tracks collection of tracks which will have ambiguities resolved. Will not be modified.
        @param prd_to_track_map on optional prd-to-track map being filled by the processor.
	@return new collections of tracks, with ambiguities resolved. Ownership is passed on.
	(i.e. client handles deletion).

        If no prd-to-track map is given the processor might create one internally (for internal
        use only, or exported to storegate).
        */
        virtual const TrackCollection*  process(const TrackCollection *, Trk::PRDtoTrackMap *prd_to_track_map=nullptr) const = 0;
        virtual const TrackCollection*  process(const TracksScores *) const = 0 ;

        /** Print statistics at the end of the processing.
        */
        virtual void statistics() = 0;

};

inline const InterfaceID& Trk::ITrackAmbiguityProcessorTool::interfaceID()
{
  return IID_ITrackAmbiguityProcessorTool;
}

} //end ns

CLASS_DEF( Trk::TracksScores, 22699437, 0 )


#endif // TrackAmbiguityProcessorTool_H
