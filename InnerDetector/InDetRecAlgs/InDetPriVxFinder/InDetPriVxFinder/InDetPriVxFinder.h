/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetPriVxFinder.h  -  Description
                             -------------------
    begin   : 27-01-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :
              2016-04-26   David Shope <david.richard.shope@cern.ch>
              EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex

                findVertex in m_VertexFinderTool will now always return an xAOD::VertexContainer,
                even when using a TrackCollection or a TrackParticleBaseCollection
                as input.

 ***************************************************************************/

#ifndef INDETPRIVXFINDER_INDETPRIVXFINDER_H
#define INDETPRIVXFINDER_INDETPRIVXFINDER_H
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "TrkVertexFitterInterfaces/IVertexMergingTool.h"
#include "TrkVertexFitterInterfaces/IVertexCollectionSortingTool.h"

/** Primary Vertex Finder.
  InDetPriVxFinder uses the InDetPrimaryVertexFinderTool in the package
  InnerDetector/InDetRecTools/InDetPriVxFinderTool. It only gives the trackcollection from storegate to it
  and records the returned VxContainer.
 */

namespace InDet
{  
  class InDetPriVxFinder : public AthReentrantAlgorithm
  {
  public:
    InDetPriVxFinder(const std::string &name, ISvcLocator *pSvcLocator);
    
    virtual ~InDetPriVxFinder() = default;

    // Gaudi algorithm hooks
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:

    //Monitoring of the vertex variables
    void monitor_vertex( const std::string &prefix, xAOD::Vertex vertex ) const;

    SG::ReadHandleKey<TrackCollection> m_trkTracksName{this,"TrkTracksName","Tracks","Trk::Track Collection used in Vertexing"};
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksName{this,"TracksName","InDetTrackParticles","xAOD::TrackParticle Collection used in Vertexing"};
    SG::WriteHandleKey<xAOD::VertexContainer> m_vxCandidatesOutputName{this,"VxCandidatesOutputName","PrimaryVertices","Output Vertex Collection"};

    ToolHandle< IVertexFinder > m_VertexFinderTool{this, "VertexFinderTool", "", "Primary vertex finder tool"};
    ToolHandle<Trk::IVertexMergingTool > m_VertexMergingTool{this, "VertexMergingTool", "", "Vertex merging tool"};
    ToolHandle<Trk::IVertexCollectionSortingTool > m_VertexCollectionSortingTool{this, "VertexCollectionSortingTool", "", "Vertex collection sorting tool"};
    ToolHandle<GenericMonitoringTool> m_monTool{this, "PriVxMonTool", "", "Monitoring tool"};

    BooleanProperty m_doVertexMerging{this, "doVertexMerging", false, "Do vertex merging"};
    BooleanProperty m_doVertexSorting{this, "doVertexSorting", false, "Do vertex sorting"};
    BooleanProperty m_useTrackParticles{this, "useTrackParticles", true, "Use track particles as input"};
  };
}
#endif
