/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloClusterContainer.cxx
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  H. Ma
CREATED:  May 2001

PURPOSE:  Container for CaloCluster objects. 


********************************************************************/

// INCLUDE HEADER FILES:
#include "DataModel/DataVector.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloTowerSeg.h"
#include <iostream>

CaloClusterContainer::CaloClusterContainer(SG::OwnershipPolicy ownPolicy ) : 
  DataVector<CaloCluster>(ownPolicy)
{}

CaloClusterContainer::CaloClusterContainer(const CaloTowerSeg& towerSeg)
  : DataVector<CaloCluster>(), m_towerSeg(towerSeg)
{ }

CaloClusterContainer::CaloClusterContainer(const std::string& RoiAuthor)
  : DataVector<CaloCluster>(), m_ROIauthor(RoiAuthor)
{ }

CaloClusterContainer::CaloClusterContainer(const CaloTowerSeg& towerSeg, const std::string& RoiAuthor)
  : DataVector<CaloCluster>(), m_towerSeg(towerSeg), m_ROIauthor(RoiAuthor)
{ }

CaloClusterContainer::~CaloClusterContainer()
{ }

void CaloClusterContainer::print()
{
  std::cout << " Size of CaloClusterContainer is " << size() << std::endl;
}

void CaloClusterContainer::push_back(const CaloCluster * theCluster)
{

  if (ownPolicy()==SG::OWN_ELEMENTS){
    std::cout << "ERROR Should not push_back const cluster in OWN CaloClusterContainer." << std::endl;
  }

  CaloCluster * theNonConstCluster = const_cast<CaloCluster*>(theCluster);
  
  DataVector<CaloCluster>::push_back(theNonConstCluster) ;
  
}

void CaloClusterContainer::push_back(CaloCluster * theCluster)
{

  DataVector<CaloCluster>::push_back(theCluster) ;
  
}

