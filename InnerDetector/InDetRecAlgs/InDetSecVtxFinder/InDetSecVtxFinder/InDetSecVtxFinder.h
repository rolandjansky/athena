/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetSecVtxFinder.h  -  Description
                             -------------------
    begin   : Nov 10 2016
    authors : Lianyou SHAN ( IHEP-Beijing ) 
    Base    : InDetPriVxFinder, only an interface for reconstructed PriVtx to a SecVtxTool
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    author of most recent changes : Neža Ribarič (Lancaster University, UK)
    changes : Added functionality to call multiple Finder Tools 
 ***************************************************************************/
#include "CxxUtils/checker_macros.h"

#ifndef INDETSECVXFINDER_INDETSECVXFINDER_H
#define INDETSECVXFINDER_INDETSECVXFINDER_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"


#include "InDetRecToolInterfaces/IInDetIterativeSecVtxFinderTool.h"
#include "InDetRecToolInterfaces/IInDetAdaptiveMultiSecVtxFinderTool.h"
#include "TrkVertexFitterInterfaces/IVertexMergingTool.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"

/* Forward declarations */


namespace InDet
{
  class ISecVertexFinder;
  class IAdaptiveMultiSecVertexFinder;
  
  class InDetSecVtxFinder : public AthAlgorithm
  {
  public:
    InDetSecVtxFinder(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~InDetSecVtxFinder() = default;
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
  private: 
    SG::ReadHandleKey<TrackCollection> m_inputTrackCollection{this,"inputTrackCollection","Tracks","Trk::Track Collection used in Vertexing"}; 
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_inputTrackParticles{this,"inputTrackParticles","InDetTrackParticles","xAOD::TrackParticle Container used in Vertexing"};
    SG::WriteHandleKey<xAOD::VertexContainer> m_outputSecondaryVertices{this,"outputSecondaryVertices","AdaptiveMultiSecVtx","Output Secondary Vertex Container"};
    SG::ReadHandleKey<xAOD::VertexContainer> m_inputPrimaryVertices{this,"inputPrimaryVertices","PrimaryVertices","Input Primary Vertex Container"};
    
    ToolHandle< ISecVertexFinder > m_InclusiveVertexFinderTool{this, "InclusiveVertexFinderTool", "", "inclusive secondary vertex finder tool"};
    ToolHandle<InDet::IAdaptiveMultiSecVertexFinder> m_AdaptiveMultiVertexFinderTool{this, "AdaptiveVertexFinderTool", "", "adaptive multi secondary vertex finder tool"};
    ToolHandle<Trk::IVertexMergingTool > m_VertexMergingTool{this, "VertexMergingTool", "", "vertex merging tool"};
    
    BooleanProperty m_doVertexMerging{this, "doVertexMerging", false, "Do vertex merging"};
    BooleanProperty m_useTrackParticles{this, "useTrackParticles", false, "use TrackParticles"};

    StringProperty m_FinderTool{ this, "AMVF", "vertexing tool to use: AMVF, ISV" };

    
    // for summary output at the end
    unsigned int m_numEventsProcessed;
    unsigned int m_totalNumVerticesWithoutDummy;
  };
}
#endif
