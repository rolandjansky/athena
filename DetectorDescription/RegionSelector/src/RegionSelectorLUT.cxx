/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RegionSelector/RegionSelectorLUT.h"

RegionSelectorLUT::RegionSelectorLUT( unsigned int maxHash ){
  m_etaMin.resize( maxHash );
  m_etaMax.resize( maxHash );
  m_phiMin.resize( maxHash );
  m_phiMax.resize( maxHash );
  m_position.resize( maxHash );
  m_number.resize( maxHash );
  m_robId.resize( maxHash );
  m_hashId.resize( maxHash );
  m_maxHash = 0;
}

RegionSelectorLUT::RegionSelectorLUT( void ){
  unsigned int maxHash = 50000;
  m_etaMin.resize( maxHash );
  m_etaMax.resize( maxHash );
  m_phiMin.resize( maxHash );
  m_phiMax.resize( maxHash );
  m_position.resize( maxHash );
  m_number.resize( maxHash );
  m_robId.resize( maxHash );
  m_hashId.resize( maxHash );
  m_maxHash = 0;
}

int RegionSelectorLUT::layerDiskPosition( int hashId ) const{
  return m_position[ hashId ];
}

int RegionSelectorLUT::layerDiskNumber( int hashId ) const{
  return m_number[ hashId ];
}

double RegionSelectorLUT::etaMin( int hashId ) const{
  return m_etaMin[ hashId ];
}

double RegionSelectorLUT::etaMax( int hashId ) const{
  return m_etaMax[ hashId ];
}

double RegionSelectorLUT::phiMin( int hashId ) const{
  return m_phiMin[ hashId ];
}

double RegionSelectorLUT::phiMax( int hashId ) const{
   return m_phiMax[ hashId ];
}

int RegionSelectorLUT::sampling( int hashId ) const{
   return m_position[ hashId ];
}

int RegionSelectorLUT::robId( int hashId ) const{
   return m_robId[ hashId ];
}

int RegionSelectorLUT::hashId( int value ) const{
   return m_hashId[ value ];
}

unsigned int RegionSelectorLUT::maxHash( void ) const{
  return m_maxHash;
}

double RegionSelectorLUT::etaminElem(){
  double etaminElem = 1000.0;
  unsigned int i;

  if( m_maxHash > 0 ){
    for( i = 0; i < m_maxHash; i++ ){
      if( m_etaMin[i] < etaminElem )
        etaminElem = m_etaMin[i]; 
    }
  }
  return etaminElem;
}

double RegionSelectorLUT::etamaxElem(){
  double etamaxElem = -1000.0;
  unsigned int i;

  if( m_maxHash > 0 ){
    for( i = 0; i < m_maxHash; i++ ){
      if( m_etaMax[i] > etamaxElem )
        etamaxElem = m_etaMax[i]; 
    }
  }
  return etamaxElem;
}

double RegionSelectorLUT::phiminElem(){
  double phiminElem = 1000.0;
  unsigned int i;

  if( m_maxHash > 0 ){
    for( i = 0; i < m_maxHash; i++ ){
      if( m_phiMin[i] < phiminElem )
        phiminElem = m_phiMin[i]; 
    }
  }
  return phiminElem;
}

double RegionSelectorLUT::phimaxElem(){
  double phimaxElem = -1000.0;
  unsigned int i;

  if( m_maxHash > 0 ){
    for( i = 0; i < m_maxHash; i++ ){
      if( m_phiMax[i] > phimaxElem )
        phimaxElem = m_phiMax[i]; 
    }
  }

  return phimaxElem;
}

void RegionSelectorLUT::additem(const int hashId,
               			    const double etaMin, const double etaMax,
               			    const double phiMin, const double phiMax,
				    const int layerDiskNumber, const int layerDiskPosition ){
  m_etaMin[m_maxHash] = etaMin;
  m_etaMax[m_maxHash] = etaMax;
  m_phiMin[m_maxHash] = phiMin;
  m_phiMax[m_maxHash] = phiMax;
  m_number[m_maxHash] = layerDiskNumber;
  m_position[m_maxHash] = layerDiskPosition;
  m_hashId[m_maxHash] = hashId;
  m_maxHash++;

}

void RegionSelectorLUT::additem(const double etaMin, const double etaMax, 
				    const double phiMin, const double phiMax,
				    const int layerDiskNumber, const int layerDiskPosition,
				    const int hashId, const int robId ){
  m_hashId[m_maxHash] = hashId;
  m_etaMin[m_maxHash] = etaMin;
  m_etaMax[m_maxHash] = etaMax;
  m_phiMin[m_maxHash] = phiMin;
  m_phiMax[m_maxHash] = phiMax;
  m_number[m_maxHash] = layerDiskNumber;
  m_position[m_maxHash] = layerDiskPosition;
  m_robId[m_maxHash] = robId;
  
  m_maxHash++;
  
}

void RegionSelectorLUT::additem(const int sampling, 
                                    const double etaMin, const double etaMax, 
				    const double phiMin, const double phiMax,
				    const int layerDiskNumber,
				    const int hashId, const int robId ){
  m_hashId[m_maxHash] = hashId;
  m_etaMin[m_maxHash] = etaMin;
  m_etaMax[m_maxHash] = etaMax;
  m_phiMin[m_maxHash] = phiMin;
  m_phiMax[m_maxHash] = phiMax;
  m_number[m_maxHash] = layerDiskNumber;
  m_position[m_maxHash] = sampling;
  m_robId[m_maxHash] = robId;
  m_maxHash++;
}
