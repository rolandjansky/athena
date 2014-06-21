/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauSeedCaloTrackAssociationTool.h"

#include <Particle/TrackParticle.h>
#include "AthenaKernel/errorcheck.h"

namespace D3PD {


TauSeedCaloTrackAssociationTool::TauSeedCaloTrackAssociationTool
(const std::string& type,
 const std::string& name,
 const IInterface* parent)
    : Base (type, name, parent),
      m_tauJet(0),
      m_trackCounter(0),
      m_seedCalo_numTrack(0)
{}

/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode TauSeedCaloTrackAssociationTool::reset (const Analysis::TauJet& p)
{
  m_tauJet = &p;
  m_trackCounter = 0;
  m_seedCalo_numTrack = m_tauJet->seedCalo_numTrack();

  return StatusCode::SUCCESS;
}

/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Rec::TrackParticle* TauSeedCaloTrackAssociationTool::next()
{
  // reached the end
  if(m_trackCounter == m_seedCalo_numTrack) return 0;
  // else return a track and increment index
  const Rec::TrackParticle *track = m_tauJet->seedCalo_track(m_trackCounter);
  if(!track)
      return 0;
  m_trackCounter++;
  return track;
}


} // namespace D3PD
