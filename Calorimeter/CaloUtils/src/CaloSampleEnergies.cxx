/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloUtils/CaloSampleEnergies.h" 
#include "CaloEvent/CaloTower.h"
#include "CaloEvent/CaloCluster.h"

// constructor
CaloSampleEnergies::CaloSampleEnergies() : m_vec((int)(CaloSampling::Unknown))
{  }

// destructor 
CaloSampleEnergies::~CaloSampleEnergies() { }  


// CaloTower
const CaloSampleEnergies::E_VECTOR& 
   CaloSampleEnergies::getEnergies( const CaloTower*  obj ) 
{
   return getEnergies(obj->cell_begin(),obj->cell_end() ); 
} 

// CaloCluster
const CaloSampleEnergies::E_VECTOR& 
   CaloSampleEnergies::getEnergies( const CaloCluster* obj ) 
{
   return getEnergies(obj->cell_begin(),obj->cell_end() ); 
} 
