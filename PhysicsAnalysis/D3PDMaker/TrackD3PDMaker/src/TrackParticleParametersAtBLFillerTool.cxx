/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleParametersAtBLFillerTool.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "AthenaKernel/errorcheck.h"
#include "TrkParameters/TrackParameters.h"
#include "Particle/TrackParticle.h"

namespace D3PD {


TrackParticleParametersAtBLFillerTool::TrackParticleParametersAtBLFillerTool (const std::string& type,
              const std::string& name,
              const IInterface* parent)
  : D3PD::BlockFillerTool<Rec::TrackParticle> (type, name, parent),
    m_trackToVertexTool("Reco::TrackToVertex")
{
  // Avoid coverity warnings.
  m_levelOfDetails = 10;
  m_fillThetaAndQoverP = true;
  book().ignore();

  declareProperty ("FillThetaAndQoverP", m_fillThetaAndQoverP=true);
  declareProperty ("DefaultValue", m_defaultValue=0,
                   "Default value to which to set unfilled variables.");
  declareProperty ("TrackToVertexTool", m_trackToVertexTool);
  declareProperty ("levelOfDetails", m_levelOfDetails);
}

StatusCode TrackParticleParametersAtBLFillerTool::initialize(){

  CHECK( D3PD::BlockFillerTool<Rec::TrackParticle>::initialize() );

  StatusCode sc = m_trackToVertexTool.retrieve();
  if(sc.isFailure()){
    REPORT_MESSAGE (MSG::ERROR) << "Could not retrieve TrackToVertexTool";
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrackParticleParametersAtBLFillerTool::book()
{
  // track parameters at perigee
  if(m_levelOfDetails > 0) {
    CHECK( addVariable ("d0_wrtBL", m_d0, "", m_defaultValue) );
    CHECK( addVariable ("z0_wrtBL", m_z0, "", m_defaultValue) );
    CHECK( addVariable ("phi_wrtBL", m_phi, "", m_defaultValue) );
    if(m_fillThetaAndQoverP) {
      CHECK( addVariable ("theta_wrtBL", m_theta, "", m_defaultValue) );
      CHECK( addVariable ("qoverp_wrtBL", m_qoverp, "", m_defaultValue) );
    }

    if(m_levelOfDetails > 1) {

      CHECK( addVariable ("d0_err_wrtBL", m_d0_err, "", m_defaultValue) );
      CHECK( addVariable ("z0_err_wrtBL", m_z0_err, "", m_defaultValue) );
      CHECK( addVariable ("phi_err_wrtBL", m_phi_err, "", m_defaultValue) );
      CHECK( addVariable ("theta_err_wrtBL", m_theta_err, "", m_defaultValue) );
      CHECK( addVariable ("qoverp_err_wrtBL", m_qoverp_err, "", m_defaultValue) );

      if(m_levelOfDetails > 2) {

        CHECK( addVariable ("d0_z0_err_wrtBL", m_d0_z0_err_wrtBL, "", m_defaultValue) );
        CHECK( addVariable ("d0_phi_err_wrtBL", m_d0_phi_err_wrtBL, "", m_defaultValue) );
        CHECK( addVariable ("d0_theta_err_wrtBL", m_d0_theta_err_wrtBL, "", m_defaultValue) );
        CHECK( addVariable ("d0_qoverp_err_wrtBL", m_d0_qoverp_err_wrtBL, "", m_defaultValue) );
        CHECK( addVariable ("z0_phi_err_wrtBL", m_z0_phi_err_wrtBL, "", m_defaultValue) );
        CHECK( addVariable ("z0_theta_err_wrtBL", m_z0_theta_err_wrtBL, "", m_defaultValue) );
        CHECK( addVariable ("z0_qoverp_err_wrtBL", m_z0_qoverp_err_wrtBL, "", m_defaultValue) );
        CHECK( addVariable ("phi_theta_err_wrtBL", m_phi_theta_err_wrtBL, "", m_defaultValue) );
        CHECK( addVariable ("phi_qoverp_err_wrtBL", m_phi_qoverp_err_wrtBL, "", m_defaultValue) );
        CHECK( addVariable ("theta_qoverp_err_wrtBL", m_theta_qoverp_err_wrtBL, "", m_defaultValue) );
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrackParticleParametersAtBLFillerTool::fill (const Rec::TrackParticle& track)
{

  // track parameters at ataline
  if(m_levelOfDetails > 0) {

    // call the TrackToVertex Tool
    const Trk::AtaStraightLine* ataline = 
      dynamic_cast<const Trk::AtaStraightLine*>(m_trackToVertexTool->trackAtBeamline(track));

    if(!ataline) {
#if 0
      if(track.perigee()) {
  REPORT_MESSAGE (MSG::DEBUG) << "Failed to extrapolate to beam line track of: "
            << "pt = " << track.perigee()->pT() << " - "
            << "eta = " << track.perigee()->eta() << " - "
            << "phi = " << track.perigee()->parameters()[Trk::phi0];
      }else{
  REPORT_MESSAGE (MSG::DEBUG) << "Failed to extrapolate to beam line track with no valid perigee";
      }
#endif
      return StatusCode::SUCCESS;
    }

    *m_d0 = ataline->parameters()[Trk::d0];
    *m_z0 = ataline->parameters()[Trk::z0];
    *m_phi = ataline->parameters()[Trk::phi0];
    if(m_fillThetaAndQoverP){
      *m_theta = ataline->parameters()[Trk::theta];
      *m_qoverp = ataline->parameters()[Trk::qOverP];
    }

    if(m_levelOfDetails > 1) {

      const AmgSymMatrix(5)* err = ataline->covariance();
      if(!err) return StatusCode::SUCCESS;
         
      *m_d0_err = Amg::error( *err,Trk::d0);
      *m_z0_err = Amg::error( *err,Trk::z0);
      *m_phi_err = Amg::error( *err,Trk::phi0);
      *m_theta_err = Amg::error( *err,Trk::theta);
      *m_qoverp_err = Amg::error( *err,Trk::qOverP);
  
      if(m_levelOfDetails > 2) {

        *m_d0_z0_err_wrtBL = (*err)(Trk::d0, Trk::z0);
        *m_d0_phi_err_wrtBL = (*err)(Trk::d0, Trk::phi0);
        *m_d0_theta_err_wrtBL = (*err)(Trk::d0, Trk::theta);
        *m_d0_qoverp_err_wrtBL = (*err)(Trk::d0, Trk::qOverP);
        *m_z0_phi_err_wrtBL = (*err)(Trk::z0, Trk::phi0);
        *m_z0_theta_err_wrtBL = (*err)(Trk::z0, Trk::theta);
        *m_z0_qoverp_err_wrtBL = (*err)(Trk::z0, Trk::qOverP);
        *m_phi_theta_err_wrtBL = (*err)(Trk::phi0, Trk::theta);
        *m_phi_qoverp_err_wrtBL = (*err)(Trk::phi0, Trk::qOverP);
        *m_theta_qoverp_err_wrtBL = (*err)(Trk::theta, Trk::qOverP);
      }
    }

    delete ataline;
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
