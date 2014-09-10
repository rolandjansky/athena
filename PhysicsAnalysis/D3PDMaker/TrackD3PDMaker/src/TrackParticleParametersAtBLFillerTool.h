/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_TRACKPARTICLEPARAMETERSATBLFILLERTOOL_H
#define TRACKD3PDMAKER_TRACKPARTICLEPARAMETERSATBLFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "ITrackToVertex/ITrackToVertex.h"

namespace Rec{
class TrackParticle;
}

namespace D3PD {

class TrackParticleParametersAtBLFillerTool
  : public D3PD::BlockFillerTool<Rec::TrackParticle>
{
public:
  TrackParticleParametersAtBLFillerTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent);
  virtual StatusCode initialize();

  virtual StatusCode book();

  virtual StatusCode fill (const Rec::TrackParticle &p);

private:

  bool m_fillThetaAndQoverP;
  int m_levelOfDetails;

  /// Parameter: Value to which to set variables if they don't get filled.
  float m_defaultValue;

  /* NTuple variables: **/
  // track parameters at perigee
  float *m_d0;
  float *m_z0;
  float *m_phi;
  float *m_theta;
  float *m_qoverp;

  float *m_d0_err;
  float *m_z0_err;
  float *m_phi_err;
  float *m_theta_err;
  float *m_qoverp_err;
  
  float *m_d0_z0_err_wrtBL;
  float *m_d0_phi_err_wrtBL;
  float *m_d0_theta_err_wrtBL;
  float *m_d0_qoverp_err_wrtBL;
  float *m_z0_phi_err_wrtBL;
  float *m_z0_theta_err_wrtBL;
  float *m_z0_qoverp_err_wrtBL;
  float *m_phi_theta_err_wrtBL;
  float *m_phi_qoverp_err_wrtBL;
  float *m_theta_qoverp_err_wrtBL;

  /* Track to vertex extrapolator **/
  ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;

}; // class TrackParticleParametersAtBLFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRKTRACKPARAMETERSATBLFILLERTOOL_H
