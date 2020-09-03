/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloClusterContainer.cxx
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  H. Ma
CREATED:  May 2001

PURPOSE:  Container for CaloCluster objects. 


********************************************************************/

// INCLUDE HEADER FILES:
#include "AthContainers/DataVector.h"
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

