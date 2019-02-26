/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetSecVtxFinder.h  -  Description
                             -------------------
    begin   : Nov 10 2016
    authors : Lianyou SHAN ( IHEP-Beijing )
    Base    : InDetPriVxFinder, only an interface for reconstructed PriVtx to a SecVtxTool
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
 ***************************************************************************/

#ifndef INDETSECVXFINDER_INDETSECVXFINDER_H
#define INDETSECVXFINDER_INDETSECVXFINDER_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"


/* Forward declarations */

namespace Trk
{
  class IVertexMergingTool;
//  class IVertexCollectionSortingTool;
  class IVxCandidateXAODVertex;  
}

namespace InDet
{
  class ISecVertexFinder;
  
  class InDetSecVtxFinder : public AthAlgorithm
  {
  public:
    InDetSecVtxFinder(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~InDetSecVtxFinder();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
  private: 
    SG::ReadHandleKey<TrackCollection> m_trkTracksName{this,"TrkTracksName","Tracks","Trk::Track Collection used in Vertexing"}; 
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksName{this,"TracksName","InDetTrackParticles","xAOD::TrackParticle Collection used in Vertexing"};
    SG::WriteHandleKey<xAOD::VertexContainer> m_vxCandidatesOutputName{this,"VxCandidatesOutputName","InclusiveSecVtx","Output Vertex Collection"};
    SG::ReadHandleKey<xAOD::VertexContainer> m_PrimaryVxInputName{this,"PrimaryVertexInputName","PrimaryVertices","Input Vertex Collection"};
      
    ToolHandle< ISecVertexFinder > m_VertexFinderTool;
    ToolHandle<Trk::IVertexMergingTool > m_VertexMergingTool;
    // ToolHandle<Trk::IVertexCollectionSortingTool > m_VertexCollectionSortingTool;
    
    bool m_doVertexMerging;
    bool m_useTrackParticles; //use TrackParticles or Trk::Tracks as input

    // for summary output at the end
    unsigned int m_numEventsProcessed;
    unsigned int m_totalNumVerticesWithoutDummy;
  };
}
#endif
