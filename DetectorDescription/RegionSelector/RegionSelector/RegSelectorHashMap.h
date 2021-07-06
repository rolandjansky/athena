// emacs: this is -*- c++ -*- 
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGSELECTORHASHMAP_H
#define REGSELECTORHASHMAP_H

// Includes --------------------------------------------------------------
#include "RegSelectorMapElement.h"
#include "RegionSelectorLUT.h"
#include "IRegionSelector/RegSelEnums.h"
#include "GaudiKernel/StatusCode.h"

#include "IRegionSelector/IRegSelLUT.h"

#include <vector>
#include <list>
#include <set>
#include <cstdint>

class RegionSelectorLUT;

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
class RegSelectorHashMap : virtual public IRegSelLUT {

public: 

  /// implementation of the IRegSelUT interface
  
  /// hash id methods

  virtual void HashIDList( const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const override;

  virtual void HashIDList( long layer, const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const override;

  /// rob methods
  
  virtual void ROBIDList( const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const override;
  
  virtual void ROBIDList( long layer, const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const override;  

  virtual ~RegSelectorHashMap() override = default;  
public:

  double etaminValue() const ;
  double etamaxValue() const ;
  double phiminValue() const ;
  double phimaxValue() const ;

  const std::vector<double>& etaMinOut(void) const ;
  const std::vector<double>& etaMaxOut(void) const ;
  const std::vector<double>& phiMinOut(void) const ;
  const std::vector<double>& phiMaxOut(void) const ;

  void mountDataStruct(void);

  const std::vector<IdentifierHash>&         hashIdOut(void) const ;
  const std::vector<std::vector<uint32_t> >&  robIdOut(void) const ;
  const std::vector<int>&                    sampleOut(void) const ;
  const std::vector<int>&                     layerOut(void) const ;

  void regionSelectorRobIdUint(double etaminIn, double etamaxIn, 
			       double phiminIn, double phimaxIn,
			       std::vector<uint32_t>& outRobIDList) const;

  void regionSelectorRobIdUint(int sampling, 
			       double etaminIn, double etamaxIn, 
			       double phiminIn, double phimaxIn, 
			       std::vector<uint32_t>& outRobIDList) const;
  
  void regionSelector(double etaminIn, double etamaxIn,
		      double phiminIn, double phimaxIn,
		      std::vector<IdentifierHash> & outList) const;

  void regionSelector(int sampling, 
		      double etaminIn, double etamaxIn,
		      double phiminIn, double phimaxIn,
		      std::vector<IdentifierHash> & outList) const;

  void initvar(void);
  StatusCode read(const char *filename);
  void addLut(const RegionSelectorLUT *detLut); 
  void summaryDataFile(std::list<RegSelectorMapElement> &dataList);
  void verifyInputs(double &etaminIn, double &etamaxIn,double &phiminIn, double &phimaxIn) const;

  void verifyROBIDOutput(double etaminIn, double etamaxIn,
			 double phiminIn, double phimaxIn,
			 std::vector<uint32_t>& outputIdlist) const ;

  void verifyROBIDOutput(int sampling,
                         double etaminIn, double etamaxIn,
                         double phiminIn, double phimaxIn,
                         std::vector<uint32_t>& outputIdlist) const;

  void verifyOutput(double etaminIn, double etamaxIn,
		    double phiminIn, double phimaxIn,
		    std::vector<IdentifierHash>* outputIdlist) const;

  void verifyOutput(int sampling, double etaminIn, double etamaxIn,
		    double phiminIn, double phimaxIn,
		    std::vector<IdentifierHash>* outputIdlist) const;

  void getEtaPhi(IdentifierHash hashId, 
		double *etaMin, double *etaMax,
		double *phiMin, double *phiMax) const;

  void DisableMissingROBs(const std::vector<uint32_t>& vec);


 private:

  double m_stepMinPhi{}, m_stepMinEta{};

  std::list<RegSelectorMapElement> m_dataList;
  std::vector<int> m_sample, m_layer;
  std::vector<IdentifierHash> m_hashId;
  std::vector<double> m_etamin, m_etamax;
  std::vector<double> m_phimin, m_phimax;
  std::vector<std::vector<uint32_t> > m_robId;
  double m_etaminDet{}, m_etamaxDet{};
  double m_phiminDet{}, m_phimaxDet{};
  int m_NumSamples{};
  int m_iColumns{}, m_iLines{};
  bool m_readFromFile{};
  std::vector<std::vector<uint32_t> > m_robIDMap;
  std::vector<IdentifierHash> m_hashIDMap;

  void populateMatrix(int iPage,IdentifierHash value);
  void populateMatrixRobId(int iPage, uint32_t value);
  void initMatrix(void);
  void writeLine(const int& layer, const IdentifierHash& hashId, const std::vector<uint32_t>& robId, const double& emin,
				    const double& emax, const double& pmin, const double& pmax, const int& samp);					
  int MyRound(double pdValue);
  void regionSelectorIN(const int& sampling, const double& etaminIn, 
	const double& etamaxIn, const double& phiminIn, const double& phimaxIn, 
	std::vector<IdentifierHash>* outListIH ) const;

  void regionSelectorINROB(const int& sampling, const double& etaminIn,
	const double& etamaxIn, const double& phiminIn, const double& phimaxIn,
  	std::vector<uint32_t>* outList) const;

  void phiCondition(const std::vector<IdentifierHash>& identifier, int i, 
		    double etaminIn, double etamaxIn,
		    double phiminIn, double phimaxIn,
		    double etamin, double etamax, 
		    std::set<IdentifierHash>* outset) const;

  void phiConditionRobId(const std::vector<std::vector<uint32_t> >& identifier, int i, 
			 double etaminIn, double etamaxIn,
			 double phiminIn, double phimaxIn,
			 double etamin, double etamax,
			 std::set<uint32_t>* outset) const;

  //LRSIdentHash
  void findIdentifier(std::vector<IdentifierHash> &auxsetIH,
		      const int iXBeg, const int iXEnd, 
		      const int iYBeg, const int iYEnd,
		      const int iPage) const;

  //LRSUint
  void findIdentifierROB(std::vector<uint32_t> &auxset, 
			 const int  iXBeg, const int  iXEnd, 
			 const int  iYBeg, const int  iYEnd, 
			 const int iPage) const;

};
#endif




