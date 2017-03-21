/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowRecTrack.cxx
 *
 *  Created on: 30.09.2013
 *      Author: tlodd
 */

#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowDepthCalculator.h"
#include "eflowRec/eflowTrackExtrapolatorBaseAlgTool.h"

eflowRecTrack::eflowRecTrack(
    const ElementLink<xAOD::TrackParticleContainer>& trackElemLink,
    const ToolHandle<eflowTrackExtrapolatorBaseAlgTool>& theTrackExtrapolatorTool) :
    m_trackId(-1), m_trackElemLink(trackElemLink), m_track(*trackElemLink), m_type(5),
    m_pull15(0.0),
    m_eExpect(1.0), m_varEExpect(0.0),  m_isInDenseEnvironment(false), m_isSubtracted(false), m_hasBin(true),
    m_trackCaloPoints(theTrackExtrapolatorTool->execute(m_track)) {
}

eflowRecTrack::eflowRecTrack(const eflowRecTrack& eflowRecTrack){
  m_trackId = eflowRecTrack.m_trackId;
  m_trackElemLink = eflowRecTrack.m_trackElemLink;
  m_track = *m_trackElemLink;
  m_type = eflowRecTrack.m_type;
  m_pull15 = eflowRecTrack.m_pull15;
  m_eExpect = eflowRecTrack.m_eExpect;
  m_varEExpect = eflowRecTrack.m_varEExpect;
  m_isInDenseEnvironment = eflowRecTrack.m_isInDenseEnvironment;
  m_isSubtracted = eflowRecTrack.m_isSubtracted;
  m_hasBin = eflowRecTrack.m_hasBin;
  m_trackCaloPoints = new eflowTrackCaloPoints(*eflowRecTrack.m_trackCaloPoints);
}

eflowRecTrack& eflowRecTrack::operator = (const eflowRecTrack& eflowRecTrack){
  m_trackId = eflowRecTrack.m_trackId;
  m_trackElemLink = eflowRecTrack.m_trackElemLink;
  m_track = *m_trackElemLink;
  m_type = eflowRecTrack.m_type;
  m_pull15 = eflowRecTrack.m_pull15;
  m_eExpect = eflowRecTrack.m_eExpect;
  m_varEExpect = eflowRecTrack.m_varEExpect;
  m_isInDenseEnvironment = eflowRecTrack.m_isInDenseEnvironment;
  m_isSubtracted = eflowRecTrack.m_isSubtracted;
  m_hasBin = eflowRecTrack.m_hasBin;
  m_trackCaloPoints = new eflowTrackCaloPoints(*eflowRecTrack.m_trackCaloPoints);
  return *this;
}

eflowRecTrack::~eflowRecTrack() { delete m_trackCaloPoints; }

void eflowRecTrack::setCaloDepthArray(const double* depthArray) {
  if (!m_caloDepthArray.empty()){
    std::cout << "WARNING\tResetting caloDepthArray in eflowRecTrack!" << std::endl;
  }
  m_caloDepthArray.assign(depthArray, depthArray + eflowDepthCalculator::NDepth() + 1);
}

const std::vector<eflowTrackClusterLink*>* eflowRecTrack::getAlternativeClusterMatches(std::string key) const  { 

  std::map<std::string,std::vector<eflowTrackClusterLink*> >::const_iterator thisIterator = m_alternativeClusterMatches.find(key);
  if (thisIterator !=  m_alternativeClusterMatches.end()) return  &m_alternativeClusterMatches.at(key);
  else return nullptr;

  return nullptr;

}
