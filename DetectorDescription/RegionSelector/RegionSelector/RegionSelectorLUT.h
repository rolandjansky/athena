/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGIONSELECTORLUT_H
#define REGIONSELECTORLUT_H 

// Includes -------------------------------------------------------------- 
#include <vector>
#include <set>
#include <stdint.h>
// using namespace std;
// Class RegionSelectorLUT ---------------------------------------------------- 
class RegionSelectorLUT{	

 public:
  
 RegionSelectorLUT(unsigned int maxHash);
 RegionSelectorLUT( void );
 ~RegionSelectorLUT(){};

 // methods to add items
 void additem(const int hashId,
 	      const double etaMin, const double etaMax,
	      const double phiMin, const double phiMax,
	      const int layerDiskNumber, const int layerDiskPosition );
	      
  void additem(const double etaMin, const double etaMax, 
  	       const double phiMin, const double phiMax,
	       const int layerDiskNumber, const int layerDiskPosition,
	       const int hashId, const int robId );

  void additem(const int sampling,
               const double etaMin, const double etaMax, 
  	       const double phiMin, const double phiMax,
	       const int layerDiskNumber,
	       const int hashId, const int robId );
	       
  // methods to retreive info
  double etaMin( int hashId ) const;
  double etaMax( int hashId ) const;
  double phiMin( int hashId ) const;
  double phiMax( int hashId ) const;
  int sampling( int hashId ) const;
  int robId( int hashId ) const;
  int hashId( int value ) const;
  int layerDiskPosition( int hashId ) const;// <0 negative endcap; >0 positive endcap; ==0 barrel
  int layerDiskNumber( int hashId ) const;
  unsigned int maxHash( void ) const; 
  double etaminElem( );
  double etamaxElem( );
  double phiminElem( );
  double phimaxElem( );
  
 private:
  unsigned int m_maxHash;
  std::vector<int> m_number, m_position;
  std::vector<double> m_phiMin, m_phiMax, m_etaMin, m_etaMax;
  std::vector<int> m_robId, m_hashId;

};

#endif
