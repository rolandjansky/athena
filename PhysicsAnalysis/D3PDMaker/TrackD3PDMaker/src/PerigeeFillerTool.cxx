/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PerigeeFillerTool.h"

#include "AthenaKernel/errorcheck.h"
//#include "TrkParameters/TrackParameters.h"
#include "EventPrimitives/EventPrimitives.h"

namespace D3PD {


PerigeeFillerTool::PerigeeFillerTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent)
  : D3PD::BlockFillerTool<Trk::TrackParameters> (type, name, parent)
{
  // Avoid coverity warnings.
  m_fillPerigee = true;
  m_fillMomentum = true;
  m_fillThetaAndQoverP = true;
  m_fillPhi = true;
  book().ignore();

  declareProperty ("FillPerigee", m_fillPerigee=true);
  declareProperty ("FillMomentum", m_fillMomentum=false);
  declareProperty ("FillThetaAndQoverP", m_fillThetaAndQoverP=true);
  declareProperty ("FillPhi", m_fillPhi=true,
                   "Should the phi variable be filled?");
  declareProperty ("DefaultValue", m_defaultValue=0,
                   "Default value to which to set unfilled variables.");
}

StatusCode PerigeeFillerTool::book()
{
  // track parameters at perigee
  if(m_fillPerigee){
    CHECK( addVariable ("d0", m_d0, "", m_defaultValue) );
    CHECK( addVariable ("z0", m_z0, "", m_defaultValue) );
    if (m_fillPhi)
      CHECK( addVariable ("phi", m_phi, "", m_defaultValue) );
    if(m_fillThetaAndQoverP){
      CHECK( addVariable ("theta", m_theta, "", m_defaultValue) );
      CHECK( addVariable ("qoverp", m_qoverp, "", m_defaultValue) );
    }
  }

  if(m_fillMomentum){
    CHECK( addVariable ("pt", m_pt, "", m_defaultValue) );
    CHECK( addVariable ("eta", m_eta, "", m_defaultValue) );
  }

  return StatusCode::SUCCESS;
}

StatusCode PerigeeFillerTool::fill (const Trk::TrackParameters& perigee)
{
  // track parameters at perigee
  if(m_fillPerigee){
    *m_d0 = perigee.parameters()[Trk::d0];
    *m_z0 = perigee.parameters()[Trk::z0];
    if (m_fillPhi)
      *m_phi = perigee.parameters()[Trk::phi0];
    if(m_fillThetaAndQoverP){
      *m_theta = perigee.parameters()[Trk::theta];
      *m_qoverp = perigee.parameters()[Trk::qOverP];
    }
  }

  if(m_fillMomentum){
    *m_pt = perigee.pT();
    *m_eta = perigee.eta();
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
