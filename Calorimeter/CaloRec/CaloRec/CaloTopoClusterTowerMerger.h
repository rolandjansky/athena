// -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CALOREC_CALOTOPOCLUSTERTOWERMERGER_H
#define CALOREC_CALOTOPOCLUSTERTOWERMERGER_H

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include <vector>
//#include <tuple>
#include <string>
#include <cmath>

class CaloTopoClusterTowerMerger : public AthAlgorithm
{
public:
  ///@brief Algorithm constructor
  CaloTopoClusterTowerMerger(const std::string& name,ISvcLocator* pSvcLocator);
  ///@brief Baseclass destructor 
  virtual ~CaloTopoClusterTowerMerger();

  ///@name Algorithm interface
  ///@{
  virtual StatusCode initialize(); ///< Initialization sets up read and write handle keys
  virtual StatusCode execute();    ///< Execution merges the container contents 
  ///@}

private:

  ///@name Internally used types
  ///@{
  typedef SG::ReadHandleKey<xAOD::CaloClusterContainer>    rhandlekey_t;   ///< Input data handle key type
  typedef SG::WriteHandleKey<xAOD::CaloClusterContainer>   whandlekey_t;   ///< Output data handle key type
  typedef SG::ReadHandle<xAOD::CaloClusterContainer>       rhandle_t;      ///< Input data handle type
  typedef SG::WriteHandle<xAOD::CaloClusterContainer>      whandle_t;      ///< Output data handle type
  ///@}

  ///@name Algorithm properties
  ///@{
  rhandlekey_t  m_clusterContainerKey;    ///< Input topo-cluster container
  rhandlekey_t  m_towerContainerKey;      ///< Input topo-tower container
  whandlekey_t  m_topoSignalContainerKey; ///< Output merged (view) container
  double        m_clusterRange;           ///< Rapidity range for topo-clusters 
  // bool          m_copyMoments;            ///< Explicitely copy cluster moments
  ///@}

  ///@name Helpers
  ///@{
  bool       makeDeepCopy(const xAOD::CaloCluster& rClus,xAOD::CaloClusterContainer* pClusCont) const; ///< Attaches a deep copy to container, returns @c true if successful. 
  //  bool       fillMoments(const xAOD::CaloCluster& rClus);                                             ///< Copies list of filled moments into lookup 
  bool       clusterFilter(const xAOD::CaloCluster& rClus) const;                                      ///< Filter topo-cluster
  bool       towerFilter(const xAOD::CaloCluster& rTowr) const;                                        ///< Filter topo-tower
  StatusCode addContainerWriteHandle(whandle_t& signalHandle);                                         ///< Add a write handle for a container (in CaloClusterStoreHelper from r21.9)
  ///@}

  ///@name Moment lookup
  ///@{
  // static std::vector<std::tuple<xAOD::CaloCluster::MomentType,std::string> > m_momentMap;  ///< Map moment types to human readable names
  // static std::vector<xAOD::CaloCluster::MomentType>                          m_momentList; ///< List of used moments for given tower collection
  ///@}
};


inline bool CaloTopoClusterTowerMerger::clusterFilter(const xAOD::CaloCluster& rClus)   const { return std::abs(rClus.eta()) <= m_clusterRange; }
inline bool CaloTopoClusterTowerMerger::towerFilter(const xAOD::CaloCluster& /*rTowr*/) const { return true; }

///@class CaloTopoClusterTowerMerger
///
/// This algorithm merges objects from two @c xAOD::CaloClusterContainer. In the context of the
/// mixed topo-tower/topo-cluster output, the objects in the topo-cluster container are taken
/// up to a client-defined (symmetric) rapidity range. The rest of the phase space is then filled 
/// with the objects from the topo-tower container. It is assumed that the overlap resolution
/// is performed when the topo-tower container is filled. The mixed object container contains deep copies 
/// of the objects in the input containers.
/// 
#endif
