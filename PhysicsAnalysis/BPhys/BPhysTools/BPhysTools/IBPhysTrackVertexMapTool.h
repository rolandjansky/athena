// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $
#ifndef BPHYSTOOLS_IBPHYSTRACKVERTEXMAPTOOL_H
#define BPHYSTOOLS_IBPHYSTRACKVERTEXMAPTOOL_H

// Framework includes
#include "AsgTools/IAsgTool.h"

// System include(s):
#include <string>
#include <memory>
#include <vector>

// EDM includes
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

namespace xAOD {
  ///
  /// Interface for dual-use tool createing a track-to-vertex map from
  /// the vertex-to-track information.
  ///
  /// @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch>
  ///
  /// $Revision:$
  /// $Date: $
  ///
  class IBPhysTrackVertexMapTool : virtual public asg::IAsgTool {

  public:
    /// Declare the correct interface for Athena
    ASG_TOOL_INTERFACE( xAOD::IBPhysTrackVertexMapTool )

    /// Function being excuted for each event 
    virtual StatusCode logEvent() = 0;

    /// Function finalizing the tool
    virtual StatusCode finalize() = 0;

    /// Function indicating whether log counter allows logging of current event
    virtual bool doLog() const = 0;

  public:
    /// @name Functions to be called by user classes
    /// @{
    /// fill cache for current event
    virtual StatusCode cacheEvent() = 0;

    /// obtain primary vertices for a given ID track (may return empty vector)
    virtual std::vector<const xAOD::Vertex*>
    pvsForIDTrack(const xAOD::TrackParticle* track) const = 0;
    
    /// obtain refitted primary vertices for a given ID track
    /// (may return empty vector)
    virtual std::vector<const xAOD::Vertex*>
    refPVsForIDTrack(const xAOD::TrackParticle* track) const = 0;
    
    /// obtain secondary vertices for a given ID track (may return empty vector)
    virtual std::vector<const xAOD::Vertex*>
    svsForIDTrack(const xAOD::TrackParticle* track) const = 0;
    
    // track-vertex association related
    virtual std::string idTrackToString(const xAOD::TrackParticle* track,
					unsigned int indent=0,
					bool withPV=false,
					bool withRefPV=false,
					bool withSV=false) = 0;
    
    virtual std::string pvToString(const xAOD::Vertex* vtx,
				   unsigned int indent=0,
				   bool withTracks=false) = 0;
    
    virtual std::string refPVToString(const xAOD::Vertex* vtx,
				      unsigned int indent=0,
				      bool withTracks=false) = 0;
    virtual std::string svToString(const xAOD::Vertex* vtx,
				   unsigned int indent=0,
				   bool withTracks=false,
				   bool withMasses=false) = 0;
    virtual std::string idTracksToString(const xAOD::TrackParticleContainer*
					 tpc,
					 unsigned int indent=0,
					 bool withPV=false,
					 bool withRefPV=false,
					 bool withSV=false) = 0;
    
    virtual std::string pvsToString(const xAOD::VertexContainer* pvc,
				    unsigned int indent=0,
				    bool withTracks=false) = 0;
    virtual std::string refPVsToString(const xAOD::VertexContainer* rpvc,
				       unsigned int indent=0,
				       bool withTracks=false) =0;
    virtual std::string svsToString(const xAOD::VertexContainer* svc,
				    unsigned int indent=0,
				    bool withTracks=false,
				    bool withMasses=false) = 0;
    virtual std::string summaryToString(std::string prefix) = 0;
    /// @}

  }; // class IBPhysTrackVertexMapTool

} // namespace xAOD

#endif // BPHYSTOOLS_IBPHYSTRACKVERTEXMAPTOOL_H
