/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOREC_CALOCLUSTERCOPIER_H
#define CALOREC_CALOCLUSTERCOPIER_H

/********************************************************************

NAME:     CaloClusterCopier.h 
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  D. Rousseau, copied from LArClusterCopier from Hong
CREATED:  September 1005

PURPOSE:  Copy a CaloClusterContainer, and its Clusters, into another
	  CaloClusterContainer

          Input: Input ClusterContainer 
          Output: new Cluster container 
          Parameters: 

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

********************************************************************/

//  cluster builder with simple  Algorithm (fixed cone around seed)

#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "AthenaBaseComps/AthAlgTool.h"

class CaloClusterCopier : public AthAlgTool, virtual public CaloClusterCollectionProcessor {
 public:

  // constructor 
  CaloClusterCopier(const std::string& type, const std::string& name,
		    const IInterface* parent);
  // destructor 
  ~CaloClusterCopier();
  // Algorithm virtual methods 
  virtual StatusCode initialize();
  virtual StatusCode execute(xAOD::CaloClusterContainer* );

  void getClusterSize();

 private:
  // properties 
  std::string m_clustersName;
  bool m_copyCells;
  bool m_useClusterPosition;
  float m_etCut;
  xAOD::CaloCluster::ClusterSize m_clusterSize;
  
};

#endif









