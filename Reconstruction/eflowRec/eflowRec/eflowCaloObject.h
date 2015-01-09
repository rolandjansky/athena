/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWEVENT_EFLOWCALOOBJECT_H
#define EFLOWEVENT_EFLOWCALOOBJECT_H
/********************************************************************

NAME:     eflowCaloObject.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D. Tovey
CREATED:  18th November, 2001

PURPOSE:  Calorimeter Object data class

********************************************************************/

#include <vector>
#include "AthLinks/ElementLink.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

class eflowRecCluster;
class eflowRecTrack;
class eflowTrackClusterLink;

class eflowCaloObject {
public:

  eflowCaloObject();
  ~eflowCaloObject();
  
  /* Track accessor methods */

  eflowRecTrack* efRecTrack(int i) { return m_eflowRecTracks[i]; }
  int nTracks() { return m_eflowRecTracks.size(); }

  void addTrack(eflowRecTrack* track, eflowTrackClusterLink* trackClusterLink = 0);

  /* Cluster accessor methods */

  eflowRecCluster* efRecCluster(int i) { return m_eflowRecClusters[i]; }
  int nClusters() { return m_eflowRecClusters.size(); }

  void addCluster(eflowRecCluster* cluster);

  /* Static container accessors */
  static void setClusterContainerPtr(xAOD::CaloClusterContainer* clusCont, xAOD::CaloClusterAuxContainer* auxCont) {
    m_clusterContainerPtr = clusCont;
    m_clusterAuxContainerPtr = auxCont;
    m_clusterContainerPtr->setStore(m_clusterAuxContainerPtr);
  }

  static xAOD::CaloClusterContainer* getClusterContainerPtr() { return m_clusterContainerPtr;}
  static xAOD::CaloClusterAuxContainer* getClusterAuxContainerPtr() { return m_clusterAuxContainerPtr;}

 private:
  /* Vector of clusters */
  std::vector<eflowRecCluster*> m_eflowRecClusters;

  /* Vector of track-cluster matches */
  std::vector<eflowTrackClusterLink*> m_trackClusterLinks;

  /* Vector of tracks */
  std::vector<eflowRecTrack*> m_eflowRecTracks;

  /* Containers of CaloClusters */
  static xAOD::CaloClusterContainer* m_clusterContainerPtr;
  static xAOD::CaloClusterAuxContainer* m_clusterAuxContainerPtr;
};

#include "AthContainers/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

class eflowCaloObjectContainer : public DataVector< eflowCaloObject >

{

 public:

  void print() { };

};

CLASS_DEF(eflowCaloObjectContainer, 9802, 1)

#endif
