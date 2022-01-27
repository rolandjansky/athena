///////////////////////// -*- C++ -*- /////////////////////////////

/*
	Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// TrkObserverTool.h 
// Header file for class TrkObserverTool
// Author: C.Rauchegger<christoph.rauchegger@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TRK_TRKOBSERVERTOOL_H
#define TRK_TRKOBSERVERTOOL_H

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/SlotSpecificObj.h"

// TrkObserverInterface includes
#include "TrkValInterfaces/ITrkObserverTool.h"

//need to include the following, since its a typedef and can't be forward declared.
#include "TrkTrack/TrackCollection.h"

// Forward declaration

// for creating xAOD with TrackParticle
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"


namespace Trk {

	class ITrackParticleCreatorTool;

	class TrkObserverTool : virtual public Trk::ITrkObserverTool, public :: AthAlgTool {
		public:
			TrkObserverTool(const std::string& type, const std::string& name, const IInterface* parent);
			virtual ~TrkObserverTool();
			// athena algtool's Hooks
			virtual StatusCode initialize();
			virtual StatusCode finalize();
			// const methods
			void updateTrackMap(int uid, double score, xAOD::RejectionStep rejectStep, xAOD::RejectionReason rejectReason) const;
			void updateScore(int uid, double score) const;
			void rejectTrack(int uid, xAOD::RejectionStep rejectStep, xAOD::RejectionReason rejectReason) const;
			void addInputTrack(int uid, const Trk::Track& track) const;
			void addSubTrack(int track_uid, int parent_uid, const Trk::Track& track) const;
			ObservedTrackMap* getTrackMap(const EventContext& ctx) const;
			int saveTracksToStore(const EventContext& ctx, const ObservedTrackMap* trk_map) const;
			void updateHolesSharedHits(int uid, int numPixelHoles, int numSCTHoles, int numSplitSharedPixel, int numSplitSharedSCT,
				int numSharedOrSplit, int numSharedOrSplitPixels, int numShared, int isPatternTrack, int totalSiHits, int inROI, int hasIBLHit,
				int hasSharedIBLHit, int hasSharedPixel, int firstPixIsShared, int numPixelDeadSensor, int numSCTDeadSensor, int numPixelHits,
				int numSCTHits, int numUnused, int numTRT_Unused, int numSCT_Unused, int numPseudo, float averageSplit1, float averageSplit2, int numWeightedShared) const;

		private:
			// name of the observed (saved) track collection
		    SG::WriteHandleKey<TrackCollection> m_savedTracksWriteKey;
		    SG::WriteHandleKey<ObservedTrackMap> m_savedTracksMapWriteKey;

			mutable std::mutex m_mutex;
			struct CacheEntry {
				EventContext::ContextEvt_t m_evt{EventContext::INVALID_CONTEXT_EVT};
				// map with observed tracks and information
				ObservedTrackMap* 	m_observedTrkMap;
			};
			mutable SG::SlotSpecificObj<CacheEntry> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex

			void newEvent(CacheEntry* ent) const;
			void dumpTrackMap(const ObservedTrackMap* trk_map) const;
			std::string dumpRejection(xAOD::RejectionStep rejectStep, xAOD::RejectionReason rejectReason) const;
			int getNFinalTracks(const ObservedTrackMap* trk_map) const;
			int getNObservedTracks(const ObservedTrackMap* trk_map) const;
			static std::map<xAOD::RejectionStep, std::string> m_rejectStep_descriptions;
			static std::map<xAOD::RejectionReason, std::string> m_rejectReason_descriptions;
	}; 
}

#endif // TRK_TRKOBSERVERTOOL_H