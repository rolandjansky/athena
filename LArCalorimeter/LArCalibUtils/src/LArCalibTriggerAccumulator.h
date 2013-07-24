/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-


/** 
@class LArCalibTriggerAccumulator
@brief Helper class to accumulate calibration triggers

Contains sum of ADC counts and the sum of the squares of ADC counts
So far only used by the Ramp builder. Could be used for Delay runs

@author Walter Lampl

*/



#ifndef LARCALIBTRIGGERACCUMULATOR
#define LARCALIBTRIGGERACCUMULATOR


#include <vector>
#include "stdint.h"
#include <limits>
#include <cstddef>

class LArCalibTriggerAccumulator {

private:

  /** @brief sum of samples  */
  std::vector<uint64_t> m_sampleSum;   
   
  /** @brief sum of squares of samples */
  std::vector<uint64_t> m_sample2Sum;   

  /** @brief number of triggers */
  uint32_t m_nTrigger;


 public:

  enum ERRTYPE {
    NoError=0,
    WrongNSamples,
    NumericOverflow
  };
    

  LArCalibTriggerAccumulator();

  LArCalibTriggerAccumulator(const std::vector<uint64_t>& sampleSum, 
			     const std::vector<uint64_t>& sample2Sum, uint32_t m_nTrigger);


 /** @brief return the number of triggers */
  uint32_t nTriggers() const {return m_nTrigger;}

  /** @brief return number of samples */
  size_t nsamples() const {return m_sampleSum.size();}

   /** @brief return a reference to a stl vector containing the sample-sums */
  const std::vector < uint64_t > & sampleSum() const { return m_sampleSum; }

  /** @brief return a reference to a stl vector containing the sum of the squares of the sample  */
  const std::vector < uint64_t > & sample2Sum() const { return m_sample2Sum; }

  /** @brief accumulated individual set of digits. Can return an error */
  ERRTYPE add(const std::vector<short>& digits);


  /** @brief accumulated pre-accumulated digits. Templated to allow 32 and 64 bit ints as input. Can return an error */
  template<class INTTYPE>
  ERRTYPE add(const std::vector<INTTYPE>& sampleSum, 
	      const std::vector<INTTYPE>& sample2Sum, const uint32_t nTrigger);

  /** @brief Calculates and returns the Mean value of each ADC sample */
  std::vector<float> mean() const;

  /** @brief Calculates and returns the RMS value of each ADC sample */
  std::vector<float> RMS() const;


  /** @brief Calculates and returns the Mean value for one ADC sample */
  double mean(const size_t i) const;

  /** @brief Calculates and returns the RMS value for one ADC sample */
  double RMS(const size_t i) const;

}; 


template<class INTTYPE>
LArCalibTriggerAccumulator::ERRTYPE LArCalibTriggerAccumulator::add(const std::vector<INTTYPE>& sampleSum, 
								    const std::vector<INTTYPE>& sample2Sum, const uint32_t nTrigger) {
  const size_t nS=sampleSum.size();
  if (m_sampleSum.size()==0 && m_sample2Sum.size()==0) {
    m_sampleSum.resize(nS);
    m_sample2Sum.resize(nS);
  }
  else if (m_sampleSum.size()!=nS || m_sample2Sum.size()!=nS || sample2Sum.size()!=nS) 
    return WrongNSamples; //ERROR: Inconsistent number of samples

  for (size_t i=0;i<nS;++i) {
    if (m_sample2Sum[i]>=std::numeric_limits<uint64_t>::max()-sample2Sum[i]) {
      //ERROR: Numeric overflow!
      //Roll back changes:
      for (size_t j=0;j<i;++j) {
	m_sampleSum[j]-=sampleSum[j];
	m_sample2Sum[j]-=sample2Sum[j];
      }
      return NumericOverflow;
    }
    m_sampleSum[i]+=sampleSum[i];
    m_sample2Sum[i]+=sample2Sum[i];
  }
  m_nTrigger+=nTrigger;

  return NoError;
}

#endif
