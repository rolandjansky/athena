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
#include <memory>
#include "AthLinks/ElementLink.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

class eflowRecCluster;
class eflowRecTrack;
class eflowTrackClusterLink;
class eflowLayerIntegrator;
class eflowEEtaBinnedParameters;

class eflowCaloObject {
public:

  eflowCaloObject() { m_eflowRecClusters.clear(); m_trackClusterLinks.clear(); m_eflowRecTracks.clear(); }
  ~eflowCaloObject();
  
  void addTrack(eflowRecTrack* track) { m_eflowRecTracks.push_back(track); }
  void addCluster(eflowRecCluster* cluster) { m_eflowRecClusters.push_back(cluster); }
  void addTrackClusterLinks(std::vector<eflowTrackClusterLink*> trackClusterLink) {
    for (unsigned int i=0; i<trackClusterLink.size(); ++i) {
      addTrackClusterLink(trackClusterLink.at(i));
    }
  }
  void addTracks(std::vector<eflowRecTrack*> tracks) {
    for (unsigned int i=0; i<tracks.size(); ++i) {
      addTrack(tracks.at(i));
    }
  }
  void addClusters(std::vector<eflowRecCluster*> clusters) {
    for (unsigned int i=0; i<clusters.size(); ++i) {
      addCluster(clusters.at(i));
    }
  }

  /* Track accessor methods */
  eflowRecTrack* efRecTrack(int i) const { return m_eflowRecTracks[i]; }
  unsigned nTracks() const{ return m_eflowRecTracks.size(); }
  void clearTracks() { m_eflowRecTracks.clear(); }

  /* Cluster accessor methods */
  eflowRecCluster* efRecCluster(int i) const { return m_eflowRecClusters[i]; }
  unsigned nClusters() const{ return m_eflowRecClusters.size(); }
  void clearClusters() { m_eflowRecClusters.clear(); }


  /* Link accessor methods */
  std::vector<eflowTrackClusterLink*> efRecLink() const { return m_trackClusterLinks; }
  void clearLinks() { m_trackClusterLinks.clear(); }

  /* Sets up static container of CaloClusters. This function does not own the objects,
     but eventually Storegate takes ownership via calls to getClusterContainerPtr etc in eflowObjectBuilder.cxx  */
  static void setClusterContainerPtr(xAOD::CaloClusterContainer* clusCont, xAOD::CaloClusterAuxContainer* auxCont) {
     m_clusterContainerPtr = clusCont;
     m_clusterAuxContainerPtr = auxCont;
     m_clusterContainerPtr->setStore(m_clusterAuxContainerPtr);
  }

  /* Calculate total tracks energy, total tracks energy variance, total cluster energy for subtraction */
  double getExpectedEnergy();
  double getExpectedVariance();
  double getClusterEnergy() ;

  void simulateShower(eflowLayerIntegrator *integrator, eflowEEtaBinnedParameters* binnedParameters, bool useUpdated2015ChargedShowerSubtraction);

  static xAOD::CaloClusterContainer* getClusterContainerPtr() { return m_clusterContainerPtr;}
  static xAOD::CaloClusterAuxContainer* getClusterAuxContainerPtr() { return m_clusterAuxContainerPtr;}

private:

  void addTrackClusterLink(eflowTrackClusterLink* trackClusterLink) { m_trackClusterLinks.push_back(trackClusterLink); }

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
