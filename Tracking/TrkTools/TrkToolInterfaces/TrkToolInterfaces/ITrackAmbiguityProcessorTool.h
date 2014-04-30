/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef ITRKAMBIGUITYPROCESSORTOOL_H
#define ITRKAMBIGUITYPROCESSORTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h" // typedef

static const InterfaceID IID_ITrackAmbiguityProcessorTool("Trk::ITrackAmbiguityProcessorTool", 1, 0);

namespace Trk {

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
	@return new collections of tracks, with ambiguities resolved. Ownership is passed on 
	(i.e. client handles deletion)*/
	virtual TrackCollection*  process(const TrackCollection* tracks)=0;

        /** statistics */
        virtual void statistics() {};

};

inline const InterfaceID& Trk::ITrackAmbiguityProcessorTool::interfaceID()
{
	return IID_ITrackAmbiguityProcessorTool;
}

} //end ns

#endif // TrackAmbiguityProcessorTool_H
