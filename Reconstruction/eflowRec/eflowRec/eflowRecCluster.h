/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowRecCluster.h
 *
 *  Created on: 30.09.2013
 *      Author: tlodd
 */

#ifndef EFLOWRECCLUSTER_H_
#define EFLOWRECCLUSTER_H_

#include <cassert>
#include <vector>
#include <utility>

#include "CxxUtils/fpcompare.h"

#include "AthLinks/ElementLink.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "CxxUtils/CachedValue.h"
#include "eflowRec/PFMatchInterfaces.h"
class eflowTrackClusterLink;
class eflowMatchCluster;

/**
This class extends the information about a xAOD::CaloCluster. It includes an
element link and raw pointer back to the CaloCluster, the index of the cluster
in the CaloClusterContainer, a bool to determine if the CaloCluster is allowed
to be modified or not (only if we have already copied the CaloCluster), the type
(ECAL/HCAL), a map to store the LC weights for the calorimeter cluster cells, a
pointer to an eflowMatchCluster and a vector of eflowTrackClusterLink
*/
class eflowRecCluster
{
public:
  eflowRecCluster(
    const ElementLink<xAOD::CaloClusterContainer>& clusElementLink,
    xAOD::CaloClusterContainer& newClusContainer);
  eflowRecCluster(const eflowRecCluster& originalEflowRecCluster);
  eflowRecCluster& operator=(const eflowRecCluster& originalEflowRecCluster);
  virtual ~eflowRecCluster();

  xAOD::CaloCluster* getCluster() { return m_cluster; }
  const xAOD::CaloCluster* getCluster() const{ return m_cluster; }



  ElementLink<xAOD::CaloClusterContainer> getClusElementLink() const
  {
    return m_clusElementLink;
  }
  ElementLink<xAOD::CaloClusterContainer> getOriginalClusElementLink() const
  {
    return m_originalClusElementLink;
  }

  eflowMatchCluster* getMatchCluster() const { return m_matchCluster.get(); }

  double getSumExpectedEnergy();
  double getVarianceOfSumExpectedEnergy();
  bool isEOverPFail(bool consistencySigmaCut, bool useGoldenMode);

  void addTrackMatch(eflowTrackClusterLink* trackMatch)
  {
    m_trackMatches.push_back(trackMatch);
  }
  const std::vector<eflowTrackClusterLink*>& getTrackMatches() const
  {
    return m_trackMatches;
  }
  void clearTrackMatches() { m_trackMatches.clear(); }
  int getNTracks() const { return m_trackMatches.size(); }
  int getClusterId() const { return m_clusterId; }
  void setClusterId(int clusterId) { m_clusterId = clusterId; }
  void setCellsWeight(std::map<IdentifierHash, double> cellsWeight)
  {
    m_cellsWeightMap = cellsWeight;
  }
  const std::map<IdentifierHash, double>& getCellsWeight() const
  {
    return m_cellsWeightMap;
  }

  int getClusterType();
  const bool& isTouchable() { return m_isTouchable; }

private:
  /** ENUM that defines calorimeter regions as ECAL, HCAL or FCAL  */
  enum CalorimeterType
  {
    CALORIMETER_START = 0,
    UNASSIGNED = CALORIMETER_START,
    ECAL = 1,
    HCAL = 2,
    FCAL = 3,
    UNKNOWN = 4,
    CALORIMETER_END = 5
  };

  int m_clusterId;
  xAOD::CaloCluster* m_cluster;
  ElementLink<xAOD::CaloClusterContainer> m_originalClusElementLink;
  ElementLink<xAOD::CaloClusterContainer> m_clusElementLink;
  bool m_isTouchable;

  /** Specifies if we have a cluster mainly in ECAL, HCAL or FCAL  */
  CalorimeterType m_calorimeterType;

  /* for EM mode, LC weight for cells are retrieved before doing any
   * subtraction; they will be used after subtraction */
  std::map<IdentifierHash, double> m_cellsWeightMap;

  std::unique_ptr<eflowMatchCluster> m_matchCluster;
  std::vector<eflowTrackClusterLink*> m_trackMatches;

public:
  class SortDescendingPt
  {
  public:
    bool operator()(const eflowRecCluster* a, const eflowRecCluster* b)
    {
      return CxxUtils::fpcompare::greater(a->getCluster()->pt(),
                                          b->getCluster()->pt());
    }
  };
};

/**
 This class, which inherits from the pure virtual ICluster, stores a pointer to
 an eflowRecCluster. It also stores assorted kinematic information such as
 cluster energy, mean energy weighted eta/phi values etc.
*/
class eflowMatchCluster : public PFMatch::ICluster
{
public:
  eflowMatchCluster(eflowRecCluster* efRecCluster)
    : m_efRecCluster(efRecCluster)
    , m_cellEta{}
    , m_cellPhi{}
    , m_clusterEne(m_efRecCluster->getCluster()->e())
    , m_clusterEta(m_efRecCluster->getCluster()->eta())
    , m_clusterPhi(m_efRecCluster->getCluster()->phi())
    , m_clusterEtaMean(0.0)
    , m_clusterPhiMean(0.0)
    , m_clusterEtaVariance(0)
    , m_clusterPhiVariance(0)
    , m_calVariance(false)
  {
    assert(m_efRecCluster);
  }
  virtual ~eflowMatchCluster() {}

  eflowRecCluster* getEfRecCluster() { return m_efRecCluster; }
  const eflowRecCluster* getEfRecCluster() const { return m_efRecCluster; }

  virtual double e() const { return m_clusterEne; }
  virtual double eta() const { return m_clusterEta; }
  virtual double phi() const { return m_clusterPhi; }
  virtual double etaMean() const { return m_clusterEtaMean; }
  virtual double phiMean() const { return m_clusterPhiMean; }
  virtual double etaVariance() const { return m_clusterEtaVariance; }
  virtual double phiVariance() const { return m_clusterPhiVariance; }
  virtual bool calVarianceStatus() const { return m_calVariance; }
  //
  virtual void etaMean(double clusterEtaMean)
  {
    m_clusterEtaMean = clusterEtaMean;
  }
  virtual void phiMean(double clusterPhiMean)
  {
    m_clusterPhiMean = clusterPhiMean;
  }
  virtual void etaVariance(double clusterEtaVariance)
  {
    m_clusterEtaVariance = clusterEtaVariance;
  }
  virtual void phiVariance(double clusterPhiVariance)
  {
    m_clusterPhiVariance = clusterPhiVariance;
  }
  virtual void setCalVarianceStatus() { m_calVariance = true; }

  virtual unsigned int nCells() const
  {
    if (m_efRecCluster) {
      const xAOD::CaloCluster* thisCluster = std::as_const(*m_efRecCluster).getCluster();
      if (thisCluster)
        return thisCluster->size();
      // Logically if we have no valid pointer to xAOD::CaloCluster, we have no
      // link to any CaloCells
      else {
        std::cerr
          << "eflowMatchCluster ERROR: No valid link to xAOD::CaloCluster"
          << std::endl;
        return 0;
      }
    }
    // Logically if we have no valid pointer to eflowRecCluster, we have no link
    // to any CaloCells
    else {
      std::cerr << "eflowMatchCluster ERROR: No valid link to eflowRecCluster"
                << std::endl;
      return 0;
    }
  }
  virtual const std::vector<double>& cellEta() const
  {
    if (!m_cellEta.isValid()) {
      initCellPositions();
    }
    return *(m_cellEta.ptr());
  }
  virtual const std::vector<double>& cellPhi() const
  {
    if (!m_cellPhi.isValid()) {
      initCellPositions();
    }
    return *(m_cellPhi.ptr());
  }

private:
  void initCellPositions() const
  {
    std::vector<double> tmp_cellEta;
    std::vector<double> tmp_cellPhi;
    if (m_efRecCluster) {
      const xAOD::CaloCluster* thisCluster = std::as_const(*m_efRecCluster).getCluster();
      if (thisCluster) {
        const CaloClusterCellLink* theCellLink = thisCluster->getCellLinks();
        CaloClusterCellLink::const_iterator itCell = theCellLink->begin();
        CaloClusterCellLink::const_iterator endCell = theCellLink->end();

        for (; itCell != endCell; ++itCell) {
          const CaloCell* thisCell = *itCell;
          tmp_cellEta.push_back(thisCell->eta());
          tmp_cellPhi.push_back(thisCell->phi());
        }
      } else {
        std::cerr
          << "eflowMatchCluster ERROR: No valid link to xAOD::CaloCluster"
          << std::endl;
      }
    } else {
      std::cerr << "eflowMatchCluster ERROR: No valid link to eflowRecCluster"
                << std::endl;
    }
    m_cellEta.set(std::move(tmp_cellEta));
    m_cellPhi.set(std::move(tmp_cellPhi));
  }

  eflowRecCluster* m_efRecCluster;
  CxxUtils::CachedValue<std::vector<double>> m_cellEta;
  CxxUtils::CachedValue<std::vector<double>> m_cellPhi;
  double m_clusterEne;
  double m_clusterEta;
  double m_clusterPhi;
  double m_clusterEtaMean;
  double m_clusterPhiMean;
  double m_clusterEtaVariance;
  double m_clusterPhiVariance;
  bool m_calVariance;
};

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

class eflowRecClusterContainer : public DataVector<eflowRecCluster>

{

public:
  void print(){};
};

CLASS_DEF(eflowRecClusterContainer, 8904, 1)
#endif /* EFLOWRECCLUSTER_H_ */
