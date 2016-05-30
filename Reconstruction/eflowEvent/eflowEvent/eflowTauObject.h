/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWTAUOBJECT_H
#define EFLOWTAUOBJECT_H
/********************************************************************

NAME:     eflowTauObject.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  Mark Hodgkinson
CREATED:  May 2013

PURPOSE:  eflow tau object data class - stores information we need to pass to each tau processint tool

********************************************************************/

#include "Particle/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "AthLinks/ElementLinkVector.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

#include <vector>

class eflowTauObject {

 public:
  eflowTauObject();
  ~eflowTauObject() {};

  void setPFOContainer(xAOD::PFOContainer* PFOContainer) {m_PFOContainer = PFOContainer; }
  void setClusterContainer(ElementLinkVector<xAOD::CaloClusterContainer>* clusterContainer) { m_clusterContainer = clusterContainer; }
  void setFinalClusterContainer(xAOD::CaloClusterContainer* clusterContainer) { m_finalClusterContainer = clusterContainer; }
  void setTrackContainer(const ElementLinkVector<xAOD::TrackParticleContainer>* trackContainer) { m_trackContainer = trackContainer; }

  const ElementLinkVector<xAOD::TrackParticleContainer>* getTrackContainer() const { return m_trackContainer;}
  ElementLinkVector<xAOD::CaloClusterContainer>* getClusterContainer() const { return m_clusterContainer; }
  xAOD::CaloClusterContainer* getFinalClusterContainer() const { return m_finalClusterContainer; }
  xAOD::PFOContainer* getPFOContainer() const { return m_PFOContainer;}

  int getNumInnerTracks() const { return m_numInnerTracks;}
  void setNumInnerTracks(int numTrack) { m_numInnerTracks = numTrack; }

 private:
  xAOD::PFOContainer* m_PFOContainer;
  const ElementLinkVector<xAOD::TrackParticleContainer>* m_trackContainer;
  ElementLinkVector<xAOD::CaloClusterContainer>* m_clusterContainer;
  xAOD::CaloClusterContainer* m_finalClusterContainer;
  int m_numInnerTracks;

};
#endif
