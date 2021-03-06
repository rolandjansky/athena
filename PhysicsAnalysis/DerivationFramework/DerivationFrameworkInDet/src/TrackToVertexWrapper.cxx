/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackToVertexWrapper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkInDet/TrackToVertexWrapper.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include <vector>
#include <string>
#include "StoreGate/WriteDecorHandle.h"
#include "DerivationFrameworkInDet/DecoratorUtils.h"

namespace DerivationFramework {

  TrackToVertexWrapper::TrackToVertexWrapper(const std::string& t,
      const std::string& n,
      const IInterface* p) :
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }

  StatusCode TrackToVertexWrapper::initialize()
  {
     if (m_sgName.empty()) {
      ATH_MSG_ERROR("No decoration prefix name provided for the output of TracktoVertexWrapper!");
      return StatusCode::FAILURE;
    }
    if (m_containerName.key().empty()) {
      ATH_MSG_ERROR("No TrackParticle collection provided for TracktoVertexWrapper!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK(m_containerName.initialize() );
    ATH_CHECK(m_tool.retrieve());
    ATH_CHECK(m_vertexKey.initialize());
    {
      std::vector<std::string> names;
      names.resize(kNFloatDecor);
      names[kdecnD0Decor]                 = "unbiased_d0";
      names[kdecnZ0Decor]                 = "unbiased_z0";
      names[kdecnZ0SinThetaDecor]         = "unbiased_z0SinTheta";
      names[kdecnD0ErrDecor]              = "unbiased_d0Sigma";
      names[kdecnZ0ErrDecor]              = "unbiased_z0Sigma";
      names[kdecnZ0SinThetaErrDecor]      = "unbiased_z0SigmaSinTheta";
      names[kdecnPVD0ErrDecor]            = "unbiased_PVd0Sigma";
      names[kdecnPVZ0ErrDecor]            = "unbiased_PVz0Sigma";
      names[kdecnPVZ0SinThetaErrDecor]    = "unbiased_PVz0SigmaSinTheta";
      names[kdecn_b_D0Decor]              = "biased_d0";
      names[kdecn_b_Z0Decor]              = "biased_z0";
      names[kdecn_b_Z0SinThetaDecor]      = "biased_z0SinTheta";
      names[kdecn_b_D0ErrDecor]           = "biased_d0Sigma";
      names[kdecn_b_Z0ErrDecor]           = "biased_z0Sigma";
      names[kdecn_b_Z0SinThetaErrDecor]   = "biased_z0SigmaSinTheta";
      names[kdecn_b_PVD0ErrDecor]         = "biased_PVd0Sigma";
      names[kdecn_b_PVZ0ErrDecor]         = "biased_PVz0Sigma";
      names[kdecn_b_PVZ0SinThetaErrDecor] = "biased_PVz0SigmaSinTheta";
      createDecoratorKeys(*this,m_containerName, m_sgName+"_", names, m_trackFloatDecorKeys);
    }
    return StatusCode::SUCCESS;
  }

  StatusCode TrackToVertexWrapper::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode TrackToVertexWrapper::addBranches() const
  {
    const EventContext& ctx = Gaudi::Hive::currentContext();

    // retrieve track container
    SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_containerName, ctx );
    if( ! tracks.isValid() ) {
      ATH_MSG_ERROR ("Couldn't retrieve TrackParticles with key: " << m_containerName.key() );
      return StatusCode::FAILURE;
    }
    SG::ReadHandle<xAOD::VertexContainer> vertices { m_vertexKey, ctx };
    if ( !vertices.isValid() )
    {
      ATH_MSG_ERROR ("Couldn't retrieve Vertices with key: " << m_vertexKey.key());
      return StatusCode::FAILURE;
    }

    std::vector<SG::WriteDecorHandle<xAOD::TrackParticleContainer,float> >
      track_decorators = createDecorators<xAOD::TrackParticleContainer,float>(m_trackFloatDecorKeys,ctx);
    // Run tool for each element and calculate the impact parameters/errors 
    for (xAOD::TrackParticleContainer::const_iterator trItr = tracks->begin(); trItr!=tracks->end(); ++trItr) {
      std::unique_ptr<const Trk::ImpactParametersAndSigma> iPandSigma;
      std::unique_ptr<const Trk::ImpactParametersAndSigma> iPandSigmaBiased;
      const xAOD::Vertex* foundVertex { nullptr };
      for (const auto vx : *vertices) {
        for (const auto& tpLink : vx->trackParticleLinks()) {
          if (*tpLink == *trItr) {
            foundVertex = vx;
            break;
          }
        }
        if (foundVertex) break;
      }
      if ( foundVertex ) {
        iPandSigma.reset(m_tool->estimate(*trItr,foundVertex,true));
        iPandSigmaBiased.reset(m_tool->estimate(*trItr,foundVertex,false));
        if( !iPandSigma )       ATH_MSG_WARNING ("trackToVertexIPEstimator failed !");
        if( !iPandSigmaBiased ) ATH_MSG_WARNING ("trackToVertexIPEstimator biased IP failed !");
      }
      else {
        ATH_MSG_DEBUG("No vertex associated to the track. Skipping it.");
      }

      // Do the decoration for each track
      if (iPandSigma) {
        track_decorators[kdecnD0Decor]( **trItr )              = iPandSigma->IPd0;
        track_decorators[kdecnZ0Decor]( **trItr )              = iPandSigma->IPz0;
        track_decorators[kdecnZ0SinThetaDecor]( **trItr )      = iPandSigma->IPz0SinTheta;
        track_decorators[kdecnD0ErrDecor]( **trItr )           = iPandSigma->sigmad0;
        track_decorators[kdecnZ0ErrDecor]( **trItr )           = iPandSigma->sigmaz0;
        track_decorators[kdecnZ0SinThetaErrDecor]( **trItr )   = iPandSigma->sigmaz0SinTheta;
        track_decorators[kdecnPVD0ErrDecor] (**trItr )         = iPandSigma->PVsigmad0;
        track_decorators[kdecnPVZ0ErrDecor] (**trItr )         = iPandSigma->PVsigmaz0;
        track_decorators[kdecnPVZ0SinThetaErrDecor] (**trItr ) = iPandSigma->PVsigmaz0SinTheta;
      }
      else {
        track_decorators[kdecnD0Decor]( **trItr )              = 999.;
        track_decorators[kdecnZ0Decor]( **trItr )              = 999.;
        track_decorators[kdecnZ0SinThetaDecor]( **trItr )      = 999.;
        track_decorators[kdecnD0ErrDecor]( **trItr )           = 999.;
        track_decorators[kdecnZ0ErrDecor]( **trItr )           = 999.;
        track_decorators[kdecnZ0SinThetaErrDecor]( **trItr )   = 999.;
        track_decorators[kdecnPVD0ErrDecor] (**trItr )         = 999.;
        track_decorators[kdecnPVZ0ErrDecor] (**trItr )         = 999.;
        track_decorators[kdecnPVZ0SinThetaErrDecor] (**trItr ) = 999.;
      }

      if (iPandSigmaBiased) {
        track_decorators[kdecn_b_D0Decor]( **trItr )              = iPandSigmaBiased->IPd0;
        track_decorators[kdecn_b_Z0Decor]( **trItr )              = iPandSigmaBiased->IPz0;
        track_decorators[kdecn_b_Z0SinThetaDecor]( **trItr )      = iPandSigmaBiased->IPz0SinTheta;
        track_decorators[kdecn_b_D0ErrDecor]( **trItr )           = iPandSigmaBiased->sigmad0;
        track_decorators[kdecn_b_Z0ErrDecor]( **trItr )           = iPandSigmaBiased->sigmaz0;
        track_decorators[kdecn_b_Z0SinThetaErrDecor]( **trItr )   = iPandSigmaBiased->sigmaz0SinTheta;
        track_decorators[kdecn_b_PVD0ErrDecor] (**trItr )         = iPandSigmaBiased->PVsigmad0;
        track_decorators[kdecn_b_PVZ0ErrDecor] (**trItr )         = iPandSigmaBiased->PVsigmaz0;
        track_decorators[kdecn_b_PVZ0SinThetaErrDecor] (**trItr ) = iPandSigmaBiased->PVsigmaz0SinTheta;
      }
      else {
        track_decorators[kdecn_b_D0Decor]( **trItr )              = 999.;
        track_decorators[kdecn_b_Z0Decor]( **trItr )              = 999.;
        track_decorators[kdecn_b_Z0SinThetaDecor]( **trItr )      = 999.;
        track_decorators[kdecn_b_D0ErrDecor]( **trItr )           = 999.;
        track_decorators[kdecn_b_Z0ErrDecor]( **trItr )           = 999.;
        track_decorators[kdecn_b_Z0SinThetaErrDecor]( **trItr )   = 999.;
        track_decorators[kdecn_b_PVD0ErrDecor] (**trItr )         = 999.;
        track_decorators[kdecn_b_PVZ0ErrDecor] (**trItr )         = 999.;
        track_decorators[kdecn_b_PVZ0SinThetaErrDecor] (**trItr ) = 999.;
      }
    } // end of loop over tracks          
    
    return StatusCode::SUCCESS;
  }  

}
