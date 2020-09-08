/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRKAMBIGUITYSCOREPROCESSORTOOL_H
#define ITRKAMBIGUITYSCOREPROCESSORTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h" // typedef
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"


static const InterfaceID IID_ITrackAmbiguityScoreProcessorTool("Trk::ITrackAmbiguityScoreProcessorTool", 1, 0);

namespace Trk {

/** @brief Interface for resolving hit association ambiguities in a given track collection.

The TrkAmbiguityScoreProcessor attempts to improve the 'score' of an event, where the score of an event is the summed scores of all the tracks it contains. 

*/
class ITrackAmbiguityScoreProcessorTool : virtual public IAlgTool
{
	public:

	static const InterfaceID& interfaceID( ) ;
	/** (in concrete object) Returns a processed TrackCollection from the passed 'tracks'
	@param tracks collection of tracks which will have ambiguities resolved. Will not be modified.
	@return  map of score and track. Ownership is passed on 
	(i.e. client handles deletion)*/
  virtual void process(const TrackCollection & tracks , TracksScores* scoredTracks) const = 0;
  //Print statistics at the end of the processing.
  virtual void statistics() = 0;

};

inline const InterfaceID& Trk::ITrackAmbiguityScoreProcessorTool::interfaceID()
{
	return IID_ITrackAmbiguityScoreProcessorTool;
}

} //end ns

#endif // TrackAmbiguityScoreProcessorTool_H
