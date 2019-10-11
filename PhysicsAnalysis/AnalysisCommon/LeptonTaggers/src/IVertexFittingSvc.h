// This is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PROMPT_IVERTEXFITTINGSVC_H
#define PROMPT_IVERTEXFITTINGSVC_H

/**********************************************************************************
 * @Package: LeptonTaggers
 * @Class  : VertexFittingSvc
 * @Author : Fudong He
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Fit the input ID tracks and produced the output vertex. 
 *
 *  VertexFittingSvc instance owns created vertexes unless the ownership is
 *  transfered to a vertex container via moveVertexPointerToContainer() function.
 * 
 **********************************************************************************/

// C++
#include <string>
#include <vector>

// Athena
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IService.h"
#include "AthenaBaseComps/AthService.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"

// xAOD
#include "xAODTracking/VertexContainer.h"

namespace Prompt
{
  enum VtxType {
    kTwoTrackVtx                  = 1,
    kSimpleMergedVtx              = 2,
    kDeepMergedVtx                = 3,
    kIterativeFitVtx              = 4,
    kTwoTrackVtxWithoutLepton     = 5,
    kIterativeFitVtxWithoutLepton = 6,
    kRefittedPriVtx               = 10,
    kRefittedPriVtxWithoutLep     = 11
  };

  struct FittingInput
  {
    FittingInput():inDetTracks(0), priVtx(0), refittedPriVtx(0), refittedPriVtxWithoutLep(0) {}

    FittingInput(const xAOD::TrackParticleContainer *inDetTracks_p,
		 const xAOD::Vertex                 *priVtx_p,
		 const xAOD::Vertex                 *refittedPriVtx_p):
      inDetTracks               (inDetTracks_p), 
      priVtx                    (priVtx_p), 
      refittedPriVtx            (refittedPriVtx_p), 
      refittedPriVtxWithoutLep(0) {}

    const xAOD::TrackParticleContainer *inDetTracks;
    const xAOD::Vertex                 *priVtx;
    const xAOD::Vertex                 *refittedPriVtx;
    const xAOD::Vertex                 *refittedPriVtxWithoutLep;
    
  };

  //
  // Interface of Vertex Merging Tool
  //   
  class IVertexFittingSvc: public virtual IService
  {
  public:

    DeclareInterfaceID( Prompt::IVertexFittingSvc, 1, 0 );

    virtual ~IVertexFittingSvc() {}

    virtual std::pair<xAOD::Vertex*, int> fitVertexWithPrimarySeed(const FittingInput &input,
								   const std::vector<const xAOD::TrackParticle* > &tracks,
								   VtxType vtx) = 0;

    virtual std::pair<xAOD::Vertex*, int> fitVertexWithSeed(const FittingInput &input, 
							    const std::vector<const xAOD::TrackParticle* > &tracks,
							    const Amg::Vector3D& seed,
							    VtxType vtxType) = 0;

    virtual bool moveVertexPointerToContainer(xAOD::Vertex *vtx, xAOD::VertexContainer &container) = 0;

    virtual void resetEvent() = 0;

    virtual bool isValidVertex(const xAOD::Vertex *vtx) const = 0;
  };
}

#endif 
