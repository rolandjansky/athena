/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauWideTrackPerigeeAssociationTool.cxx 577665 2014-01-09 23:18:59Z ssnyder $
/**
 * @file TauD3PDMaker/src/TauWideTrackPerigeeAssociationTool.cxx
 * @author KG <kong.guan.tan@cern.ch>
 * @date October, 2011
 * @brief Associate from a TrackParticle to its perigee at the tau's origin
 */

#include "TauWideTrackPerigeeAssociationTool.h"
#include "Particle/TrackParticle.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TauWideTrackPerigeeAssociationTool::TauWideTrackPerigeeAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_tauJet(0),
      m_taudetails(0),
      m_trackCounter(0),
      m_seedCalo_nWideTrk(0),
      m_trackToVertexTool("Reco::TrackToVertex")
{
  declareProperty ("TrackToVertexTool", m_trackToVertexTool);
}

StatusCode TauWideTrackPerigeeAssociationTool::initialize()
{
  CHECK( Base::initialize() );

  StatusCode sc = m_trackToVertexTool.retrieve();
  if(sc.isFailure()){
    REPORT_MESSAGE (MSG::ERROR) << "Could not retrieve TrackToVertexTool";
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode TauWideTrackPerigeeAssociationTool::reset(const Analysis::TauJet& p)
{
  m_tauJet = &p;
  m_taudetails = (&p)->details<const Analysis::TauCommonDetails>();
  if (!m_taudetails)
  {
    REPORT_MESSAGE (MSG::ERROR) << "TauJet has no associated CommonDetails!!";
    return 0;
  }
  m_trackCounter = 0;
  m_seedCalo_nWideTrk = m_taudetails->seedCalo_nWideTrk();

  return StatusCode::SUCCESS;
}

/**
 * @brief Return the target object.
 * @param track The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::Perigee *TauWideTrackPerigeeAssociationTool::next()
{
  // If no vertex associated to tau, then skip this association
  if (!m_tauJet->origin()) return 0;

  // reached the end
  if(m_trackCounter == m_seedCalo_nWideTrk) return 0;
  // else return a track and increment index
  const Rec::TrackParticle *track = m_taudetails->seedCalo_wideTrk(m_trackCounter);
  if(!track) return 0;
  m_trackCounter++;

  // ??? Still needed?
  if (track->measuredPerigee()->covariance() == 0)
  {
    REPORT_MESSAGE (MSG::WARNING) << "Bad track; can't find perigee at vertex.";
    return 0;
  }

  const Trk::Perigee* perigee = m_trackToVertexTool->perigeeAtVertex(*track, m_tauJet->origin()->position());

  return perigee;
}

/**
 * @brief Release an object retrieved from the association.
 * @param p The object to release.
 *
 * Call this when you are done with the object returned by
 * @c next().  The default implementation is a no-op,
 * but if the association dynamically allocated the object which
 * it returned, this gives it a chance to free it.
 */
void TauWideTrackPerigeeAssociationTool::releaseElement (const Trk::Perigee* p)
{
  if(p) delete p;
}

} // namespace D3PD
