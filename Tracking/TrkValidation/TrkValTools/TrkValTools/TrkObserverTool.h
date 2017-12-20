///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrkObserverTool.h 
// Header file for class TrkObserverTool
// Author: C.Rauchegger<christoph.rauchegger@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TRK_TRKOBSERVERTOOL_H
#define TRK_TRKOBSERVERTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

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


// data types of my maps for storing in store gate
#include "ObservedTracksMap.h"

namespace Trk {
	class ITrackParticleCreatorTool;


	class TrkObserverTool
  	: virtual public Trk::ITrkObserverTool,
            public ::AthAlgTool
	{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
	public: 
	
  	// Copy constructor: 

  	/// Constructor with parameters: 
  	TrkObserverTool( const std::string& type,
		     const std::string& name, 
		     const IInterface* parent );

  	/// Destructor: 
  	virtual ~TrkObserverTool(); 

  	// Athena algtool's Hooks
  	virtual StatusCode  initialize();
		virtual StatusCode  finalize();

		void saveTracksToxAOD();
		void dumpTrackMap();

		void updateTrackMap(const Trk::Track& track, double score, int rejectPlace);
		void updateScore(const Trk::Track& track, double score);
		void updateHolesSharedHits(const Trk::Track& track, int numPixelHoles, int numSCTHoles, int numSplitSharedPixel, int numSplitSharedSCT, int numSharedOrSplit, int numSharedOrSplitPixels, int numShared) const;
		void rejectTrack(const Trk::Track& track, int rejectPlace) const;

		void addSubTrack(const Trk::Track& track, const Trk::Track& parentTrack);
		void storeInputTracks(const TrackCollection& trackCollection);
		void reset();

	/////////////////////////////////////////////////////////////////// 
  // Private data: 
	/////////////////////////////////////////////////////////////////// 
	private: 

		/// job Properties
		std::string m_savedTracksName;	// name of the observed (saved) Track collection
		std::string m_observedTrksXaodName;	// name of the observed tracks in xaod

		ObservedTracksMap* 	m_observedTrkMap;	// Maps with the all observed tracks and information




		ToolHandle<Trk::ITrackParticleCreatorTool> m_particleCreator;		// paricle creator, for creating TrackParticle from Tracks
		TrackCollection* m_savedTracks;		// collection with all the Tracks whcih are/should be saved
		inline xAOD::TrackParticle* createParticle( xAOD::TrackParticleContainer& xaod, const TrackCollection& container, const Trk::Track& tp) ;	// method for particle creation from tracks



		/// Default constructor: 
		TrkObserverTool();

	}; 
}


#endif //> !TRK_TRKOBSERVERTOOL_H
