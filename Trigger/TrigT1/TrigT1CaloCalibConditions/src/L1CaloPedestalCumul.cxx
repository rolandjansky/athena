/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPedestalCumul.h"

#include <iostream>

L1CaloPedestalCumul::L1CaloPedestalCumul() {
  m_min  = 0;
  m_max  = 0;
  m_nped = 0;
}

L1CaloPedestalCumul::~L1CaloPedestalCumul() {
}

double  L1CaloPedestalCumul::get_sum(const unsigned isample) const {
  if (isample>=m_sum.size())
    return 0;

  double sum = 0;
  sum = m_sum[isample];

  return sum;
}

double  L1CaloPedestalCumul::get_sum() const {
  double sum = 0;
  int nsamples = m_sum.size();
  for(int i=0; i<nsamples; i++)
    sum += m_sum[i];

  return sum;
}

double  L1CaloPedestalCumul::get_mean(const unsigned isample) const {
	if(m_nped==0) return -1;
  if (isample>=m_sum.size())
    return 0;
  double mean = 0;
  mean = m_sum[isample];
  mean /= ((double) m_nped);

  return mean;
}

double  L1CaloPedestalCumul::get_mean() const {
	if(m_nped==0) return -1;
  double mean = 0;
  int nsamples = m_sum.size();
  for(int i=0; i<nsamples; i++)
    mean += m_sum[i];
  mean /= ((double)(nsamples*m_nped));

  return mean;
}

double  L1CaloPedestalCumul::get_rms(const unsigned isample) const {
	if(m_nped==0) return -1;
  if (isample>=m_sum.size())
    return 0;
  double x=0, y=0;
  int k=0;
  int nsamples = m_sum.size();

  x = m_sum[isample];
  for(unsigned i=0; i<isample; i++)
    k += nsamples - i;
  y = m_matrix[k];

  double noise =(y/((double) m_nped))
    -((x*x)/((double) (m_nped*m_nped)));

  noise = sqrt(noise);
  
  return noise;
}

double  L1CaloPedestalCumul::get_rms() const {
	if(m_nped==0) return -1;
  double x=0, y=0;
  int k=0;
  int nsamples = m_sum.size();

  x = get_mean();
  for(int i=0; i<nsamples; i++)
    {
      y += m_matrix[k];
      k += nsamples - i; // Index of diagonal element
    }
  y /= (double) (nsamples*m_nped);

  double noise = sqrt(y - x*x);
  
  return noise;
}

void  L1CaloPedestalCumul::add(const std::vector<int> samples) {
  unsigned int nsamples = samples.size();
  int k=0;

  if(m_sum.size()<nsamples) {
    m_sum.resize(nsamples);
    m_matrix.resize((nsamples*(nsamples+1))/2);
  }

  for(unsigned i=0; i<nsamples; i++) {
  	for(unsigned j=i; j<nsamples; j++,k++) m_matrix[k] += ((double)(samples[j]*samples[i]));
  	m_sum[i] += ((double) samples[i]);
  }
  m_nped++;
}

void  L1CaloPedestalCumul::clear() {
  int nsamples = m_sum.size();
  int j =0;

  for(int l=0; l<nsamples; l++) {
  	for(int k=l; k<nsamples; k++,j++) m_matrix[j] = 0;
  	m_sum[l]=0;
  }
  m_nped=0;
}


