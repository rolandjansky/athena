/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGIONSELECTORLUTIDENTIFIERHASH_H
#define REGIONSELECTORLUTIDENTIFIERHASH_H 

// Includes -------------------------------------------------------------- 
#include <vector>
#include <set>
#include "Identifier/IdentifierHash.h"

// Class RegionSelectorLUTIdentifierHash ---------------------------------------------------- 
class RegionSelectorLUTIdentifierHash{	

 public:
  
 RegionSelectorLUTIdentifierHash(unsigned int maxHash);
 ~RegionSelectorLUTIdentifierHash(){};

 // methods to add items
 void additem(IdentifierHash hashId,
 	      const double etaMin, const double etaMax,
	      const double phiMin, const double phiMax,
	      const int layerDiskNumber, const int layerDiskPosition );
	      
  void additem(const double etaMin, const double etaMax, 
  	       const double phiMin, const double phiMax,
	       const int layerDiskNumber, const int layerDiskPosition,
	       IdentifierHash hashId, const std::set<int> robID );

  void additem(const int sampling,
               const double etaMin, const double etaMax, 
  	       const double phiMin, const double phiMax,
	       const int layerDiskNumber, 
	       IdentifierHash hashId, const std::set<int> robID );
	       
  // methods to retreive info
  double etaMin( IdentifierHash hashId );
  double etaMax( IdentifierHash hashId );
  double phiMin( IdentifierHash hashId );
  double phiMax( IdentifierHash hashId );
  int layerDiskPosition( IdentifierHash hashId );// <0 negative endcap; >0 positive endcap; ==0 barrel
  int layerDiskNumber( IdentifierHash hashId);
  double etaminElem( );
  double etamaxElem( );
  double phiminElem( );
  double phimaxElem( );
  int sampling( IdentifierHash hashId );
  unsigned int maxHash( void );

 private:
  unsigned int m_maxHash;
  std::vector<int> m_number, m_position, m_sample;
  std::vector<double> m_phiMin, m_phiMax, m_etaMin, m_etaMax;
  std::vector< std::set<int> > m_robID;

};

#endif
