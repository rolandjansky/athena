/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowCaloObject.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R.Tovey, M.Hodgkinson and T.Velz
CREATED:  22nd November, 2004

********************************************************************/

#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowTrackClusterLink.h"

xAOD::CaloClusterContainer* eflowCaloObject::m_clusterContainerPtr = 0;
xAOD::CaloClusterAuxContainer* eflowCaloObject::m_clusterAuxContainerPtr = 0;

eflowCaloObject::eflowCaloObject() { }

eflowCaloObject::~eflowCaloObject() {
  std::vector<eflowRecCluster*>::iterator  itCluster = m_eflowRecClusters.begin();
  std::vector<eflowRecCluster*>::iterator endCluster = m_eflowRecClusters.end();
  for (; itCluster != endCluster; ++itCluster) { delete *itCluster; }

  std::vector<eflowRecTrack*>::iterator  itTrack = m_eflowRecTracks.begin();
  std::vector<eflowRecTrack*>::iterator endTrack = m_eflowRecTracks.end();
  for (; itTrack != endTrack; ++itTrack) { delete *itTrack; }
}

eflowTrackClusterLink* eflowCaloObject::addTrack(eflowRecTrack* track, eflowRecCluster* cluster) {
  m_eflowRecTracks.push_back(track);

  if (!cluster){
    return 0;
  }
  eflowTrackClusterLink* trackClusterLink = eflowTrackClusterLink::getInstance(track, cluster);
  track->addClusterMatch(trackClusterLink);
  cluster->addTrackMatch(trackClusterLink);
  m_trackClusterLinks.push_back(trackClusterLink);
  return trackClusterLink;
}

eflowRecCluster* eflowCaloObject::addCluster(ElementLink<xAOD::CaloClusterContainer> clusterElementLink) {
  m_eflowRecClusters.push_back(new eflowRecCluster(clusterElementLink, this));
  return m_eflowRecClusters.back();
}

