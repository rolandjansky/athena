/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArAccumulatedDigit.h"
#include <math.h>
#include <iostream>

/** default constructor for persistency */
LArAccumulatedDigit::LArAccumulatedDigit() :
  m_gain(CaloGain::UNKNOWNGAIN),  m_nTrigger(0)
{}

LArAccumulatedDigit::LArAccumulatedDigit(HWIdentifier & channel_value, 
					 CaloGain::CaloGain gain_value, 
					 const std::vector<uint32_t>& sampleSum_value, 
					 const std::vector< uint32_t >& sampleSquare_value,
					 uint32_t nTrigger_value) :
  m_hardwareID(channel_value), m_gain(gain_value), m_nTrigger(nTrigger_value)  {
  
  const size_t nS=sampleSum_value.size();
  m_sampleSum.resize(nS);
  for (size_t i=0;i<nS;++i) 
    m_sampleSum[i]=(uint64_t)sampleSum_value[i];

  
  const size_t nSS=sampleSquare_value.size();
  m_sampleSquare.resize(nSS);
  for (size_t i=0;i<nSS;++i) 
    m_sampleSquare[i]=(uint64_t)sampleSquare_value[i];
}

  /** @brief Constructor (second type)*/      
LArAccumulatedDigit::LArAccumulatedDigit(HWIdentifier & channel_value)
  : m_hardwareID (channel_value),
    m_gain       ((CaloGain::CaloGain) 0),
    m_nTrigger   (0)
{
}

/** Destructor */
LArAccumulatedDigit::~LArAccumulatedDigit() {}


float LArAccumulatedDigit::mean(int n_min, int n_max) const {
  //float mean;
  unsigned imin=0;
  const size_t nS=m_sampleSum.size();
  if(nS==0) return 0.;
  if(n_min>0 && (unsigned)n_min<nS) imin=n_min;
  unsigned imax=nS-1;
  if(n_max>0 && n_max>n_min && (unsigned)n_max<nS) imax=n_max;

  const double n    = (imax-imin+1)*m_nTrigger;
  if(n<=0) return 0;
  uint64_t x=0;
  for(size_t i=imin;i<=imax;i++) {
    //std::cout << "Computing mean: " << x << " += " << m_sampleSum[i] << " [" << i << "] ";
    x += m_sampleSum[i];
    //std::cout << " = " << x << std::endl;
  }
  //std::cout << x << " /= " << n << std::endl;
  //mean = x/n;
  //std::cout << " = " << mean << std::endl;
  return x/n;
}

float LArAccumulatedDigit::RMS(int n_min, int n_max) const 
{
  unsigned imin=0; 
  const size_t nS=m_sampleSum.size();
  if(nS==0) return 0.;
  if(n_min>0 && (unsigned)n_min<nS) imin=n_min;
  unsigned imax=nS-1;
  if(n_max>0 && n_max>n_min && (unsigned)n_max<nS) imax=n_max;

  const double n    = (imax-imin+1)*m_nTrigger;
  if(n<=0) return 0;
  if(m_sampleSquare.size()<1) return 0;
  
  uint64_t x=0;
  for(size_t i=imin;i<=imax;i++)
    x += m_sampleSum[i];
  const double inv_n = 1. / n;
  double mean2 = x * inv_n;
  mean2=mean2*mean2;
  const double rms2=m_sampleSquare[0]*inv_n - mean2;
  if (rms2<0.0) { //W.L 2010-12-07 protect against FPE due to rounding error
    //std::cout << "ERROR negative squareroot:" << rms2 <<  std::endl;
    return 0.0;
  }
  float  rms   = sqrt(rms2);
  return rms;
}

bool LArAccumulatedDigit::setAddDigit(const std::vector<short>& digit) {
  const size_t nS=digit.size();
  if(nS!=m_sampleSum.size() || nS!=m_sampleSquare.size()) {
    if (!m_nTrigger)
      return false;
    m_sampleSquare.resize(nS,0);
    m_sampleSum.resize(nS,0);
  }// end if object empty
   

  for (size_t i=0;i<nS;++i) {
    m_sampleSum[i]+=digit[i];
    for (size_t j=i;j<nS;++j)
      m_sampleSquare[j]+=digit[i]*digit[j];
  }
  ++m_nTrigger;
  return true;
}



bool LArAccumulatedDigit::setAddSubStep(const CaloGain::CaloGain gain_value,
                                        const HWIdentifier chid, 
					const std::vector<uint64_t>& sampleSum,
                                        const std::vector <uint64_t>& sampleSquare, 
					const unsigned nTrigger)
{
  size_t i;

  if(gain_value!=m_gain || chid!=m_hardwareID) {
    if(m_nTrigger!=0)
      return false;
    m_gain=gain_value;
    m_hardwareID=chid;
  }
  

  const size_t n = sampleSquare.size();
  if(n!=sampleSum.size())
    return false; // Can not accumulate if nsamples differs
  if(n!=m_sampleSum.size() || n!=m_sampleSquare.size()) {
    if(m_nTrigger!=0) 
      return false; // Can not accumulate if nsamples differs
    m_sampleSquare.resize(n,0);
    m_sampleSum.resize(n,0);
  }
  for(i=0;i<n;++i)
    m_sampleSum[i] += sampleSum[i];
  
  for(i=0;i<n;i++)
    m_sampleSquare[i] += sampleSquare[i];

  m_nTrigger += nTrigger;
  return true;
}


void LArAccumulatedDigit::setAddSubStep(CaloGain::CaloGain gain_value, std::vector<uint32_t> sampleSum, 
					std::vector <uint32_t> sampleSquare, unsigned nTrigger)
{
  std::vector<uint64_t> tmpSum;
  std::vector<uint64_t> tmpSquare;
  unsigned i,n;
  const size_t nS=m_sampleSum.size();
  if(gain_value!=m_gain) {
    if(m_nTrigger!=0)
      return;
    m_gain=gain_value;
  }
  n = sampleSquare.size();
  if(n!=sampleSum.size())
    return; // Can not accumulate if nsamples differs
  if(n!=nS) {
    if(m_nTrigger!=0) 
      return; // Can not accumulate if nsamples differs
    m_sampleSquare.resize(n,0);
    m_sampleSum.resize(n,0);
  }

  tmpSum.resize(n,0);
  for(i=0;i<n;i++) {
    tmpSum[i] = sampleSum[i];
    m_sampleSum[i] += sampleSum[i];
  }
  
  tmpSquare.resize(n,0);
  for(i=0;i<n;i++) {
    tmpSquare[i] = sampleSquare[i];
    m_sampleSquare[i] += sampleSquare[i];
  }

  m_nTrigger += nTrigger;
  tmpSum.clear();
  tmpSquare.clear();
}

void LArAccumulatedDigit::setAddSubStep(CaloGain::CaloGain gain_value, std::vector<int32_t> sampleSum, 
					std::vector <int32_t> sampleSquare, unsigned nTrigger,
					int32_t base)
{
  int64_t  tmpBase;
  std::vector<uint64_t> tmpSum;
  std::vector<uint64_t> tmpSquare;
  unsigned i,n;
  const size_t nS=m_sampleSum.size();
  if(gain_value!=m_gain) {
    if(m_nTrigger!=0)
      return;
    m_gain=gain_value;
  }
  n = sampleSquare.size();
  if(n!=sampleSum.size())
    return; // Can not accumulate if nsamples differs
  if(n!=nS) {
    if(m_nTrigger!=0) 
      return; // Can not accumulate if nsamples differs
    m_sampleSquare.resize(n,0);
    m_sampleSum.resize(n,0);
  }
  tmpBase = base;
  tmpSum.resize(n,0);
  for(i=0;i<n;i++) {
    tmpSum[i] = (uint64_t) (sampleSum[i]+tmpBase);
    m_sampleSum[i] += tmpSum[i];
  }
  
  tmpBase = static_cast<int64_t>(base)*base;
  tmpSquare.resize(n,0);
  for(i=0;i<n;i++) {
    tmpSquare[i] = (uint64_t) (sampleSquare[i]+tmpBase);
    m_sampleSquare[i] += tmpSquare[i];
  }
  
  m_nTrigger += nTrigger;

  tmpSum.clear();
  tmpSquare.clear();
}


void LArAccumulatedDigit::getCov(std::vector<float>& cov, int normalize) const 
{
  cov.clear();
  const double n    = m_nTrigger;
  const size_t nS=m_sampleSum.size();
  if(n<=0) return;
  if(m_sampleSquare.size()!=nS) return;

  std::vector<double> mean2;
  mean2.resize(nS);
  for(size_t i=1;i<nS;i++) {
    for(size_t j=i;j<nS;j++) {
      mean2[i] += m_sampleSum[j-i]*m_sampleSum[j];
    }
    mean2[i]/=n*n*(nS-i);
  }
  uint64_t temp_mean = 0;
  double mean;
  for(uint32_t i=0;i<nS;i++)
    temp_mean += m_sampleSum[i];
  mean = temp_mean/((double) n*nS);

  if(normalize!=1) { // No mormalization send as is
    for(size_t i=1;i<nS;i++)
      cov.push_back( m_sampleSquare[i]/(n*(nS-i)) - mean2[i] );
    mean2.clear();
    return;
  }

  //Normalize covariance elements if required
  const double norm = m_sampleSquare[0]/(n*nS)-mean*mean;
  //norm = sqrt(norm*norm);
  if (norm==0.0) 
    cov.assign(nS-1,0.0);
  else {
    const double inv_norm = 1. / norm;
    for(uint32_t i=1;i<nS;i++) {
      cov.push_back((m_sampleSquare[i]/(n*(nS-i)) - mean2[i])*inv_norm);
    }
  }

  mean2.clear();
  return;
}

