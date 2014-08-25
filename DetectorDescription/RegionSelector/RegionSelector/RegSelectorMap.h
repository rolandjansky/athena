/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGSELECTORMAP_H
#define REGSELECTORMAP_H

// Includes --------------------------------------------------------------
#include "RegSelectorMapElement.h"
#include "RegionSelectorLUT.h"
//#include "ITrigRegionSelector.h"
#include "RegionSelector/RegSelEnums.h"
#include "GaudiKernel/StatusCode.h"
#include <vector>
#include <list>
#include <set>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <cstdio>

enum MDET{
  MDET_MDT = 60,
  MDET_RPC,
  MDET_TGC,
  MDET_CSC
};

// Class RegSelectorMap ----------------------------------------------------
class RegSelectorMap{

 public:
  double etaminValue(void);
  double etamaxValue(void);
  double phiminValue(void);
  double phimaxValue(void);
  std::vector<IdentifierHash> hashIdOut(void);
  std::vector<uint32_t> robIdOut(void);
  std::vector<int> barORendOut(void);
  std::vector<int> layORdskOut(void);
  std::vector<double> etaMinOut(void);
  std::vector<double> etaMaxOut(void);
  std::vector<double> phiMinOut(void);
  std::vector<double> phiMaxOut(void);
  void mountDataStruct(void);
  void regionSelectorRobIdUint(DETID TYPE, double& etaminIn, double& etamaxIn,
					    double& phiminIn, double& phimaxIn,std::vector<uint32_t>& outList);
  void regionSelectorRobIdUint(double& etaminIn, double& etamaxIn, double& phiminIn,
  			       double& phimaxIn, std::vector<uint32_t>& outList);
  void regionSelectorRobIdUint(long layNumber, double& etaminIn, double& etamaxIn, 
  					double& phiminIn, double& phimaxIn, std::vector<uint32_t>& outList);
  void addLut(const RegionSelectorLUT &detLut); 
  void regionSelector(DETID TYPE,double& etaminIn, double& etamaxIn, double& phiminIn,
  			       double& phimaxIn, std::vector<IdentifierHash>& outList);
  void regionSelector(TYPEID typeinID, double& etaminIn, double& etamaxIn, double& phiminIn,
                      double& phimaxIn, std::vector<IdentifierHash>& outList);
  void regionSelector(long layNumber, double& etaminIn, double& etamaxIn, double& phiminIn,
                      double& phimaxIn, std::vector<IdentifierHash>& outList);
  StatusCode read(const char *filename, DETID type);
  void summaryDataFile(std::list<RegSelectorMapElement> &dataList);
  StatusCode verifyInputs(double &etaminIn, double &etamaxIn, double &phiminIn, double &phimaxIn);
  StatusCode verifyInputsMinusPi(double &etaminIn, double &etamaxIn, double &phiminIn, double &phimaxIn);
  void verifyOutput(double etaminIn, double etamaxIn, double phiminIn, double phimaxIn,
  			   std::vector<IdentifierHash> outputIdlist);
  void verifyOutputRobId(double etaminIn, double etamaxIn, double phiminIn, double phimaxIn,
  			   std::vector<uint32_t> outputIdlist);
  void getEtaPhi(IdentifierHash hashId, 
		double *etaMin, double *etaMax,
		double *phiMin, double *phiMax);

 private:
  std::list<RegSelectorMapElement> m_barreldataList, m_posdataList, m_negdataList;
  std::vector<int> m_barORend, m_layORdsk;
  std::vector<IdentifierHash> m_hashId;
  std::vector<uint32_t> m_robId;
  std::vector<double> m_etamin, m_etamax;
  std::vector<double> m_phimin, m_phimax;
  double m_etaminDet, m_etamaxDet;
  double m_phiminDet, m_phimaxDet;
  void findPosition(TYPEID& typeinID, bool& posORneg, std::list<RegSelectorMapElement>::iterator& it);
  void findPosition(long& layNumber, bool& posORneg, std::list<RegSelectorMapElement>::iterator& it);
	void findMaxMinValues(std::list<RegSelectorMapElement> &dataList);
  RegSelectorMapElement creatingElement(int& positionIn,int& numberIn, double& etaminIn, double& etamaxIn,
                       		        double& phiminIn, double& phimaxIn, IdentifierHash& hashIdIn, uint32_t& robId);
  void insertList(std::list<RegSelectorMapElement> &dataList, int positionIn, int numberIn,
		  double etaminIn, double etamaxIn, double phiminIn, double phimaxIn,
		  IdentifierHash hashIdIn, uint32_t& robIdIn);
  void insertDataElement(int& positionIn, int& numberIn, double& etaminIn, double& etamaxIn,
				      double& phiminIn, double& phimaxIn, IdentifierHash& hashIdIn, uint32_t& robIdIn);
  void writeLine(int barORend, int layORdsk, IdentifierHash hashId, uint32_t robId, double emin, double emax,
		 double pmin, double pmax);
  StatusCode readTILE(const char *filename);
  StatusCode readMuon(MDET mtype, const char *filename);
  void readMDTData(std::ifstream &fin);
  void readRPCData(std::ifstream &fin);

};

#endif




