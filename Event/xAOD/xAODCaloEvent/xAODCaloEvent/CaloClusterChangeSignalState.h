/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs this is -*-c++-*-
#ifndef XAODCALOCLUSTERCHANGESIGNALSTATE
#define XAODCALOCLUSTERCHANGESIGNALSTATE

#include "xAODCaloEvent/CaloCluster.h"

/**
 * @brief Helper to temporarily change the signal state of a cluster.
 *
 * Constructing one of these objects will change the signal state of a cluster.
 * When this object is desctructed, the cluster's previous signal state
 * will be restored.
 */
class CaloClusterChangeSignalState
{
public:

#if __cplusplus >= 201100
  //Delete default and copy constructor 
  CaloClusterChangeSignalState() = delete;
  CaloClusterChangeSignalState(const CaloClusterChangeSignalState&)=delete;
  CaloClusterChangeSignalState& operator=(const CaloClusterChangeSignalState&) = delete;
#else
  CaloClusterChangeSignalState() : m_clus(0) {};
  void init(const xAOD::CaloCluster* pClus,
	    xAOD::CaloCluster::State s);
#endif


  /**
   * @brief Change the signal state of a cluster, remembering the old state.
   * @param pClus The cluster to change.
   * @param s The new signal state for the cluster.
   */
  CaloClusterChangeSignalState (const xAOD::CaloCluster* pClus,
                                xAOD::CaloCluster::State s);


  /**
   * @brief Destructor.  Restore the cluster to its previous state.
   */
  ~CaloClusterChangeSignalState ();

  ///Return a ptr to the cluster we are managing
  const xAOD::CaloCluster* cluster() {return m_clus;}


private:
  /// The cluster that we're managing
  const xAOD::CaloCluster* m_clus;
  /// The previous signal state for the cluster.
  xAOD::CaloCluster::State m_oldstate;
};


#if __cplusplus >= 201100
#include <forward_list>

/**
 * @brief Helper to temporarily change the signal state of many clusters
 *
 * When this object is desctructed, all clusters will be reverted to their 
 * previous signal state
 */
class CaloClusterChangeSignalStateList {

 public:
   /**
   * @brief Add one cluster to the list of clusters to be managed
   * @param clus The cluster
   * @param s The new signal state for the cluster.
   */
  void add(const xAOD::CaloCluster* clu, xAOD::CaloCluster::State s) {
    m_stateHelpers.emplace_front(clu,s);
  }

  /**
   * @brief Revert the signal state of all managed clusters.
   * Note that the implicit destructor does the same - 
   * no need to call this method
   */

  void reset() {
    m_stateHelpers.clear();
  }

 private:
  //As far as I can see this is the only stl container that doesn't require the payload to be
  //copy-constructable. 
  std::forward_list<CaloClusterChangeSignalState> m_stateHelpers;
  
};
#else //C++11

#include <list>

/**
 * @brief Helper to temporarily change the signal state of many clusters
 *
 * When this object is desctructed, all clusters will be reverted to their 
 * previous signal state
 */
class CaloClusterChangeSignalStateList {

 public:
   /**
   * @brief Add one cluster to the list of clusters to be managed
   * @param clus The cluster
   * @param s The new signal state for the cluster.
   */
  void add(const xAOD::CaloCluster* clu, xAOD::CaloCluster::State s) {
    m_stateHelpers.push_back(CaloClusterChangeSignalState());
    m_stateHelpers.back().init(clu,s);
    
  }

  /**
   * @brief Revert the signal state of all managed clusters.
   * Note that the implicit destructor does the same - 
   * no need to call this method
   */

  void reset() {
    m_stateHelpers.clear();
  }

 private:
  //As far as I can see this is the only stl container that doesn't require the payload to be
  //copy-constructable. 
  std::list<CaloClusterChangeSignalState> m_stateHelpers;
  
};


#endif //C++03


#endif
