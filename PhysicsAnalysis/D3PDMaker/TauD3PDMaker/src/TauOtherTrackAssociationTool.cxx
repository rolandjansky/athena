/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauOtherTrackAssociationTool.h"

#include <Particle/TrackParticle.h>
#include "AthenaKernel/errorcheck.h"

namespace D3PD {


TauOtherTrackAssociationTool::TauOtherTrackAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
      : Base (type, name, parent),
        m_taudetails(0),
	m_trackCounter(0),
	m_nOtherTrk(0)
{}

/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode TauOtherTrackAssociationTool::reset (const Analysis::TauJet& p)
{
  m_trackCounter = 0;

  m_taudetails = 0;
  m_taudetails = (&p)->details<const Analysis::TauCommonDetails>();

  if (m_taudetails==0)
  {
    REPORT_MESSAGE (MSG::ERROR) << "TauJet has no associated CommonDetails!!";
    return 0;
  }

  m_nOtherTrk = m_taudetails->nOtherTrk();

  return StatusCode::SUCCESS;
}

/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Rec::TrackParticle* TauOtherTrackAssociationTool::next()
{

  // reached the end
  if(m_trackCounter == m_nOtherTrk) return 0;
  // else return a track and increment index
  
  const Rec::TrackParticle *track = m_taudetails->otherTrk(m_trackCounter);
  if(!track)
      return 0;
  m_trackCounter++;
  return track;
}


} // namespace D3PD
