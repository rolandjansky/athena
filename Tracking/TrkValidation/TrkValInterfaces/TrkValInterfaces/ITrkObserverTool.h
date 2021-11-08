///////////////////////// -*- C++ -*- /////////////////////////////

/*
	Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ITrkObserverTool.h 
// Header file for class ITrkObserverTool
// Author: C.Rauchegger <christoph.rauchegger@cern.ch>
//
// Tool to observe/monitor the track candiates during the 
// whole ambiguity solving process
/////////////////////////////////////////////////////////////////// 
#ifndef TRK_ITRKOBSERVERTOOL_H
#define TRK_ITRKOBSERVERTOOL_H

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODTracking/TrackingPrimitives.h"
// data type of map for storing in cache
#include "TrkTrack/ObservedTrackMap.h"

// Forward declaration

namespace Trk
{
	static const InterfaceID IID_ITrkObserverTool("Trk::ITrkObserverTool", 1, 0);

	class ITrkObserverTool : virtual public IAlgTool { 
		public: 
			virtual ~ITrkObserverTool() {};
			static const InterfaceID& interfaceID();

			virtual void updateTrackMap(int uid, double score, xAOD::RejectionStep rejectStep, xAOD::RejectionReason rejectReason) const = 0;
			virtual void updateScore(int uid, double score) const = 0;
			virtual void rejectTrack(int uid, xAOD::RejectionStep rejectStep, xAOD::RejectionReason rejectReason) const = 0;
			virtual void addInputTrack(int uid, const Trk::Track& track) const = 0;
			virtual void addSubTrack(int track_uid, int parent_uid, const Trk::Track& track) const = 0;
			virtual ObservedTrackMap* getTrackMap(const EventContext& ctx) const = 0;
			virtual int saveTracksToStore(const EventContext& ctx, const ObservedTrackMap* trk_map) const = 0;
			virtual void updateHolesSharedHits(int uid, int numPixelHoles, int numSCTHoles, int numSplitSharedPixel, int numSplitSharedSCT,
				int numSharedOrSplit, int numSharedOrSplitPixels, int numShared, int isPatternTrack, int totalSiHits, int inROI, int hasIBLHit,
				int hasSharedIBLHit, int hasSharedPixel, int firstPixIsShared, int numPixelDeadSensor, int numSCTDeadSensor, int numPixelHits,
				int numSCTHits, int numUnused, int numTRT_Unused, int numSCT_Unused, int numPseudo, float averageSplit1, float averageSplit2,
				int numWeightedShared) const = 0;
	};

	inline const InterfaceID& Trk::ITrkObserverTool::interfaceID() { 
		return IID_ITrkObserverTool; 
	}
}

#endif // TRK_ITRKOBSERVERTOOL_H