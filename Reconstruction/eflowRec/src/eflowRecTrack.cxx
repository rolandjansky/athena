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

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

eflowRecTrack::eflowRecTrack(
    const ElementLink<xAOD::TrackParticleContainer>& trackElemLink,
    const ToolHandle<eflowTrackExtrapolatorBaseAlgTool>& theTrackExtrapolatorTool) :
    m_trackId(-1), m_trackElemLink(trackElemLink), m_track(*trackElemLink), m_type(5),
    m_pull15(0.0),
    m_layerHED(-1),
    m_eExpect(1.0),
    m_varEExpect(0.0), 
    m_isInDenseEnvironment(false),
    m_isSubtracted(false),
    m_hasBin(true),
    m_trackCaloPoints(theTrackExtrapolatorTool->execute(m_track)) {
  m_svcLoc = Gaudi::svcLocator();
  StatusCode status = m_svcLoc->service( "MessageSvc", m_msgSvc );  
  if ( status.isSuccess( ) ) m_mlog = std::make_unique<MsgStream>(m_msgSvc,"eflowRecTrack");
  else m_mlog = nullptr;
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
  m_trackCaloPoints = std::make_unique<eflowTrackCaloPoints>(*eflowRecTrack.m_trackCaloPoints);
  m_svcLoc = Gaudi::svcLocator();
  StatusCode status = m_svcLoc->service( "MessageSvc", m_msgSvc );  
  if ( status.isSuccess( ) ) m_mlog = std::make_unique<MsgStream>(m_msgSvc,"eflowRecTrack");
  else m_mlog = nullptr;
}

eflowRecTrack& eflowRecTrack::operator = (const eflowRecTrack& originalEflowRecTrack){
  if (this == &originalEflowRecTrack) return *this;
  //if not assigning to self, then we copy the data to the new object
  else{
    m_trackId = originalEflowRecTrack.m_trackId;
    m_trackElemLink = originalEflowRecTrack.m_trackElemLink;
    m_track = *m_trackElemLink;
    m_type = originalEflowRecTrack.m_type;
    m_pull15 = originalEflowRecTrack.m_pull15;
    m_eExpect = originalEflowRecTrack.m_eExpect;
    m_varEExpect = originalEflowRecTrack.m_varEExpect;
    m_isInDenseEnvironment = originalEflowRecTrack.m_isInDenseEnvironment;
    m_isSubtracted = originalEflowRecTrack.m_isSubtracted;
    m_hasBin = originalEflowRecTrack.m_hasBin;
    m_trackCaloPoints = std::make_unique<eflowTrackCaloPoints>(*originalEflowRecTrack.m_trackCaloPoints);
    m_svcLoc = Gaudi::svcLocator();
    StatusCode status = m_svcLoc->service( "MessageSvc", m_msgSvc );  
    if ( status.isSuccess( ) ) m_mlog = std::make_unique<MsgStream>(m_msgSvc,"eflowRecTrack");
    else m_mlog = nullptr;
    return *this;
  }//if not assigning to self, then we have copied the data to the new object
}

eflowRecTrack::~eflowRecTrack() {}

void eflowRecTrack::setCaloDepthArray(const double* depthArray) {
  m_caloDepthArray.assign(depthArray, depthArray + eflowDepthCalculator::NDepth() + 1);
}

const std::vector<eflowTrackClusterLink*>* eflowRecTrack::getAlternativeClusterMatches(std::string key) const  { 

  std::map<std::string,std::vector<eflowTrackClusterLink*> >::const_iterator thisIterator = m_alternativeClusterMatches.find(key);
  if (thisIterator !=  m_alternativeClusterMatches.end()) return  &m_alternativeClusterMatches.at(key);
  else return nullptr;

  return nullptr;

}
