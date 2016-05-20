/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERMATCHING_TOPOCLUSTERMAP_H
#define CALOCLUSTERMATCHING_TOPOCLUSTERMAP_H

//xAOD includes:
#include "xAODCaloEvent/CaloClusterContainer.h"

class TopoClusterMap {

  typedef std::map<int, std::map< int, std::vector<const xAOD::CaloCluster* > > > TopoCluster2DMap;

 public:

  //Constructor/destructor.
  //  TopoClusterMap();

  TopoClusterMap(float minEta=-6., float minPhi=-3.2, float maxEta=6., float maxPhi=3.2,
		 float dEta=0.3,   float dPhi=0.1);
  ~TopoClusterMap();

  /** @brief interface methods */

  //Function to insert a topocluster, then re-sort the vector it belongs to.
  void InsertTopoCluster(xAOD::CaloCluster*);

  inline TopoClusterMap& DuplicateMap() {
    return (*this);
  }

  //Clear the map.
  void ClearMap();

  //Function to set TopoClusters.
  StatusCode SetTopoClusters(const xAOD::CaloClusterContainer*);

  TopoCluster2DMap& RetrieveMap() {return m_map;}

  //Routine to retrieve vector of TopoClusters for a given Pt region.
  std::vector<const xAOD::CaloCluster*> RetrieveTopoClusters(double eta, double phi,
						       double Pt) const;

  //Routine to retrieve vector of TopoClusters for a given (eta, phi) region.
  std::vector<const xAOD::CaloCluster*> RetrieveTopoClusters(double eta, double phi,
						       double dEta,
						       double dPhi) const;

  //Routine to retrieve vector of TopoClusters for a given (eta, phi) region.
  inline std::vector<const xAOD::CaloCluster*> RetrieveTopoClusters(int eta_key, int phi_key) const
  {
    return m_map.at(eta_key).at(phi_key);
  }

  //Private routine to translate an input eta, phi into integer keys for map.
  inline std::pair<int,int> GetEtaPhiKeys(double eta, double phi) const {

    //Some checks on eta, phi values.
    if (eta > m_maxEta) eta = m_maxEta;
    if (eta < m_minEta) eta = m_minEta;
    if (phi > m_maxPhi) phi = m_maxPhi;
    if (phi < m_minPhi) phi = m_minPhi;

    //Need some warnings against max/min eta, phi here, I think.
    int eta_key = (int)((eta-m_minEta)/m_dEta);

    //NOTE: May have to account for phi wraparound here.
    int phi_key = (int)((phi-m_minPhi)/m_dPhi);

    return std::pair<int,int>(eta_key,phi_key);

  }

  /* void DumpMapContents(); */

 private:

  TopoCluster2DMap m_map;

  //Map granularity and range.
  double m_minEta, m_minPhi, m_maxEta, m_maxPhi;
  double m_dEta, m_dPhi;

  //Routine to sort topocluster grid elements by Pt.
  void SortGridVectors();
  void SortGridVector(int eta_key, int phi_key);

  //Used for removing clusters from map - reducing size, access time.
  double GetLArThirdLayerRatio (const xAOD::CaloCluster*);

};

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF(TopoClusterMap,179090094,1)

#endif
