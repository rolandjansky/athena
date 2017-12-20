///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ITrkObserverTool.h 
// Header file for class ITrkObserverTool
// Author: C.Rauchegger <christoph.rauchegger@cern.ch>
//
// Tool to observe/monitor the track candiates during the 
// whole ambiguity solving process
/////////////////////////////////////////////////////////////////// 
#ifndef TRK_ITRKOBSERVERINTERACE
#define TRK_ITRKOBSERVERINTERACE 1

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"

// Forward declaration


namespace Trk {
	static const InterfaceID IID_ITrkObserverTool("ITrkObserverTool", 1, 0);

	class ITrkObserverTool
  	: virtual public ::IAlgTool
	{ 

		public: 

		  /** Destructor: 
  		 */
  		virtual ~ITrkObserverTool() {};

			static const InterfaceID& interfaceID();

			virtual void saveTracksToxAOD()=0;
			virtual void storeInputTracks(const TrackCollection& trackCollection) = 0;
			virtual void dumpTrackMap() = 0;

			virtual void updateTrackMap(const Trk::Track& track, double score, int rejectPlace) = 0;
			virtual void updateScore(const Trk::Track& track, double score) = 0;
			virtual void updateHolesSharedHits(const Trk::Track& track, int numPixelHoles, int numSCTHoles, int numSplitSharedPixel, int numSplitSharedSCT, int numSharedOrSplit, int numSharedOrSplitPixels, int numShared) const = 0;
			virtual void rejectTrack(const Trk::Track& track, int rejectPlace) const = 0;

			virtual void addSubTrack(const Trk::Track& track, const Trk::Track& parentTrack) = 0;
			virtual void reset() = 0;


	}; 

	inline const InterfaceID& Trk::ITrkObserverTool::interfaceID()  { 
		return IID_ITrkObserverTool; 
	}
}

#endif //> !TRK_ITRKOBSERVERINTERACE
