/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

/** Primary Vertex Finder.
  InDetPriVxFinder uses the InDetPrimaryVertexFinderTool in the package
  InnerDetector/InDetRecTools/InDetPriVxFinderTool. It only gives the trackcollection from storegate to it
  and records the returned VxContainer.
 */

/* Forward declarations */

namespace Trk
{
  class IVertexMergingTool;
  class IVertexCollectionSortingTool;
}

namespace InDet
{
  class IVertexFinder;
  
  class InDetPriVxFinder : public AthAlgorithm
  {
  public:
    InDetPriVxFinder(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~InDetPriVxFinder();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
  private:
    SG::ReadHandleKey<TrackCollection> m_trkTracksName{this,"TrkTracksName","Tracks","Trk::Track Collection used in Vertexing"};
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksName{this,"TracksName","InDetTrackParticles","xAOD::TrackParticle Collection used in Vertexing"};
    SG::WriteHandleKey<xAOD::VertexContainer> m_vxCandidatesOutputName{this,"VxCandidatesOutputName","PrimaryVertices","Output Vertex Collection"};

    ToolHandle< IVertexFinder > m_VertexFinderTool;
    ToolHandle<Trk::IVertexMergingTool > m_VertexMergingTool;
    ToolHandle<Trk::IVertexCollectionSortingTool > m_VertexCollectionSortingTool;
    
    bool m_doVertexMerging;
    bool m_doVertexSorting;
    bool m_useTrackParticles;//use TrackParticles or Trk::Tracks as input

    // for summary output at the end
    unsigned int m_numEventsProcessed;
    unsigned int m_totalNumVerticesWithoutDummy;
  };
}
#endif
