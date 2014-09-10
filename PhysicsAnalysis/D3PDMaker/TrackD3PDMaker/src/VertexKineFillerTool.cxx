/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/VertexKineFillerTool.cxx
 * @author maarten boonekamp <maarten.boonekamp@cea.fr>
 * @date Sep, 2009
 * @brief Block filler tool for vertex kinematics, from attached tracks.
 */


#include "VertexKineFillerTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
#include "AthenaKernel/errorcheck.h"
#include "TrkParameters/TrackParameters.h"


namespace {
inline
double sq(double x) { return x*x; }
}



namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
VertexKineFillerTool::VertexKineFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : Base (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode VertexKineFillerTool::book()
{
  CHECK( addVariable ("px",      m_px)  );
  CHECK( addVariable ("py",      m_py)  );
  CHECK( addVariable ("pz",      m_pz)  );
  CHECK( addVariable ("E",       m_E)  );
  CHECK( addVariable ("m",       m_m)  );
  CHECK( addVariable ("nTracks", m_nTracks)  );
  CHECK( addVariable ("sumPt",   m_sumPt)  );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode VertexKineFillerTool::fill (const Trk::VxCandidate& p)
{
  for (const Trk::VxTrackAtVertex* tv : *p.vxTrackAtVertex()) {

    const Trk::Perigee* perigee = 0;
    if ( tv->perigeeAtVertex() )
      perigee = (const Trk::Perigee*) tv->perigeeAtVertex();
    else
      perigee = (const Trk::Perigee*) tv->initialPerigee();

    *m_px    += perigee->momentum().x();
    *m_py    += perigee->momentum().y();
    *m_pz    += perigee->momentum().z();
    *m_E     += perigee->momentum().mag();
    *m_sumPt += hypot(perigee->momentum().x(),perigee->momentum().y());
  }
  
  float vtxmass = sq(*m_E) - sq(*m_px) - sq(*m_py) - sq(*m_pz);
  if (vtxmass>0.0)
    vtxmass = std::sqrt(vtxmass);
  else
    vtxmass = 0.0;

  *m_m       = vtxmass;
  *m_nTracks = p.vxTrackAtVertex()->size();

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode VertexKineFillerTool::fill (const xAOD::Vertex& p)
{
  for (const auto& tplink : p.trackParticleLinks()) {

    const Trk::Perigee& perigee = (*tplink)->perigeeParameters();

    *m_px    += perigee.momentum().x();
    *m_py    += perigee.momentum().y();
    *m_pz    += perigee.momentum().z();
    *m_E     += perigee.momentum().mag();
    *m_sumPt += hypot(perigee.momentum().x(),perigee.momentum().y());
  }
  
  float vtxmass = sq(*m_E) - sq(*m_px) - sq(*m_py) - sq(*m_pz);
  if (vtxmass>0.0)
    vtxmass = std::sqrt(vtxmass);
  else
    vtxmass = 0.0;

  *m_m       = vtxmass;
  *m_nTracks = p.nTrackParticles();

  return StatusCode::SUCCESS;
}


} // namespace D3PD
