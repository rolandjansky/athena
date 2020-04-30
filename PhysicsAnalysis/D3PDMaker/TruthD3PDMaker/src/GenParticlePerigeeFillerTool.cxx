/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GenParticlePerigeeFillerTool.h"

#include "AthenaKernel/errorcheck.h"
#include "AtlasHepMC/GenParticle.h"
#include "TrkToolInterfaces/ITruthToTrack.h"

namespace D3PD {

GenParticlePerigeeFillerTool::GenParticlePerigeeFillerTool (const std::string& type,
					    const std::string& name,
					    const IInterface* parent)
  : Base (type, name, parent),
    m_truthToTrack("Trk::TruthToTrack/TruthToTrack")
{
  declareProperty("TruthToTrackTool", m_truthToTrack, "Tool for truth to track");

  book().ignore();  // Avoid coverity warnings.
}

StatusCode GenParticlePerigeeFillerTool::initialize(){

  CHECK( Base::initialize() );

  // Get TruthToTrack
  StatusCode sc = m_truthToTrack.retrieve();
  if (sc.isFailure()) {
    REPORT_MESSAGE (MSG::FATAL) << "Could not retrieve TruthToTrackTool";
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode GenParticlePerigeeFillerTool::book()
{
  // truth track parameters at perigee
  CHECK( addVariable ("ok", m_ok) );
  CHECK( addVariable ("d0", m_d0) );
  CHECK( addVariable ("z0", m_z0) );
  CHECK( addVariable ("phi", m_phi) );
  CHECK( addVariable ("theta", m_theta) );
  CHECK( addVariable ("qoverp", m_qoverp) );

  return StatusCode::SUCCESS;
}

StatusCode GenParticlePerigeeFillerTool::fill (const HepMC::GenParticle& particle)
{

  this->clearData();

  // truth track parameters at perigee
  const Trk::TrackParameters *perigee = m_truthToTrack->makePerigeeParameters(&particle);
  if(perigee) {
    *m_ok = 1;
    *m_d0 = perigee->parameters()[Trk::d0];
    *m_z0 = perigee->parameters()[Trk::z0];
    *m_phi = perigee->parameters()[Trk::phi0];
    *m_theta = perigee->parameters()[Trk::theta];
    *m_qoverp = perigee->parameters()[Trk::qOverP];
  } else {
    REPORT_MESSAGE (MSG::DEBUG) << "Pointer to perigee is NULL";
  }
  delete perigee;

  return StatusCode::SUCCESS;
}

void GenParticlePerigeeFillerTool::clearData(){

  *m_ok = 0;
  *m_d0 = 0;
  *m_z0 = 0;
  *m_phi = 0;
  *m_theta = 0;
  *m_qoverp = 0;
}

} // namespace MinBiasD3PD
