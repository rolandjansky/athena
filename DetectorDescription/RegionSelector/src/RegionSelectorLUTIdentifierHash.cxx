/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RegionSelector/RegionSelectorLUTIdentifierHash.h"

RegionSelectorLUTIdentifierHash::RegionSelectorLUTIdentifierHash( unsigned int maxHash ){
  m_etaMin.resize( maxHash );
  m_etaMax.resize( maxHash );
  m_phiMin.resize( maxHash );
  m_phiMax.resize( maxHash );
  m_position.resize( maxHash );
  m_number.resize( maxHash );
  m_robID.resize( maxHash );
  m_maxHash =  maxHash;
}

int RegionSelectorLUTIdentifierHash::layerDiskPosition( IdentifierHash hashId ){
  return m_position[ hashId ];
}

int RegionSelectorLUTIdentifierHash::layerDiskNumber( IdentifierHash hashId ){
  return m_number[ hashId ];
}

double RegionSelectorLUTIdentifierHash::etaMin( IdentifierHash hashId ){
  return m_etaMin[ hashId ];
}

double RegionSelectorLUTIdentifierHash::etaMax( IdentifierHash hashId ){
  return m_etaMax[ hashId ];
}

double RegionSelectorLUTIdentifierHash::phiMin( IdentifierHash hashId ){
  return m_phiMin[ hashId ];
}

double RegionSelectorLUTIdentifierHash::phiMax( IdentifierHash hashId ){
   return m_phiMax[ hashId ];
}

int RegionSelectorLUTIdentifierHash::sampling( IdentifierHash hashId ){
   return m_sample[ hashId ];
}

unsigned int RegionSelectorLUTIdentifierHash::maxHash( void ){
  return m_maxHash;
}

double RegionSelectorLUTIdentifierHash::etaminElem(){
  double etaminElem = 1000.0;
  unsigned int i;

  for( i = 0; i < m_maxHash; i++ ){
    if( m_etaMin[i] < etaminElem )
      etaminElem = m_etaMin[i]; 
  }
  return etaminElem;
}

double RegionSelectorLUTIdentifierHash::etamaxElem(){
  double etamaxElem = -1000.0;
  unsigned int i;

  for( i = 0; i < m_maxHash; i++ ){
    if( m_etaMax[i] > etamaxElem )
      etamaxElem = m_etaMax[i]; 
  }
  return etamaxElem;
}

double RegionSelectorLUTIdentifierHash::phiminElem(){
  double phiminElem = 1000.0;
  unsigned int i;

  for( i = 0; i < m_maxHash; i++ ){
    if( m_phiMin[i] < phiminElem )
      phiminElem = m_phiMin[i]; 
  }
  return phiminElem;
}

double RegionSelectorLUTIdentifierHash::phimaxElem(){
  double phimaxElem = -1000.0;
  unsigned int i;

  for( i = 0; i < m_maxHash; i++ ){
    if( m_phiMax[i] > phimaxElem )
      phimaxElem = m_phiMax[i]; 
  }
  return phimaxElem;
}

void RegionSelectorLUTIdentifierHash::additem(IdentifierHash hashId,
               			    const double etaMin, const double etaMax,
               			    const double phiMin, const double phiMax,
				    const int layerDiskNumber, const int layerDiskPosition ){
  m_etaMin[hashId] = etaMin;
  m_etaMax[hashId] = etaMax;
  m_phiMin[hashId] = phiMin;
  m_phiMax[hashId] = phiMax;
  m_number[hashId] = layerDiskNumber;
  m_position[hashId] = layerDiskPosition;

}

void RegionSelectorLUTIdentifierHash::additem(const double etaMin, const double etaMax, 
				    const double phiMin, const double phiMax,
				    const int layerDiskNumber, const int layerDiskPosition,
				    IdentifierHash hashId, const std::set<int> robID ){
  m_etaMin[hashId] = etaMin;
  m_etaMax[hashId] = etaMax;
  m_phiMin[hashId] = phiMin;
  m_phiMax[hashId] = phiMax;
  m_number[hashId] = layerDiskNumber;
  m_position[hashId] = layerDiskPosition;
  m_robID[hashId] = robID;
  
}

void RegionSelectorLUTIdentifierHash::additem(const int sampling,
                                    const double etaMin, const double etaMax, 
				    const double phiMin, const double phiMax,
				    const int layerDiskNumber, 
				    IdentifierHash hashId, const std::set<int> robID ){
  m_etaMin[hashId] = etaMin;
  m_etaMax[hashId] = etaMax;
  m_phiMin[hashId] = phiMin;
  m_phiMax[hashId] = phiMax;
  m_number[hashId] = layerDiskNumber;
  m_sample[hashId] = sampling;
  m_robID[hashId] = robID;
  
}

