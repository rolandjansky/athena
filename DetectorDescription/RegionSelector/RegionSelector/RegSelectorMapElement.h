// emacs: this is -*- c++ -*- 
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGSELECTORMAPELEMENT_H
#define REGSELECTORMAPELEMENT_H

// Includes --------------------------------------------------------------
#include <vector>
#include <set>
#include <stdint.h>
#include "Identifier/IdentifierHash.h"

#include <cmath>

class RegSelectorMapElement{
  
public:

  RegSelectorMapElement();

  int layerDiskPosition() const;
  int layerDiskNumber() const;

  double etaMin( const IdentifierHash hashId ) const;
  double etaMax( IdentifierHash hashId ) const;
  double phiMin( IdentifierHash hashId ) const;
  double phiMax( IdentifierHash hashId ) const;

  const std::vector<IdentifierHash>& hashId() const; 

  double etaminElem() const; 
  double etamaxElem() const; 
  double phiminElem() const;
  double phimaxElem() const;

  double phiminElem();
  double phimaxElem();
 
  void findMaxMinElem(void);

  void additem( const IdentifierHash hashId, 
		const double etaMin, const double etaMax, 
		const double phiMin, const double phiMax, 
		const int layerDiskNumber, const int layerDiskPosition, 
		const uint32_t robId );

  void selection( double etaminIn, double etamaxIn, 
		  double phiminIn, double phimaxIn,
		  std::set<IdentifierHash> &outset ) const; 

  void selectionRobIdUint( double etaminIn, double etamaxIn, 
			   double phiminIn, double phimaxIn, 
			   std::set<uint32_t> &outset ) const;

private:

  int m_position; // <0 negative endcap; >0 positive endcap; ==0 barrel
  int m_number;
  std::vector<double> m_etamin, m_etamax;
  std::vector<double> m_phimin, m_phimax;
  std::vector<IdentifierHash> m_hashId;
  std::vector<uint32_t> m_robId;
  double m_etaminValue, m_etamaxValue;
  double m_phiminValue, m_phimaxValue;

  void insertElementInVector( double etaminIn, double etamaxIn,
			      double phiminIn, double phimaxIn,
			      IdentifierHash hashIdIn, uint32_t robIdIn );

  void insertNumberElement(int numberIn);

  void insertPositionElement(int position);

  void findHash( double etaminIn, double etamaxIn,
		 double phiminIn, double phimaxIn,
		 std::set<IdentifierHash>& outset ) const; // no RegSelectElement eh std::set<int>

  void findRobId( double etaminIn, double etamaxIn,
		  double phiminIn, double phimaxIn,
		  std::set<uint32_t>& outset ) const; //// no RegSelectElement eh std::set<int>
};

#endif

