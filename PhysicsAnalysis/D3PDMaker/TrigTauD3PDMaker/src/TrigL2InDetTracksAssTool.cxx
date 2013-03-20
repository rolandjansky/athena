/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2InDetTracksAssTool.h"

#include "AthenaKernel/errorcheck.h"

namespace D3PD {

  TrigL2InDetTracksAssTool::TrigL2InDetTracksAssTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_matchTool ("TrigTauTool::TrigReadNavTool/TrigReadNavTool"),
      m_trackCounter(0),
      m_numTracks(0)
  {
    this->declareProperty ("MatchTool", m_matchTool,
			   "Trigger object matching tool.");
    this->declareProperty ("ChainPattern", m_chainPattern = "L2.*tau.*, L2.*trk.*",
    			   "Pattern to use for matching chains.");
  }
  /**
   * @brief Standard Gaudi initialize method.
   */
  StatusCode TrigL2InDetTracksAssTool::initialize()
  {
    CHECK( m_matchTool.retrieve() );
    return StatusCode::SUCCESS;
  }
  
  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  StatusCode TrigL2InDetTracksAssTool::reset (const TrigTau& p)
  { 
    m_tracks.clear();
    std::vector<const TrigInDetTrackCollection*> trackcollections = m_matchTool->getRelations<TrigInDetTrackCollection, TrigTau>(m_chainPattern, &p);
    for(size_t k=0; k<trackcollections.size(); k++){
      const TrigInDetTrackCollection* trackcollection = (trackcollections[k]);
      for(size_t j=0; j<trackcollection->size(); j++){
	const TrigInDetTrack* tautrks = (*trackcollection)[j];
	if(tautrks!=0) m_tracks.push_back(tautrks);
      }
    }
    m_trackCounter=0;
    m_numTracks=m_tracks.size();
 
    return StatusCode::SUCCESS;
  }

  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  const TrigInDetTrack* TrigL2InDetTracksAssTool::next()
  {
    // reached the end
    if(m_trackCounter == m_numTracks) return 0;
    // else return a track and increment index
    const TrigInDetTrack *track = m_tracks[m_trackCounter];
    m_trackCounter++;
    return track;
  }

} // namespace D3PD
