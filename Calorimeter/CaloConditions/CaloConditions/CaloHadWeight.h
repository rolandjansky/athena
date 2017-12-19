// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CALOCONDITIONS_CALOHADWEIGHT_H
#define CALOCONDITIONS_CALOHADWEIGHT_H


#include "AthenaKernel/CLASS_DEF.h" 
#include <vector>

/**
 * @class CaloHadWeight
 *
 */
class CaloHadWeight {

public:

  struct HadData {
    float m_weight;
    float m_entries;
    float m_error;
    HadData() : m_weight(0),m_entries(0),m_error(0) {};
  };

  //** Default Constructor
  CaloHadWeight();

  CaloHadWeight(const int   & nBinEta, 
		const float & MinEta, 
		const float & MaxEta, 
		const int   & nBinSampling, 
		const int   & MinSampling,
		const int   & MaxSampling,
		const int   & nBinLogEClus,
		const float & MinLogEClus,
		const float & MaxLogEClus,
		const int   & nBinLogEDensCell,
		const float & MinLogEDensCell,
		const float & MaxLogEDensCell
		);

  // access methods

  inline const int & getNbinsEta() const {return m_nBinEta;}
  inline const float & getEtaMin() const {return m_MinEta;}
  inline const float & getEtaMax() const {return m_MaxEta;}

  inline const int & getNbinsSampling() const {return m_nBinSampling;}
  inline const int & getSamplingMin() const {return m_MinSampling;}
  inline const int & getSamplingMax() const {return m_MaxSampling;}

  inline const int & getNbinsLogEClus() const {return m_nBinLogEClus;}
  inline const float & getLogEClusMin() const {return m_MinLogEClus;}
  inline const float & getLogEClusMax() const {return m_MaxLogEClus;}

  inline const int & getNbinsLogEDensCell() const {return m_nBinLogEDensCell;}
  inline const float & getLogEDensCellMin() const {return m_MinLogEDensCell;}
  inline const float & getLogEDensCellMax() const {return m_MaxLogEDensCell;}

  const struct HadData * getHadData(const float & eta, const int & sampling, const float & logEClus, const float & logEDensCell) const;

  // set methods

  void setHadData(const float & eta, const int & sampling, const float & logEClus, const float & logEDensCell, const struct HadData & theData);


private:

  int m_nBinEta; 
  int m_nBinSampling; 
  int m_nBinLogEClus;
  int m_nBinLogEDensCell;

  float m_MinEta; 
  int m_MinSampling;
  float m_MinLogEClus;
  float m_MinLogEDensCell;

  float m_MaxEta; 
  int m_MaxSampling;
  float m_MaxLogEClus;
  float m_MaxLogEDensCell;

  std::vector<int> m_indexEtaSampling;
  std::vector<struct HadData> m_data;

  int getEtaSamplingBin(const float & eta, const int & sampling) const;

  int getBin(const float & eta, const int & sampling, const float & logEClus, const float & logEDensCell) const;


};//end class

CLASS_DEF(CaloHadWeight,124150759,1)

#endif
