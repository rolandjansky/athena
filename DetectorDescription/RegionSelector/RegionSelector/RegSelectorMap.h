// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGSELECTORMAP_H
#define REGSELECTORMAP_H


#include "RegSelectorMapElement.h"
#include "RegionSelectorLUT.h"

#include "IRegionSelector/IRegSelLUT.h"
#include "IRegionSelector/RegSelEnums.h"

#include "GaudiKernel/StatusCode.h"
#include <vector>
#include <list>
#include <set>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <cstdio>



class RegSelectorMap : virtual public IRegSelLUT {

public:


  /// implementation of the IRegSelUT interface - intentionally inlined
  
  /// hash id methods

  virtual void HashIDList( const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const override;
  
  virtual void HashIDList( long layer, const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const override;

  /// Rob identifier methods
  
  virtual void ROBIDList( const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const override;
  
  virtual void ROBIDList( long layer, const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const override;
  
  virtual ~RegSelectorMap() override = default;

public:

  double etaminValue(void) const;
  double etamaxValue(void) const;
  double phiminValue(void) const;
  double phimaxValue(void) const;

  const std::vector<IdentifierHash>& hashIdOut(void) const;
  const std::vector<uint32_t>&        robIdOut(void) const;

  const std::vector<int>&   barORendOut(void) const;
  const std::vector<int>&   layORdskOut(void) const;
  const std::vector<double>&  etaMinOut(void) const;
  const std::vector<double>&  etaMaxOut(void) const;
  const std::vector<double>&  phiMinOut(void) const;
  const std::vector<double>&  phiMaxOut(void) const;

  void mountDataStruct(void);

  void regionSelectorRobIdUint( DETID TYPE, 
			        double etaminIn, double etamaxIn,
			        double phiminIn, double phimaxIn,
			        std::vector<uint32_t>& outList ) const;

  void regionSelectorRobIdUint( double etaminIn, double etamaxIn, 
				double phiminIn, double phimaxIn, 
			        std::vector<uint32_t>& outList ) const;

  void regionSelectorRobIdUint( long layNumber, 
			        double etaminIn, double etamaxIn, 
			        double phiminIn, double phimaxIn, 
			        std::vector<uint32_t>& outList ) const;

  void addLut( const RegionSelectorLUT* detLut ); 
  void addLut( const RegionSelectorLUT& detLut ) { addLut(&detLut); } 

  void regionSelector( DETID TYPE,
		       double etaminIn, double etamaxIn, 
		       double phiminIn, double phimaxIn, 
		       std::vector<IdentifierHash>& outList) const;

  void regionSelector( TYPEID typeinID, 
		       double etaminIn, double etamaxIn, 
                       double phiminIn, double phimaxIn, 
		       std::vector<IdentifierHash>& outList ) const;

  void regionSelector( long layNumber, 
		       double etaminIn, double etamaxIn, 
                       double phiminIn, double phimaxIn, 
		       std::vector<IdentifierHash>& outList ) const;

  StatusCode read(const char *filename, DETID type);

  void summaryDataFile( std::list<RegSelectorMapElement> &dataList );

  void verifyInputsInternal( double &etaminIn, double &etamaxIn, 
			     double &phiminIn, double &phimaxIn ) const;

  StatusCode verifyInputs( double &etaminIn, double &etamaxIn, 
			   double &phiminIn, double &phimaxIn ) const;

  StatusCode verifyInputsMinusPi( double &etaminIn, double &etamaxIn, 
				  double &phiminIn, double &phimaxIn ) const;

  void verifyOutput( double etaminIn, double etamaxIn, 
		     double phiminIn, double phimaxIn,
		     std::vector<IdentifierHash> outputIdlist ) const;

  void verifyOutputRobId( double etaminIn, double etamaxIn, 
			  double phiminIn, double phimaxIn,
			  std::vector<uint32_t> outputIdlist ) const;

  void getEtaPhi( IdentifierHash hashId, 
		  double *etaMin, double *etaMax,
		  double *phiMin, double *phiMax ) const;

private:
  
  void findPosition( TYPEID typeinID, bool posORneg, std::list<RegSelectorMapElement>::const_iterator& it ) const;

  void findPosition( long layNumber, bool posORneg, std::list<RegSelectorMapElement>::const_iterator& it ) const;

  void findMaxMinValues( std::list<RegSelectorMapElement>& dataList );

  RegSelectorMapElement creatingElement( int& positionIn,  int& numberIn, 
					 double& etaminIn, double& etamaxIn,
					 double& phiminIn, double& phimaxIn, IdentifierHash& hashIdIn, uint32_t& robId);
  
  void insertList( std::list<RegSelectorMapElement> &dataList, 
		   int positionIn, int numberIn,
		   double etaminIn, double etamaxIn, 
		   double phiminIn, double phimaxIn,
		   IdentifierHash hashIdIn, 
		   uint32_t& robIdIn);

  void insertDataElement(int& positionIn, int& numberIn, double& etaminIn, double& etamaxIn,
			 double& phiminIn, double& phimaxIn, IdentifierHash& hashIdIn, uint32_t& robIdIn);

  void writeLine(int barORend, int layORdsk, IdentifierHash hashId, uint32_t robId, double emin, double emax,
		 double pmin, double pmax);

  StatusCode readTILE(const char *filename);

private:

  std::list<RegSelectorMapElement> m_barreldataList, m_posdataList, m_negdataList;
 
  std::vector<int> m_barORend, m_layORdsk;
  
  std::vector<IdentifierHash> m_hashId;
  
  std::vector<uint32_t> m_robId;
  
  std::vector<double> m_etamin, m_etamax;
  
  std::vector<double> m_phimin, m_phimax;
  
  double m_etaminDet, m_etamaxDet;
  
  double m_phiminDet, m_phimaxDet;


};

#endif




