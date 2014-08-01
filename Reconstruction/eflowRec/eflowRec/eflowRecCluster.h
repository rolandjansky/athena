/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowRecCluster.h
 *
 *  Created on: 30.09.2013
 *      Author: tlodd
 */

#ifndef EFLOWRECCLUSTER_H_
#define EFLOWRECCLUSTER_H_

#include <vector>

#include "CxxUtils/fpcompare.h"

#include "AthLinks/ElementLink.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "eflowTrackClusterMatcher.h"

class eflowTrackClusterLink;
class eflowCaloObject;

class eflowRecCluster {
public:
  eflowRecCluster(const ElementLink<xAOD::CaloClusterContainer>& clusElementLink, eflowCaloObject* calob);
  virtual ~eflowRecCluster() { }

  const xAOD::CaloCluster* getCluster() const { return m_cluster; }
  xAOD::CaloCluster* getClusterForModification(xAOD::CaloClusterContainer* container);

  ElementLink<xAOD::CaloClusterContainer> getClusElementLink() const { return m_clusElementLink; }

  eflowCaloObject* getCaloObject() { return m_caloObject; }

  double getSumExpectedEnergy();
  double getVarianceOfSumExpectedEnergy();
  bool isEOverPFail(bool consistencySigmaCut, bool useGoldenMode);

  eflowTrackClusterMatcher& getTrackClusterMatcher() { return m_trackClusterMatcher; }

  void addTrackMatch(eflowTrackClusterLink* trackMatch) { m_trackMatches.push_back(trackMatch); }
  const std::vector<eflowTrackClusterLink*>& getTrackMatches() const { return m_trackMatches; }
  int getNTracks() const { return m_trackMatches.size(); }

  const bool& isTouchable() { return m_isTouchable;}

private:
  void replaceClusterByCopyInContainer(xAOD::CaloClusterContainer* container);

  const xAOD::CaloCluster* m_cluster;
  ElementLink<xAOD::CaloClusterContainer> m_clusElementLink;
  bool m_isTouchable;

  eflowTrackClusterMatcher m_trackClusterMatcher;
  eflowCaloObject* m_caloObject;

  std::vector<eflowTrackClusterLink*> m_trackMatches;

public:
  class SortDescendingPt {
  public:
    bool operator()(const eflowRecCluster* a, const eflowRecCluster* b) {
      return CxxUtils::fpcompare::greater(a->getCluster()->pt(),
                                          b->getCluster()->pt());
    }
  };
};

#endif /* EFLOWRECCLUSTER_H_ */
