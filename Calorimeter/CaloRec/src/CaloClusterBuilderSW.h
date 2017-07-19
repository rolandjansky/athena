/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOREC_CALOCLUSTERBUILDERSW_H
#define CALOREC_CALOCLUSTERBUILDERSW_H

/********************************************************************

NAME:     CaloClusterBuilderSW.h 
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  H. Ma
CREATED:  May 2001

Orig:     LArCalorimeter/LArClusterRec/LArSlidingWindow

PURPOSE:  Build calorimeter cluster with Sliding Window algorithm

          Input: Calorimeter towers
          Output: Cluster in a container 
          Parameters: 
	   primary window size in eta,phi for finding maximum
	   secondary window size for calculating positions. 
	   et threshold          

Updated:  Sep 6, 2005    (DLelas)
          Modifications needed for proper filling of LAr clusters
          -Cells are put in the clusters according to a flag steered
           by a jobOption.
          -Combined clustering is unchanged
          -LAr clustering: Only seeds and zero cells are put into clusters. 
           LAr cells are put later in CaloFillRectangularCluster, a 
           derivative of CaloComputeEMlayers.

Updated:  Feb 09, 2005 (DLelas)
          New functionality added:
          -it is now possible to steer independently
           cluster-seed finding, checks for duplicate seeds 
           and cell filling

Updated: Feb 2014 (W.Lampl)
          Migrate to xAOD::CaloCluster

********************************************************************/

// INCLUDE HEADER FILES:
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class CaloTowerContainer;

class CaloClusterBuilderSW : public AthAlgTool, virtual public CaloClusterCollectionProcessor
{
 public:

  // constructor 
  CaloClusterBuilderSW(const std::string& type, const std::string& name,
		       const IInterface* parent);

  using CaloClusterCollectionProcessor::execute;
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::CaloClusterContainer* theClusters) const override;
  virtual StatusCode initialize() override;

  //virtual xAOD::CaloCluster* makeCluster(double eta, double phi); 

  struct ClusterWithCenter{ 
    std::unique_ptr<xAOD::CaloCluster> cluster; 
    int eta; 
    int phi;
    float et;
  }; 

 protected:
  int eliminate(const CaloTowerContainer& towers,
                ClusterWithCenter& cwc1, ClusterWithCenter& cwc2) const;
  
  // properties 
  int m_neta;     // Cluster sizes in eta and phi
  int m_nphi;

  int m_netap;    // window sizes in eta and phi for calculating the 
  int m_nphip;    // position. Default is the same as m_neta

  double m_ethreshold; // Energy threshold

  SG::ReadHandleKey<CaloTowerContainer> m_towerContainerKey;
  SG::ReadHandleKey<CaloCellContainer> m_cellContainerKey;
  


  bool m_FillClusterCells;  //flag to fill or not to fill cells in cluster
  int m_nextra;  // number of extra tower to be saved in the cluster.

  int m_etaSeedGrid;  // Sliding Seed_Grid sizes in eta and phi
  int m_phiSeedGrid;

  int m_etaDuplicate;  // Duplicate Grid sizes in eta and phi
  int m_phiDuplicate;

};

#endif









