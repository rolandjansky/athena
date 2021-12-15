/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArAccumulatedCalibDigit.h"
#include <math.h>
#include <iostream>
#include <limits>

/** Destructor */
LArAccumulatedCalibDigit::~LArAccumulatedCalibDigit()
{}


float LArAccumulatedCalibDigit::mean(const size_t i) const {

  if (i<m_sampleSum.size() && i<m_sample2Sum.size() && m_nTriggers>0)
    return ((double)m_sampleSum[i])/((double)m_nTriggers);
  else
    return 0.0;
}

float LArAccumulatedCalibDigit::RMS(const size_t i) const {

  if (i<m_sampleSum.size() && i<m_sample2Sum.size() && m_nTriggers>0) {
    const double mean=((double)m_sampleSum[i])/((double)m_nTriggers);
    const double rms2=((double)m_sample2Sum[i])/((double)m_nTriggers)-mean*mean;
    if (rms2<=0.0) return 0.0; //W.L 2010-12-07 protect against FPE due to rounding error
    return sqrt(rms2);
  }
  else
    return 0.0;
}


std::vector<float> LArAccumulatedCalibDigit::mean() const {

  const size_t nS=m_sampleSum.size();
  std::vector<float> mean;
  if (m_nTriggers==0)
    return mean;
  else {
    mean.resize(nS,0.0);
    for (size_t i=0;i<nS;++i)
      mean[i]=((double)m_sampleSum[i])/((double)m_nTriggers);
    return mean;
  }
}

std::vector<float> LArAccumulatedCalibDigit::RMS() const {
  std::vector<float> rms;  
  const size_t nS=m_sampleSum.size();
  if (m_sample2Sum.size()!=nS || m_nTriggers==0) 
    return rms;// ERROR, return emtpy vector
  
  double mean=0;
  double rms2=0;
  rms.resize(nS,0.0);
  
  for (size_t i=0;i<nS;++i) {
      mean=((double)m_sampleSum[i])/((double)m_nTriggers);
      rms2=((double)m_sample2Sum[i])/((double)m_nTriggers)-mean*mean;
      if (rms2<=0.0) //W.L 2010-12-07 protect against FPE due to rounding error
	rms[i]=0.0;
      else
	rms[i]=sqrt(rms2);
  }
  return rms;
}



void LArAccumulatedCalibDigit::setSampleSum(const std::vector < uint64_t >& sampleSum) {
  m_sampleSum = sampleSum;
}

void LArAccumulatedCalibDigit::setSample2Sum(const std::vector < uint64_t >& sample2Sum) {
  m_sample2Sum = sample2Sum;
}


bool LArAccumulatedCalibDigit::setAddDigit(const std::vector<short>& samples) {
   const size_t nS=samples.size();
   if (m_sampleSum.size()==0 && m_sample2Sum.size()==0) {
     m_sampleSum.resize(nS,0);
     m_sample2Sum.resize(nS,0);
   }
   else 
     if (m_sampleSum.size() != nS || m_sample2Sum.size() != nS) {
       return false; //ERROR, number of samples doesn't match!
     }

   for(size_t i=0;i<nS;++i) {
     const uint64_t s2=samples[i]*samples[i];
     if (m_sampleSum[i]>=std::numeric_limits<uint64_t>::max()-samples[i]) {
       return false; //ERROR, overflow! 
     }
     if (m_sample2Sum[i]>=std::numeric_limits<uint64_t>::max()-s2) {
       return false; //ERROR, overflow! 
     }

     m_sampleSum[i]+=samples[i];
     m_sample2Sum[i]+=s2;
     ++m_nTriggers;
   }
   return true;
}




bool LArAccumulatedCalibDigit::setAddSubStep(const std::vector < uint64_t >& sampleSum, 
					     const std::vector < uint64_t >& sample2Sum, const uint32_t nTriggerPerStep) {

 
  const size_t nS=sampleSum.size();

  if (m_sampleSum.size()==0 && m_sample2Sum.size()==0) {
      m_sampleSum.resize(nS,0);
      m_sample2Sum.resize(nS,0);
  }
  else 
    if (m_sampleSum.size() != nS || m_sample2Sum.size() != nS || sample2Sum.size() != nS) {
      return false; //ERROR, number of samples doesn't match!
    }

  for(size_t is=0; is<nS; is++) {
    if (m_sampleSum[is]>=std::numeric_limits<uint64_t>::max()-sampleSum[is]) {
      return false; //ERROR, overflow! 
    }
    if (m_sample2Sum[is]>=std::numeric_limits<uint64_t>::max()-sample2Sum[is]) {
      return false; //ERROR, overflow! 
    }

    m_sampleSum[is] += sampleSum[is];
    m_sample2Sum[is] += sample2Sum[is];
  }

  m_nTriggers += nTriggerPerStep;
  return true;
}

