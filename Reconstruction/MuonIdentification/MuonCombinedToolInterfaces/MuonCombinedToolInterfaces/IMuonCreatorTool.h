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
#include "xAODMuon/SlowMuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace MuonCombined {

  static const InterfaceID IID_IMuonCreatorTool("MuonCombined::IMuonCreatorTool", 1, 0);

  /** @class IMuonCreatorTool
      @brief interface for tools building combined muons from ID and Muon candidates
      
      The tool is capable of writing a large number of different output formats. Whether or not certain outputs 
      are written is controled using the OutputData struct defined below. 
      If the pointer to a certain type is non-zero, the container is filled. 
      The MuonContainer is manditory, all other output types are optional.

      @author Niels van Eldik
   */

  class IMuonCreatorTool : virtual public IAlgTool {
  public:
    struct OutputData {
      OutputData(xAOD::MuonContainer& container) : muonContainer(&container),
                                                   combinedTrackParticleContainer(0),combinedTrackCollection(0),
                                                   extrapolatedTrackParticleContainer(0), extrapolatedTrackCollection(0),
	                                           msOnlyExtrapolatedTrackParticleContainer(0), msOnlyExtrapolatedTrackCollection(0),
                                                   xaodSegmentContainer(0), muonSegmentCollection(0),
                                                   slowMuonContainer(0), clusterContainer(0) {
      }
      /** MuonContainer to be filled with the Muon objects */
      xAOD::MuonContainer* muonContainer; 

      /** container for the combined track particles */
      xAOD::TrackParticleContainer* combinedTrackParticleContainer;

      /** container for the combined tracks */
      TrackCollection* combinedTrackCollection;

      /** container for the extrapolated track particles */
      xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer; 

      /** container for the extrapolated tracks */
      TrackCollection* extrapolatedTrackCollection;

      /** container for the extrapolated track particles */
      xAOD::TrackParticleContainer* msOnlyExtrapolatedTrackParticleContainer;

      /** container for the extrapolated tracks */
      TrackCollection* msOnlyExtrapolatedTrackCollection;

      /** container for the xAOD segments from MuGirl */
      xAOD::MuonSegmentContainer* xaodSegmentContainer; 

      /** collection for the segments from MuGirl */
      Trk::SegmentCollection* muonSegmentCollection;

      /** container for the Slow muon content */
      xAOD::SlowMuonContainer* slowMuonContainer; 

      /** container for the clusters associated with muons */
      xAOD::CaloClusterContainer* clusterContainer;
    };

    static const InterfaceID& interfaceID( ) ;


    /**IMuonCreatorTool interface: build muons from ID and MS candidates */    

    virtual void create( const MuonCandidateCollection* muonCandidates, const InDetCandidateCollection* inDetCandidates,
    			 OutputData& outputData ) const = 0;

    /** create a muon from a muon candidate */
    virtual xAOD::Muon* create( const MuonCandidate& candidate, OutputData& outputData ) const = 0;

    /** create a muon from a muon candidate */
    virtual xAOD::Muon* create( const InDetCandidate& candidate, OutputData& outputData ) const = 0;

  };

  inline const InterfaceID& IMuonCreatorTool::interfaceID()
    { 
      return IID_IMuonCreatorTool; 
    }

} // end of namespace

#endif 
