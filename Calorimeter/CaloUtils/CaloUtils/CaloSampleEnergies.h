/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOSAMPLEENERGIRES_H
#define CALOUTILS_CALOSAMPLEENERGIRES_H

/*   Purpose:	Given a CaloTower, CaloCluster, or iterator pair of
	        CaloCells, return a vector of energy in each sampling 
*/

#include "CaloEvent/CaloCell.h" 
#include "CaloGeoHelpers/CaloSampling.h" 

class CaloTower;
class CaloCluster; 

#include <vector>
#include <math.h>

class CaloSampleEnergies
{

 public:

  typedef  std::vector<double>                  E_VECTOR ; 
  typedef  E_VECTOR::const_iterator             E_ITERATOR;

  CaloSampleEnergies( );
  ~CaloSampleEnergies();

  /** Return energy in each Sampling for CaloTower
  */ 
  const E_VECTOR& getEnergies(const CaloTower*) ; 
  /** Return energy in each Sampling for CaloCluster
  */ 
  const E_VECTOR& getEnergies(const CaloCluster*) ; 

  /** Return energy in each Sampling for a pair of interators
  */ 
  template <class ITER > 
  const E_VECTOR& getEnergies( ITER t1, ITER t2); 

 private: 
  E_VECTOR m_vec ; 

};

template <class ITER> 
const CaloSampleEnergies::E_VECTOR& CaloSampleEnergies::getEnergies(ITER it1, 
							ITER it2)
{

 int nEntry = CaloSampling::Unknown ; 

 for(int i=0; i<nEntry; ++i) m_vec[i]=0; 

 for(; it1!=it2;++it1) 
 {
  const CaloCell* cell   = *it1; 
  int sam = cell->caloDDE()->getSampling();
  m_vec[sam] += cell->energy(); 
 }

 return m_vec;

} 

#endif







