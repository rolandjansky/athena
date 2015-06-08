/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackToVertexWrapper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkInDet/TrackToVertexWrapper.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  TrackToVertexWrapper::TrackToVertexWrapper(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_tool("Trk::TrackToVertexIPEstimator"),
    m_sgName(""),
    m_containerName("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TrackToVertexIPEstimator", m_tool);
    declareProperty("DecorationPrefix", m_sgName);
    declareProperty("ContainerName", m_containerName);
  }

  StatusCode TrackToVertexWrapper::initialize()
  {
    if (m_sgName=="") {
      ATH_MSG_ERROR("No decoration prefix name provided for the output of TracktoVertexWrapper!");
      return StatusCode::FAILURE;
    }
    if (m_containerName=="") {
      ATH_MSG_ERROR("No TrackParticle collection provided for TracktoVertexWrapper!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK(m_tool.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode TrackToVertexWrapper::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode TrackToVertexWrapper::addBranches() const
  {

    // retrieve track container
    const xAOD::TrackParticleContainer* tracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >( m_containerName );
    if( ! tracks ) {
        ATH_MSG_ERROR ("Couldn't retrieve TrackParticles with key: " << m_containerName );
        return StatusCode::FAILURE;
    }
    // Run tool for each element and calculate the impact parameters/errors 
    for (xAOD::TrackParticleContainer::const_iterator trItr = tracks->begin(); trItr!=tracks->end(); ++trItr) {
      const Trk::ImpactParametersAndSigma* iPandSigma(NULL);
      const Trk::ImpactParametersAndSigma* iPandSigmaBiased(NULL);
      if ( (*trItr)->vertex() ) {
	iPandSigma = m_tool->estimate(*trItr,(*trItr)->vertex(),true);
	iPandSigmaBiased = m_tool->estimate(*trItr,(*trItr)->vertex(),false);
	if( iPandSigma==0 ) ATH_MSG_WARNING ("trackToVertexIPEstimator failed !");
	if( iPandSigmaBiased==0 ) ATH_MSG_WARNING ("trackToVertexIPEstimator biased IP failed !");
      } else {
	ATH_MSG_DEBUG("No vertex associated to the track. Skipping it.");
      }
      // Do the decoration for each track
      SG::AuxElement::Decorator< float > decnD0(m_sgName+"_unbiased_d0");
      SG::AuxElement::Decorator< float > decnZ0(m_sgName+"_unbiased_z0");
      SG::AuxElement::Decorator< float > decnZ0SinTheta(m_sgName+"_unbiased_z0SinTheta");
      SG::AuxElement::Decorator< float > decnD0Err(m_sgName+"_unbiased_d0Sigma");
      SG::AuxElement::Decorator< float > decnZ0Err(m_sgName+"_unbiased_z0Sigma");
      SG::AuxElement::Decorator< float > decnZ0SinThetaErr(m_sgName+"_unbiased_z0SigmaSinTheta");
      SG::AuxElement::Decorator< float > decnPVD0Err(m_sgName+"_unbiased_PVd0Sigma");
      SG::AuxElement::Decorator< float > decnPVZ0Err(m_sgName+"_unbiased_PVz0Sigma");
      SG::AuxElement::Decorator< float > decnPVZ0SinThetaErr(m_sgName+"_unbiased_PVz0SigmaSinTheta");
      if (iPandSigma) {
	decnD0( **trItr ) = iPandSigma->IPd0;
	decnZ0( **trItr ) = iPandSigma->IPz0;
	decnZ0SinTheta( **trItr ) = iPandSigma->IPz0SinTheta;
	decnD0Err( **trItr ) = iPandSigma->sigmad0;
	decnZ0Err( **trItr ) = iPandSigma->sigmaz0;
	decnZ0SinThetaErr( **trItr ) = iPandSigma->sigmaz0SinTheta;
	decnPVD0Err (**trItr ) = iPandSigma->PVsigmad0;
	decnPVZ0Err (**trItr ) = iPandSigma->PVsigmaz0;
	decnPVZ0SinThetaErr (**trItr ) = iPandSigma->PVsigmaz0SinTheta;
	delete iPandSigma;
      } else {
	decnD0( **trItr ) = 999.;
	decnZ0( **trItr ) = 999.;
	decnZ0SinTheta( **trItr ) = 999.;
	decnD0Err( **trItr ) = 999.;
	decnZ0Err( **trItr ) = 999.;
	decnZ0SinThetaErr( **trItr ) = 999.;
	decnPVD0Err (**trItr ) = 999.;
	decnPVZ0Err (**trItr ) = 999.;
	decnPVZ0SinThetaErr (**trItr ) = 999.;
      }

      SG::AuxElement::Decorator< float > decn_b_D0(m_sgName+"_biased_d0");
      SG::AuxElement::Decorator< float > decn_b_Z0(m_sgName+"_biased_z0");
      SG::AuxElement::Decorator< float > decn_b_Z0SinTheta(m_sgName+"_biased_z0SinTheta");
      SG::AuxElement::Decorator< float > decn_b_D0Err(m_sgName+"_biased_d0Sigma");
      SG::AuxElement::Decorator< float > decn_b_Z0Err(m_sgName+"_biased_z0Sigma");
      SG::AuxElement::Decorator< float > decn_b_Z0SinThetaErr(m_sgName+"_biased_z0SigmaSinTheta");
      SG::AuxElement::Decorator< float > decn_b_PVD0Err(m_sgName+"_biased_PVd0Sigma");
      SG::AuxElement::Decorator< float > decn_b_PVZ0Err(m_sgName+"_biased_PVz0Sigma");
      SG::AuxElement::Decorator< float > decn_b_PVZ0SinThetaErr(m_sgName+"_biased_PVz0SigmaSinTheta");
      if (iPandSigmaBiased) {
	decn_b_D0( **trItr ) = iPandSigmaBiased->IPd0;
	decn_b_Z0( **trItr ) = iPandSigmaBiased->IPz0;
	decn_b_Z0SinTheta( **trItr ) = iPandSigmaBiased->IPz0SinTheta;
	decn_b_D0Err( **trItr ) = iPandSigmaBiased->sigmad0;
	decn_b_Z0Err( **trItr ) = iPandSigmaBiased->sigmaz0;
	decn_b_Z0SinThetaErr( **trItr ) = iPandSigmaBiased->sigmaz0SinTheta;
	decn_b_PVD0Err (**trItr ) = iPandSigmaBiased->PVsigmad0;
	decn_b_PVZ0Err (**trItr ) = iPandSigmaBiased->PVsigmaz0;
	decn_b_PVZ0SinThetaErr (**trItr ) = iPandSigmaBiased->PVsigmaz0SinTheta;
	delete iPandSigmaBiased;
      } else {
	decn_b_D0( **trItr ) = 999.;
	decn_b_Z0( **trItr ) = 999.;
	decn_b_Z0SinTheta( **trItr ) = 999.;
	decn_b_D0Err( **trItr ) = 999.;
	decn_b_Z0Err( **trItr ) = 999.;
	decn_b_Z0SinThetaErr( **trItr ) = 999.;
	decn_b_PVD0Err (**trItr ) = 999.;
	decn_b_PVZ0Err (**trItr ) = 999.;
	decn_b_PVZ0SinThetaErr (**trItr ) = 999.;
      }
    } // end of loop over tracks		 	 	  
    
    return StatusCode::SUCCESS;
  }  

}
