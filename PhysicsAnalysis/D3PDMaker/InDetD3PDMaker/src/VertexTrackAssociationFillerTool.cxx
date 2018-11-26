/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VertexTrackAssociationFillerTool.h"

#include "AthenaKernel/errorcheck.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkParameters/TrackParameters.h"
#include <cassert>

namespace D3PD {


VertexTrackAssociationFillerTool::VertexTrackAssociationFillerTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent)
  : Base (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode VertexTrackAssociationFillerTool::book()
{

  CHECK ( addVariable ("chi2", m_chi2) );

  CHECK ( addVariable ("d0", m_d0) );
  CHECK ( addVariable ("z0", m_z0) );
  CHECK ( addVariable ("phi", m_phi) );
  CHECK ( addVariable ("theta", m_theta) );

  return StatusCode::SUCCESS;
}

StatusCode VertexTrackAssociationFillerTool::fill (const Trk::VxTrackAtVertex& track)
{

  this->clearData();

  const Trk::FitQuality fq = track.trackQuality();
  *m_chi2 = fq.chiSquared();

  const Trk::TrackParameters *parameters = track.perigeeAtVertex();
  if(!parameters){
    REPORT_MESSAGE (MSG::WARNING) << "Perigee at vertex not filled";
  }else{
    assert (parameters->parameters().rows() >= (int)Trk::theta);
    *m_d0 = parameters->parameters()[Trk::d0];
    *m_z0 = parameters->parameters()[Trk::z0];
    *m_phi = parameters->parameters()[Trk::phi0];
    *m_theta = parameters->parameters()[Trk::theta];
  }

  return StatusCode::SUCCESS;
}

void VertexTrackAssociationFillerTool::clearData(){

  *m_chi2 = 0;

  *m_d0 = 0;
  *m_z0 = 0;
  *m_phi = 0;
  *m_theta = 0;
}

} // namespace D3PD
