// This is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VERTEXMERGINGTOOL_H
#define VERTEXMERGINGTOOL_H

/**********************************************************************************
 * @Package: LeptonTaggers
 * @Class  : VertexMergingTool
 * @Author : Fudong He
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Merge the input vertices and output merged vertices.
 * 
 **********************************************************************************/

// Local
#include "IVertexFittingSvc.h"
#include "IVertexMergingTool.h"

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// xAOD
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace Prompt
{
  //
  // Vertex Merging Tool
  //
  class VertexMergingTool: public AthAlgTool, virtual public IVertexMergingTool
  {
  public:

    VertexMergingTool(const std::string &name,
                      const std::string &type,
                      const IInterface  *parent);

    virtual ~VertexMergingTool() {}

    virtual StatusCode initialize(void);

    virtual MergeResult mergeInitVertices(const FittingInput &input,
					  const xAOD::TrackParticle *tracklep,
					  const std::vector<xAOD::Vertex *> &init_vtxs,
					  const std::vector<const xAOD::TrackParticle *> &selected_tracks);

  private: 

    bool passVertexSelection(const xAOD::Vertex *vtx) const;

    bool makeClusters(std::vector<VtxCluster> &clusters, std::vector<xAOD::Vertex *> &init_vtxs);

    bool matchVtxToCluster(const VtxCluster &cluster, const xAOD::Vertex *vtx) const;

    bool addInitVtxToCluster(VtxCluster &cluster, const xAOD::Vertex *vtx) const;

    bool fitVertexCluster(const FittingInput &input,
			  const xAOD::TrackParticle *tracklep, 
			  VtxCluster &cluster);

    double getMinNormDistVtx(const xAOD::Vertex *vtx1, const xAOD::Vertex *vtx2) const;

  private:

    //
    // Properties:
    //
    ServiceHandle<Prompt::IVertexFittingSvc>               m_vertexFitterSvc;
    
    bool                                                   m_debug;
    bool                                                   m_useMinNormDist;

    double                                                 m_minFitProb;    
    double                                                 m_minDistanceClusterVtx;
  };
}

#endif 
