/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTriggerAccumulator.h"
#include <cmath>

LArCalibTriggerAccumulator::LArCalibTriggerAccumulator() : m_nTrigger(0) {}

LArCalibTriggerAccumulator::LArCalibTriggerAccumulator(const std::vector<uint64_t>& sampleSum, 
						       const std::vector<uint64_t>& sample2Sum, 
						       uint32_t nTrigger) :
  m_sampleSum(sampleSum), m_sample2Sum(sample2Sum), m_nTrigger(nTrigger) {}
  


LArCalibTriggerAccumulator::ERRTYPE LArCalibTriggerAccumulator::add(const std::vector<short>& digits) {
  const size_t nS=digits.size();
  if (nS==0) return WrongNSamples;
  
  if (m_sampleSum.size()==0 && m_sample2Sum.size()==0) {
    m_sampleSum.resize(nS);
    m_sample2Sum.resize(nS);
  }
  else if (m_sampleSum.size()!=nS || m_sample2Sum.size()!=nS) 
    return WrongNSamples; //Inconsistent number of samples

  for (size_t i=0;i<nS;++i) {
    const uint32_t& dg=digits[i];
    if (m_sample2Sum[i]>=std::numeric_limits<uint64_t>::max()-dg) {
      //ERROR: Numeric overflow
      //Roll back changes:
      for (size_t j=0;j<i;++j) {
	m_sampleSum[j]-=digits[j];
	m_sample2Sum[j]-=digits[j]*digits[j];
      }
      return NumericOverflow;
    }

    m_sampleSum[i]+=dg;
    m_sample2Sum[i]+=dg*dg;
  }
  ++m_nTrigger;
  return NoError;
}



double LArCalibTriggerAccumulator::mean(const size_t i) const {

  if (i<m_sampleSum.size() && i<m_sample2Sum.size() && m_nTrigger>0)
    return ((double)m_sampleSum[i])/((double)m_nTrigger);
  else
    return 0.0;
}

double LArCalibTriggerAccumulator::RMS(const size_t i) const {

  if (i<m_sampleSum.size() && i<m_sample2Sum.size() && m_nTrigger>0) {
    const double mean=((double)m_sampleSum[i])/((double)m_nTrigger);
    return sqrt(((double)m_sample2Sum[i])/((double)m_nTrigger)-mean*mean);
  }
  else
    return 0.0;
}


std::vector<float> LArCalibTriggerAccumulator::mean() const {

  const size_t nS=m_sampleSum.size();
  std::vector<float> mean;
  if (m_nTrigger==0)
    return mean;
  else {
    mean.resize(nS,0.0);
    for (size_t i=0;i<nS;++i)
      mean[i]=((double)m_sampleSum[i])/((double)m_nTrigger);
    return mean;
  }
}

std::vector<float> LArCalibTriggerAccumulator::RMS() const {
  std::vector<float> rms;  
  const size_t nS=m_sampleSum.size();
  if (m_sample2Sum.size()!=nS || m_nTrigger==0) 
    return rms;// ERROR, return emtpy vector
  
  double mean;
  rms.resize(nS,0.0);
  
  for (size_t i=0;i<nS;++i) {
      mean=((double)m_sampleSum[i])/((double)m_nTrigger);
      rms[i]=sqrt(((double)m_sample2Sum[i])/((double)m_nTrigger)-mean*mean);
  }
  return rms;
}
