/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonCombinedTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IRECMUONCREATORTOOL_H
#define IRECMUONCREATORTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkTrack/TrackCollection.h"

namespace MuonCombined {

  static const InterfaceID IID_IMuonCreatorTool("MuonCombined::IMuonCreatorTool", 1, 0);

  /** @class IMuonCreatorTool
      @brief interface for tools building combined muons from ID and Muon candidates
 
      @author Niels van Eldik
   */

  class IMuonCreatorTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;


    /**IMuonCreatorTool interface: build muons from ID and MS candidates */    
    virtual void create( const MuonCandidateCollection* muonCandidates, const InDetCandidateCollection* inDetCandidates,
			 xAOD::MuonContainer& container, 
			 xAOD::TrackParticleContainer* combinedTrackParticleContainer=0, TrackCollection* combinedTrackCollection=0,
			 xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer=0, TrackCollection* extrapolatedTrackCollection=0,
			 xAOD::MuonSegmentContainer* xaodSegments=0, Trk::SegmentCollection* muonSegmentCollection=0
 ) const = 0;

    /** create a muon from a muon candidate */
    virtual xAOD::Muon* create( const MuonCandidate& candidate, xAOD::MuonContainer& container, 
				xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer=0, TrackCollection* extrapolatedTrackCollection=0
				) const = 0;

    /** create a muon from a muon candidate */
    virtual xAOD::Muon* create( const InDetCandidate& candidate, xAOD::MuonContainer& container, 
				xAOD::TrackParticleContainer* combinedTrackParticleContainer=0, TrackCollection* combinedTrackCollection=0,
				xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer=0, TrackCollection* extrapolatedTrackCollection=0,
				xAOD::MuonSegmentContainer* xaodSegments=0, Trk::SegmentCollection* muonSegmentCollection=0 ) const = 0;

  };

  inline const InterfaceID& IMuonCreatorTool::interfaceID()
    { 
      return IID_IMuonCreatorTool; 
    }

} // end of namespace

#endif 
