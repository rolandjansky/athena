/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSPRIVTXFINDER_ACTSPRIMARYVTXFINDERALG_H
#define ACTSPRIVTXFINDER_ACTSPRIMARYVTXFINDERALG_H

// ATHENA
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

// ACTS
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Geometry/GeometryID.hpp"
#include "Acts/Utilities/Helpers.hpp"

// PACKAGE
#include "ActsPriVtxFinderInterfaces/IActsPriVtxFinderTool.h"

// STL
#include <memory>
#include <vector>
#include <fstream>
#include <mutex>

namespace Trk
{
  //class IVertexMergingTool;
  //class IVertexCollectionSortingTool;
}


class ActsPrimaryVtxFinderAlg : public AthReentrantAlgorithm {
public:
  ActsPrimaryVtxFinderAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;

private:

  SG::ReadHandleKey<TrackCollection> m_trkTracksName{this,"TrkTracksName","Tracks","Trk::Track Collection used in Vertexing"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksName{this,"TracksName","InDetTrackParticles","xAOD::TrackParticle Collection used in Vertexing"};
  SG::WriteHandleKey<xAOD::VertexContainer> m_vxCandidatesOutputName{this,"VxCandidatesOutputName","PrimaryVertices","Output Vertex Collection"};

  ToolHandle< IActsPriVtxFinderTool > m_ActsVertexFinderTool;
  //ToolHandle<Trk::IVertexMergingTool > m_VertexMergingTool;
  //ToolHandle<Trk::IVertexCollectionSortingTool > m_VertexCollectionSortingTool;
  
  bool m_doVertexMerging;
  bool m_doVertexSorting;
  bool m_useTrackParticles;//use TrackParticles or Trk::Tracks as input

  
  mutable std::mutex m_writeMutex{};


};

#endif
