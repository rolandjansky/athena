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
#include <cassert>

#include "CxxUtils/fpcompare.h"

#include "AthLinks/ElementLink.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "eflowRec/PFMatchInterfaces.h"

class eflowTrackClusterLink;
class eflowMatchCluster;

class eflowRecCluster {
public:
  eflowRecCluster(const ElementLink<xAOD::CaloClusterContainer>& clusElementLink);
  eflowRecCluster(const eflowRecCluster& originalEflowRecCluster);
  eflowRecCluster&  operator=(const eflowRecCluster& originalEflowRecCluster);
  virtual ~eflowRecCluster();

  const xAOD::CaloCluster* getCluster() const { return m_cluster; }
  xAOD::CaloCluster* getClusterForModification(xAOD::CaloClusterContainer* container);

  ElementLink<xAOD::CaloClusterContainer> getClusElementLink() const { return m_clusElementLink; }

  eflowMatchCluster* getMatchCluster() const { return m_matchCluster.get(); }

  double getSumExpectedEnergy();
  double getVarianceOfSumExpectedEnergy();
  bool isEOverPFail(bool consistencySigmaCut, bool useGoldenMode);

  void addTrackMatch(eflowTrackClusterLink* trackMatch) { m_trackMatches.push_back(trackMatch); }
  const std::vector<eflowTrackClusterLink*>& getTrackMatches() const { return m_trackMatches; }
  void clearTrackMatches() { m_trackMatches.clear(); }
  int getNTracks() const { return m_trackMatches.size(); }
  int getClusterId() const { return m_clusterId; }
  void setClusterId(int clusterId) { m_clusterId = clusterId; }
  void setCellsWeight(std::map<IdentifierHash, double> cellsWeight) { m_cellsWeightMap = cellsWeight; }
  const std::map<IdentifierHash, double>& getCellsWeight() const { return m_cellsWeightMap; }

  int getClusterType();
  const bool& isTouchable() { return m_isTouchable;}

private:
  int m_clusterId;
  const xAOD::CaloCluster* m_cluster;
  ElementLink<xAOD::CaloClusterContainer> m_clusElementLink;
  bool m_isTouchable;
  /* 1: ECAL, 2: HCAL */
  int m_type;

  /* for EM mode, LC weight for cells are retrieved before doing any subtraction; they will be used after subtraction */
  std::map<IdentifierHash,double> m_cellsWeightMap;

  std::unique_ptr<eflowMatchCluster> m_matchCluster;
  std::vector<eflowTrackClusterLink*> m_trackMatches;

  void replaceClusterByCopyInContainer(xAOD::CaloClusterContainer* container);

public:
  class SortDescendingPt {
  public:
    bool operator()(const eflowRecCluster* a, const eflowRecCluster* b) {
      return CxxUtils::fpcompare::greater(a->getCluster()->pt(),
                                          b->getCluster()->pt());
    }
  };
};

class eflowMatchCluster: public PFMatch::ICluster {
public:
 eflowMatchCluster(eflowRecCluster* efRecCluster) :  m_efRecCluster(efRecCluster), m_clusterEne(m_efRecCluster->getCluster()->e()), m_clusterEta(m_efRecCluster->getCluster()->eta()), m_clusterPhi(m_efRecCluster->getCluster()->phi()), m_clusterEtaMean(0.0), m_clusterPhiMean(0.0), m_clusterEtaVariance(0), m_clusterPhiVariance(0), m_calVariance(false) {
    assert(m_efRecCluster);
  }
  virtual ~eflowMatchCluster() {
  }

  eflowRecCluster* getEfRecCluster() const { return m_efRecCluster; }

  virtual double e() const { return m_clusterEne; }
  virtual double eta() const { return m_clusterEta; }
  virtual double phi() const { return m_clusterPhi; }
  virtual double etaMean() const { return m_clusterEtaMean; }
  virtual double phiMean() const { return m_clusterPhiMean; }
  virtual double etaVariance() const { return m_clusterEtaVariance; }
  virtual double phiVariance() const { return m_clusterPhiVariance; }
  virtual bool calVarianceStatus() const { return m_calVariance; }
  virtual void etaMean(double clusterEtaMean) const { m_clusterEtaMean = clusterEtaMean; }
  virtual void phiMean(double clusterPhiMean) const { m_clusterPhiMean = clusterPhiMean; }
  virtual void etaVariance(double clusterEtaVariance) const { m_clusterEtaVariance = clusterEtaVariance; }
  virtual void phiVariance(double clusterPhiVariance) const { m_clusterPhiVariance = clusterPhiVariance; }
  virtual void setCalVarianceStatus() const { m_calVariance = true; }

  virtual unsigned int nCells() const {
    if (m_efRecCluster){
      const xAOD::CaloCluster* thisCluster = m_efRecCluster->getCluster();
      if (thisCluster) return thisCluster->size();
      //Logically if we have no valid pointer to xAOD::CaloCluster, we have no link to any CaloCells
      else{
	std::cerr << "eflowMatchCluster ERROR: No valid link to xAOD::CaloCluster" << std::endl;
	return 0;
      }
    }
    //Logically if we have no valid pointer to eflowRecCluster, we have no link to any CaloCells
    else {
      std::cerr << "eflowMatchCluster ERROR: No valid link to eflowRecCluster" << std::endl;
      return 0;
    }
  }
  virtual const std::vector<double>& cellEta() const {
    if (m_cellEta.empty()) {
      initCellPositions();
    }
    return m_cellEta;
  }
  virtual const std::vector<double>& cellPhi() const {
    if (m_cellPhi.empty()) {
      initCellPositions();
    }
    return m_cellPhi;
  }

private:
  void initCellPositions() const {
    assert(m_cellEta.empty() && m_cellPhi.empty());
    
    if (m_efRecCluster){
      const xAOD::CaloCluster* thisCluster = m_efRecCluster->getCluster();
      if (thisCluster) {
      	const CaloClusterCellLink* theCellLink = thisCluster->getCellLinks();
	CaloClusterCellLink::const_iterator itCell = theCellLink->begin();
	CaloClusterCellLink::const_iterator endCell = theCellLink->end();
    
	for( ; itCell!=endCell; ++itCell) {
	  const CaloCell* thisCell = *itCell;
	  m_cellEta.push_back(thisCell->eta());
	  m_cellPhi.push_back(thisCell->phi());
	}
      }
      else std::cerr << "eflowMatchCluster ERROR: No valid link to xAOD::CaloCluster" << std::endl;
    }
    else std::cerr << "eflowMatchCluster ERROR: No valid link to eflowRecCluster" << std::endl;
  }

  eflowRecCluster* m_efRecCluster;
  mutable std::vector<double> m_cellEta;
  mutable std::vector<double> m_cellPhi;
  double m_clusterEne;
  double m_clusterEta;
  double m_clusterPhi;
  mutable double m_clusterEtaMean;
  mutable double m_clusterPhiMean;
  mutable double m_clusterEtaVariance;
  mutable double m_clusterPhiVariance;
  mutable bool m_calVariance;
};

#include "AthContainers/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

class eflowRecClusterContainer : public DataVector< eflowRecCluster >

{

 public:

  void print() { };

};

CLASS_DEF(eflowRecClusterContainer, 8904, 1)
#endif /* EFLOWRECCLUSTER_H_ */
