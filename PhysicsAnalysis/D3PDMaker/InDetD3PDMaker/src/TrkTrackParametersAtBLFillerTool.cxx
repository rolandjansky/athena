/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackParametersAtBLFillerTool.h"

#include "AthenaKernel/errorcheck.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"

namespace D3PD {


TrkTrackParametersAtBLFillerTool::TrkTrackParametersAtBLFillerTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent)
  : D3PD::BlockFillerTool<Trk::Track> (type, name, parent),
    m_trackToVertexTool("Reco::TrackToVertex")
{
  m_levelOfDetails = 10;
  m_fillThetaAndQoverP = true;
  book().ignore(); // Avoid coverity warnings.

  declareProperty ("FillThetaAndQoverP", m_fillThetaAndQoverP=true);
  declareProperty ("DefaultValue", m_defaultValue=0,
                   "Default value to which to set unfilled variables.");
  declareProperty ("TrackToVertexTool", m_trackToVertexTool);
  declareProperty ("levelOfDetails", m_levelOfDetails);
}

StatusCode TrkTrackParametersAtBLFillerTool::initialize()
{
  CHECK( D3PD::BlockFillerTool<Trk::Track>::initialize() );
  CHECK( m_trackToVertexTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode TrkTrackParametersAtBLFillerTool::book()
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

StatusCode TrkTrackParametersAtBLFillerTool::fill (const Trk::Track& track)
{

  if(m_levelOfDetails > 0) {

    // call the TrackToVertex Tool
    const Trk::TrackParameters* ataline =  m_trackToVertexTool->trackAtBeamline(track);

    if(!ataline) {
#if 0
      if(track.perigeeParameters()) {
	REPORT_MESSAGE (MSG::DEBUG) << "Failed to extrapolate to beam line track of: "
				    << "pt = " << track.perigeeParameters()->pT() << " - "
				    << "eta = " << track.perigeeParameters()->eta() << " - "
				    << "phi = " << track.perigeeParameters()->parameters()[Trk::phi0];
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

    const AmgSymMatrix(5)* err = ataline->covariance();

    if(m_levelOfDetails > 1 && err) {

       
      *m_d0_err = sqrt((*err)(Trk::d0,Trk::d0));
      *m_z0_err = sqrt((*err)(Trk::z0,Trk::z0));
      *m_phi_err = sqrt((*err)(Trk::phi0,Trk::phi0));
      *m_theta_err = sqrt((*err)(Trk::theta,Trk::theta));
      *m_qoverp_err = sqrt((*err)(Trk::qOverP,Trk::qOverP));
  
      if(m_levelOfDetails > 2) {

        *m_d0_z0_err_wrtBL =(*err)(Trk::d0, Trk::z0);
        *m_d0_phi_err_wrtBL =(*err)(Trk::d0, Trk::phi0);
        *m_d0_theta_err_wrtBL =(*err)(Trk::d0, Trk::theta);
        *m_d0_qoverp_err_wrtBL =(*err)(Trk::d0, Trk::qOverP);
        *m_z0_phi_err_wrtBL =(*err)(Trk::z0, Trk::phi0);
        *m_z0_theta_err_wrtBL =(*err)(Trk::z0, Trk::theta);
        *m_z0_qoverp_err_wrtBL =(*err)(Trk::z0, Trk::qOverP);
        *m_phi_theta_err_wrtBL =(*err)(Trk::phi0, Trk::theta);
        *m_phi_qoverp_err_wrtBL =(*err)(Trk::phi0, Trk::qOverP);
        *m_theta_qoverp_err_wrtBL =(*err)(Trk::theta, Trk::qOverP);
      }
    }

    delete ataline;
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
