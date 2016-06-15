/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EGAMMATOPOCLUSTERMAP_H
#define EGAMMATOOLS_EGAMMATOPOCLUSTERMAP_H

//xAOD includes:
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "egammaInterfaces/IegammaTopoClusterMap.h"
#include <string>

//typedef std::map< int, std::vector<const xAOD::CaloCluster* > > TopoClusterMap;

typedef std::map< int, std::vector< ElementLink< xAOD::CaloClusterContainer > > > TopoClusterMap;
typedef std::map<int, TopoClusterMap > TopoCluster2DMap;

class egammaTopoClusterMap : public AthAlgTool, virtual public IegammaTopoClusterMap {

 public:

  //Constructor/destructor.
  egammaTopoClusterMap(const std::string& type,
		       const std::string& name,
		       const IInterface* parent);
  egammaTopoClusterMap();
  ~egammaTopoClusterMap();
  /** @brief AlgTool interface methods */ 
  //Tool standard routines.
  StatusCode initialize();
  StatusCode finalize();
  //Fill and sort map.
  StatusCode execute(const xAOD::CaloClusterContainer*);
  //Function to insert a topocluster, then re-sort the vector it belongs to.
  //Routine to retrieve vector of TopoClusters for a given Pt region.
  virtual std::vector<const xAOD::CaloCluster*> RetrieveTopoClusters(double eta, double phi,
								     double Pt) const ;
  //Routine to retrieve vector of TopoClusters for a given (eta, phi) region.
  virtual std::vector<const xAOD::CaloCluster*> RetrieveTopoClusters(double eta, double phi,
								     double dEta,
								     double dPhi) const ;
  //Routine to retrieve vector of TopoClusters for a given (eta, phi) region.
  virtual std::vector<const xAOD::CaloCluster*> RetrieveTopoClusters(const int eta_key, const int phi_key) const ;

 private:

  //Clear the map.
  StatusCode ClearMap() ;

  TopoCluster2DMap m_map;
  //Function to set TopoClusters.
  StatusCode SetTopoClusters(const xAOD::CaloClusterContainer*);

  //Private routine to translate an input eta, phi into integer keys for map.
  std::pair<int,int> GetEtaPhiKeys(double eta, double phi) const {
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
  //Input, output strings.
  std::string m_topoClusterMapName;
  //Map granularity and range.
  float m_minEta, m_minPhi, m_maxEta, m_maxPhi;
  float m_dEta, m_dPhi;
  float m_puThresholdCut;
  //Routine to sort topocluster grid elements by Pt.
  void SortGridVectors();
  void SortGridVector(int eta_key, int phi_key);
};
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( TopoClusterMap , 179090094 , 1 )
CLASS_DEF( TopoCluster2DMap , 60481946 , 1 )
#endif

#endif
