// This is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PROMPT_IVERTEXMERGINGTOOL_H
#define PROMPT_IVERTEXMERGINGTOOL_H

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

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// xAOD
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace Prompt
{
  //======================================================================================================
  // Vertex cluster helper class
  //
  struct VtxCluster
  {
    VtxCluster():vtx_merged(0) {}

    std::vector<const xAOD::Vertex *>          vtxs_init;
    std::vector<xAOD::Vertex *>                vtxs_fitted_bad;

    std::vector<const xAOD::TrackParticle *>   trks_init;
    std::vector<const xAOD::TrackParticle *>   trks_curr;
    
    xAOD::Vertex                              *vtx_merged;
  };

  //======================================================================================================
  struct MergeResult
  {
    std::vector<xAOD::Vertex *> vtxs_new_merged;

    std::vector<xAOD::Vertex *> vtxs_init_passed;
    std::vector<xAOD::Vertex *> vtxs_init_passed_not_merged;
  };

  struct FittingInput;

  //======================================================================================================
  // Interface of Vertex Merging Tool
  //
  static const InterfaceID IPrompt_VertexMergingTool("Prompt::VertexMergingTool", 1, 0);
  
  class IVertexMergingTool: public virtual IAlgTool
  {
  public:

    static const InterfaceID& interfaceID() { return IPrompt_VertexMergingTool; }

    virtual MergeResult mergeInitVertices(const FittingInput &input,
					  const xAOD::TrackParticle *tracklep, 
					  const std::vector<xAOD::Vertex *> &init_vtxs,
					  const std::vector<const xAOD::TrackParticle *> &selected_tracks) = 0;
  };
}

#endif 
