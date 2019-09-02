///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    : public extends<AthAlgTool, Trk::ITrkObserverTool>
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
    virtual ~TrkObserverTool() = default;

    // Athena algtool's Hooks
    virtual StatusCode  initialize() override;
    virtual StatusCode  finalize() override;

    virtual void saveTracksToxAOD() const override;
    virtual void dumpTrackMap() const override;

    virtual void updateTrackMap(const Trk::Track& track, double score, int rejectPlace) override;
    virtual void updateScore(const Trk::Track& track, double score) override;
    virtual void updateHolesSharedHits(const Trk::Track& track, int numPixelHoles, int numSCTHoles, int numSplitSharedPixel, int numSplitSharedSCT, int numSharedOrSplit, int numSharedOrSplitPixels, int numShared) const override;
    virtual void rejectTrack(const Trk::Track& track, int rejectPlace) const override;

    virtual void addSubTrack(const Trk::Track& track, const Trk::Track& parentTrack) override;
    virtual void storeInputTrack(const Trk::Track& track) override;
    virtual void reset() override;

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
  private: 

    /// job Properties
    StringProperty m_savedTracksName{this, "ObservedTrackCollectionName", "ObservedTrackCollection", "name of the observed (saved) Track collection"};
    StringProperty m_observedTrksXaodName{this, "ObservedTracksXaodName", "ObservedTracks", "name of the observed tracks in xaod"};
    PublicToolHandle<Trk::ITrackParticleCreatorTool> m_particleCreator{this, "TrackParticleCreator", "Trk::TrackParticleCreatorTool/TrackParticleCreatorTool", "paricle creator, for creating TrackParticle from Tracks"};

    std::unique_ptr<ObservedTracksMap> m_observedTrkMap; // Maps with the all observed tracks and information

    std::vector<const Track*> m_savedTracks;  //  all the Tracks which are/should be saved for xaod, no ownership
  }; 
}


#endif //> !TRK_TRKOBSERVERTOOL_H
