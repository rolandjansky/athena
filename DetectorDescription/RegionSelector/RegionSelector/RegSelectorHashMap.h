/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGSELECTORHASHMAP_H
#define REGSELECTORHASHMAP_H

// Includes --------------------------------------------------------------
#include "RegSelectorMapElement.h"
#include "RegionSelectorLUT.h"
//#include "ITrigRegionSelector.h"
#include "RegionSelector/RegSelEnums.h"
#include "GaudiKernel/StatusCode.h"
#include <vector>
#include <list>
#include <set>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <iterator>
#include <stdint.h>

#define INITHASH 30000 // nonexistent hashID number (init value)

struct equalsUint{
  bool operator() (uint32_t i, uint32_t j) const{
    return i==j;
  }
};

struct equals{
  bool operator() (int i, int j) const{
    return i==j;
  }
};


// Class RegSelectorHashMap ----------------------------------------------------
class RegSelectorHashMap{

 public:
  double etaminValue();
  double etamaxValue();
  double phiminValue();
  double phimaxValue();
  std::vector<double> etaMinOut(void);
  std::vector<double> etaMaxOut(void);
  std::vector<double> phiMinOut(void);
  std::vector<double> phiMaxOut(void);

  void mountDataStruct(void);
  std::vector<IdentifierHash> hashIdOut(void);
  std::vector<std::vector<uint32_t> > robIdOut(void);
  std::vector<int> sampleOut(void);
  std::vector<int> layerOut(void);
  void regionSelectorRobIdUint(double etaminIn, double etamaxIn, double phiminIn, double phimaxIn,
			       std::vector<uint32_t>& outRobIDList);
  void regionSelectorRobIdUint(int sampling, double etaminIn, double etamaxIn, double phiminIn,
  			        double phimaxIn, std::vector<uint32_t>& outRobIDList);

  void regionSelector(double etaminIn, double etamaxIn,
		double phiminIn, double phimaxIn,std::vector<IdentifierHash> & outList);
  void regionSelector(int sampling, double etaminIn, double etamaxIn,
		double phiminIn, double phimaxIn,std::vector<IdentifierHash> & outList);

  void initvar(void);
  StatusCode read(const char *filename);
  void addLut(const RegionSelectorLUT *detLut); 
  void summaryDataFile(std::list<RegSelectorMapElement> &dataList);
  void verifyInputs(double &etaminIn, double &etamaxIn,double &phiminIn, double &phimaxIn);

  void verifyROBIDOutput(double etaminIn, double etamaxIn,
		    double phiminIn, double phimaxIn,
		    std::vector<uint32_t>& outputIdlist);
  void verifyROBIDOutput(int sampling,
                         double etaminIn, double etamaxIn,
                         double phiminIn, double phimaxIn,
                         std::vector<uint32_t>& outputIdlist);

  void verifyOutput(double etaminIn, double etamaxIn,
		    double phiminIn, double phimaxIn,
		    std::vector<IdentifierHash>* outputIdlist);
  void verifyOutput(int sampling, double etaminIn, double etamaxIn,
		    double phiminIn, double phimaxIn,
		    std::vector<IdentifierHash>* outputIdlist);
  void getEtaPhi(IdentifierHash hashId, 
		double *etaMin, double *etaMax,
		double *phiMin, double *phiMax);

  void DisableMissingROBs(const std::vector<uint32_t>& vec);


 private:
  double m_stepMinPhi, m_stepMinEta;
  std::list<RegSelectorMapElement> m_dataList;
  std::vector<int> m_sample, m_layer;
  std::vector<IdentifierHash> m_hashId;
  std::vector<double> m_etamin, m_etamax;
  std::vector<double> m_phimin, m_phimax;
  std::vector<std::vector<uint32_t> > m_robId;
  double m_etaminDet, m_etamaxDet;
  double m_phiminDet, m_phimaxDet;
  int m_NumSamples;
  int m_iColumns, m_iLines;
  bool m_readFromFile;
  //__gnu_cxx::hash_std::map<uint32_t,std::vector<uint32_t>, __gnu_cxx::hash<uint32_t>, equalsUint> m_robIDMap;
  std::vector<std::vector<uint32_t> > m_robIDMap;
  //__gnu_cxx::hash_std::map<int,IdentifierHash, __gnu_cxx::hash<int>, equals> m_hashIDMap;
  std::vector<IdentifierHash> m_hashIDMap;

  void populateMatrix(int iPage,IdentifierHash value);
  void populateMatrixRobId(int iPage, uint32_t value);
  void initMatrix(void);
  void writeLine(const int& layer, const IdentifierHash& hashId, std::vector<uint32_t> robId, const double& emin,
				    const double& emax, const double& pmin, const double& pmax, const int& samp);					
  int MyRound(double pdValue);
  void regionSelectorIN(const int& sampling, const double& etaminIn, 
	const double& etamaxIn, const double& phiminIn, const double& phimaxIn, 
	std::vector<IdentifierHash>* outListIH );
  void regionSelectorINROB(const int& sampling, const double& etaminIn,
	const double& etamaxIn, const double& phiminIn, const double& phimaxIn,
  	std::vector<uint32_t>* outList);
  void phiCondition(std::vector<IdentifierHash>& identifier, int i, 
  										  double etaminIn, double etamaxIn,
                                double phiminIn, double phimaxIn,
                                double etamin, double etamax, 
										  std::set<IdentifierHash>* outset);
  void phiConditionRobId(std::vector<std::vector<uint32_t> >& identifier, int i, 
  											double etaminIn, double etamaxIn,
                                	double phiminIn, double phimaxIn,
											double etamin, double etamax,
                                	std::set<uint32_t>* outset);
  //LRSIdentHash
  void findIdentifier(std::vector<IdentifierHash> &auxsetIH,
     const int& iXBeg, const int& iXEnd, const int iYBeg, const int iYEnd,
     const int iPage);
  //LRSUint
  void findIdentifierROB(std::vector<uint32_t> &auxset, 
     const int& iXBeg, const int& iXEnd, const int iYBeg, const int
                    iYEnd, const int iPage);

};
#endif




