// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $
#ifndef BPHYSTOOLS_BPHYSTRACKVERTEXMAPTOOL_H
#define BPHYSTOOLS_BPHYSTRACKVERTEXMAPTOOL_H

// Framework includes
#include "BPhysTools/IBPhysTrackVertexMapTool.h"
#include "AsgTools/AsgTool.h"

// System include(s):
#include <memory>

// EDM includes
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

namespace xAOD {
  ///
  /// Dual-use tool createing a track-to-vertex map from
  /// the vertex-to-track information.
  ///
  /// Job options provided by this class:
  /// - VertexContainerName        -- name of container for secondary vertices
  /// - RefPVContainerName         -- name of container for refitted PVs
  /// - PVContainerName            -- name of container for primary vertices
  /// - TrackParticleContainerName -- name of container for TrackParticles
  /// - DebugTrkToVtxMaxEvents     -- Maximum number of events to produce
  ///                                 detailed log output for the
  ///                                 track-to-vertex association maps.
  ///                                 Set to -1 for infinity.
  /// - DumpPrefix                 -- Line prefix for log dump lines.
  /// - HypoName                   -- Hypothesis name
  ///                                 (for picking up inv. mass values)
  ///                                 May be a set of hypo names to be
  ///                                 tested, delimited by '|'.
  ///
  /// @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch>
  ///
  /// $Revision:$
  /// $Date: $
  ///
  class BPhysTrackVertexMapTool :
    public asg::AsgTool, virtual public xAOD::IBPhysTrackVertexMapTool {
    
    /// Declare the correct constructor for Athena
    ASG_TOOL_CLASS( BPhysTrackVertexMapTool, xAOD::IBPhysTrackVertexMapTool )
    
   public:
    /// Regular AsgTool constructor
    BPhysTrackVertexMapTool(const std::string& name =
			    "BPhysTrackVertexMapTool");

    /// Function initialising the tool
    virtual StatusCode initialize() override;
    
    /// Function being excuted for each event 
    virtual StatusCode logEvent() override;

    /// Function finalizing the tool
    virtual StatusCode finalize() override;

    /// Function indicating whether log counter allows logging of current event
    virtual bool doLog() const override;

    /// convenience method to wrap output lines by a prefix
    static std::string wrapLines(std::string lines, std::string prefix);

  protected:
    /// @name Functions to be called by user classes
    /// @{
    /// fill cache for current event
    virtual StatusCode cacheEvent() override;

    /// obtain primary vertices for a given ID track (may return empty vector)
    virtual std::vector<const xAOD::Vertex*>
    pvsForIDTrack(const xAOD::TrackParticle* track) const override;
    
    /// obtain refitted primary vertices for a given ID track
    /// (may return empty vector)
    virtual std::vector<const xAOD::Vertex*>
    refPVsForIDTrack(const xAOD::TrackParticle* track) const override;
    
    /// obtain secondary vertices for a given ID track (may return empty vector)
    virtual std::vector<const xAOD::Vertex*>
    svsForIDTrack(const xAOD::TrackParticle* track) const override;
    
    // track-vertex association related
    virtual std::string idTrackToString(const xAOD::TrackParticle* track,
					unsigned int indent=0,
					bool withPV=false,
					bool withRefPV=false,
					bool withSV=false) override;
    
    virtual std::string pvToString(const xAOD::Vertex* vtx,
				   unsigned int indent=0,
				   bool withTracks=false) override;
    
    virtual std::string refPVToString(const xAOD::Vertex* vtx,
				      unsigned int indent=0,
				      bool withTracks=false) override;
    virtual std::string svToString(const xAOD::Vertex* vtx,
				   unsigned int indent=0,
				   bool withTracks=false,
				   bool withMasses=false) override;
    virtual std::string idTracksToString(const xAOD::TrackParticleContainer*
					 tpc,
					 unsigned int indent=0,
					 bool withPV=false,
					 bool withRefPV=false,
					 bool withSV=false) override;
    
    virtual std::string pvsToString(const xAOD::VertexContainer* pvc,
				    unsigned int indent=0,
				    bool withTracks=false) override;
    virtual std::string refPVsToString(const xAOD::VertexContainer* rpvc,
				       unsigned int indent=0,
				       bool withTracks=false) override;
    virtual std::string svsToString(const xAOD::VertexContainer* svc,
				    unsigned int indent=0,
				    bool withTracks=false,
				    bool withMasses=false) override;
    virtual std::string summaryToString(std::string prefix) override;
    /// @}

  protected:
    virtual float getFloat(std::string name, const xAOD::Vertex* b);

    virtual std::vector<std::string> getTokens(std::string input,
					       std::string seperators);


  private:
    // track-vertex association related
    typedef std::map<const xAOD::TrackParticle*,
		     std::vector<const xAOD::Vertex*> > TrackToVertexMap_t;
    
    virtual void initTrackVertexMaps(const xAOD::TrackParticleContainer* tpc,
				     const xAOD::VertexContainer* pvc, 
				     const xAOD::VertexContainer* rpvc,
				     const xAOD::VertexContainer* svc);
    virtual void addVertexToTrackVertexMap(TrackToVertexMap_t& map,
					   const xAOD::TrackParticle* track,
					   const xAOD::Vertex* vtx);
    virtual std::string pvName(const xAOD::Vertex* vtx);
    virtual std::string refPVName(const xAOD::Vertex* vtx);
    virtual std::string svName(const xAOD::Vertex* vtx);
    virtual std::string idTrackName(const xAOD::TrackParticle* vtx);
    
  protected:      
    // job options
    std::string  m_vertexContainerName;
    std::string  m_refPVContainerName;
    std::string  m_pvContainerName;
    std::string  m_trackParticleContainerName;
    int          m_debugTrkToVtxMaxEvents;
    std::string  m_dumpPrefix;
    std::string  m_hypoName;
    
    // containers
    const xAOD::TrackParticleContainer*    m_tracks;
    const xAOD::TrackParticleAuxContainer* m_tracksAux;
    const xAOD::VertexContainer*           m_pvtxContainer;
    const xAOD::VertexContainer*           m_svtxContainer;
    const xAOD::VertexAuxContainer*        m_svtxAuxContainer;
    const xAOD::VertexContainer*           m_refPVContainer;
    const xAOD::VertexAuxContainer*        m_refPVAuxContainer;
    
    unsigned int m_nEvtsSeen;

    int          m_cachedRun;
    int          m_cachedEvent;
    
  private:
    // track-vertex association related
    typedef std::map<const xAOD::Vertex*, std::string> VertexNameMap_t;
    VertexNameMap_t m_pvNameMap;
    VertexNameMap_t m_refPVNameMap;
    VertexNameMap_t m_svNameMap;
    
    typedef std::map<const xAOD::TrackParticle*, std::string> TrackNameMap_t;
    TrackNameMap_t  m_idTrackNameMap;

    TrackToVertexMap_t m_idTrackToPVMap;
    TrackToVertexMap_t m_idTrackToRefPVMap;
    TrackToVertexMap_t m_idTrackToSVMap;
        
  }; // class BPhysTrackVertexMapTool

} // namespace xAOD

#endif // BPHYSTOOLS_BPHYSTRACKVERTEXMAPTOOL_H
