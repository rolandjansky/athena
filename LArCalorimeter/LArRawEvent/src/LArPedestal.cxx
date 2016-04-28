/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     LArPedestal.cxx
 PACKAGE:  offline/LArCalorimeter/LArRawEvent
 
 AUTHORS:  M. AHARROUCHE
 CREATED:  Jan. 12, 2004
 UPDATED:  Mar. 09, 2004 Remi Lafaye  

 PURPOSE:  Interchanges the data with LArPedestalMaker for 
           calculation of pedestal and rms.
            
 ********************************************************************/
#include "CaloIdentifier/CaloGain.h"
#include "LArRawEvent/LArPedestal.h"
#include <iostream>

//----------------------------------------------------------------------------
LArPedestal::LArPedestal()
//----------------------------------------------------------------------------
{
  m_min  = -1;
  m_max  = -1;
  m_nped = 0;
}

//----------------------------------------------------------------------------
LArPedestal::~LArPedestal()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void  LArPedestal::set_min(const short min)
//----------------------------------------------------------------------------
{
  m_min = min;
}

//----------------------------------------------------------------------------
void  LArPedestal::set_max(const short max)
//----------------------------------------------------------------------------
{
  m_max = max;
}

//----------------------------------------------------------------------------
double  LArPedestal::get_sum(const unsigned isample) const
//----------------------------------------------------------------------------
{
  if (isample>=m_sum.size())
    return 0;

  return  m_sum[isample];
}

//----------------------------------------------------------------------------
double  LArPedestal::get_sum() const
//----------------------------------------------------------------------------
{
  double sum = 0;
  int nsamples = m_sum.size();
  for(int i=0; i<nsamples; i++)
    sum += m_sum[i];

  return sum;
}

//----------------------------------------------------------------------------
double  LArPedestal::get_mean(const unsigned isample_min, const unsigned isample_max) const
//----------------------------------------------------------------------------
{
  unsigned imin=0;
  unsigned imax=m_sum.size()-1;

  if (isample_min > 0 && isample_min < m_sum.size()) imin=isample_min;
  if (isample_max > 0 && isample_max < m_sum.size()) imax=isample_max;
  double mean = 0;
  for(unsigned i=0; i<=imax; ++i) mean += m_sum[i];
  mean /= ((imax-imin+1)*(double) m_nped);

  return mean;
}
//----------------------------------------------------------------------------
double  LArPedestal::get_mean(const unsigned isample) const
//----------------------------------------------------------------------------
{
  if (isample>=m_sum.size() || m_nped==0)
    return 0;
  double mean = 0;
  mean = m_sum[isample];
  mean /= ((double) m_nped);

  return mean;
}

//----------------------------------------------------------------------------
double  LArPedestal::get_mean() const
//----------------------------------------------------------------------------
{ 
  const int nsamples = m_sum.size();
  if (nsamples==0 || m_nped==0) return 0;
  uint32_t imean = 0;
  for(int i=0; i<nsamples; i++)
    imean += m_sum[i];

  return double(imean)/(nsamples*m_nped);


}

//----------------------------------------------------------------------------
double  LArPedestal::get_rms(const unsigned isample_min, const unsigned isample_max) const
//----------------------------------------------------------------------------
{
  unsigned imin=0;
  unsigned imax=m_sum.size()-1;

  if (isample_min > 0 && isample_min < m_sum.size()) imin=isample_min;
  if (isample_max > 0 && isample_max < m_sum.size()) imax=isample_max;
  //const int nsamples = m_sum.size();
  uint64_t x=0, y=0;
  for(unsigned i=imin; i<=imax; i++) {
    x+=m_sum[i];
    y+=m_sumSquares[i];
  }
  
  const double mean=double(x)/((imax-imin+1)*m_nped);
  const double ss=double(y)/((imax-imin+1)*m_nped);
  return sqrt(ss-mean*mean);
}


//----------------------------------------------------------------------------
double  LArPedestal::get_rms(const unsigned isample) const
//----------------------------------------------------------------------------
{
  if (isample>=m_sum.size() || m_nped==0)
    return 0;
  //const int nsamples = m_sum.size();
  const double x = m_sum[isample]/double(m_nped);
  const double y = m_sumSquares[isample]/double(m_nped);;
  return sqrt(y-x*x);
}

//----------------------------------------------------------------------------
double  LArPedestal::get_rms() const
//----------------------------------------------------------------------------
{
  const int nsamples = m_sum.size();
  if (nsamples==0 || m_nped==0) return 0;
  uint64_t x=0, y=0;
  for(int i=0; i<nsamples; i++) {
    x+=m_sum[i];
    y+=m_sumSquares[i];
  }
  
  const double mean=double(x)/(nsamples*m_nped);
  const double ss=double(y)/(nsamples*m_nped);
  return sqrt(ss-mean*mean);
  //double noise=y-x*x
  //return sqrt(noise/(m_nped*m_nped*nsamples*nsamples));
}

//----------------------------------------------------------------------------
void  LArPedestal::add(const std::vector<short>& samples)
//----------------------------------------------------------------------------
{
  const size_t nsamples = samples.size();

  if(m_sum.size()<nsamples) {
    m_sum.resize(nsamples);
    m_sumSquares.resize(nsamples);
  }

  for(size_t i=0; i<nsamples; i++) {
    m_sum[i] += samples[i];
    m_sumSquares[i] += (samples[i]*samples[i]);
  }
  m_nped++;
}

//----------------------------------------------------------------------------
void  LArPedestal::zero()
//----------------------------------------------------------------------------
{
  const int nsamples = m_sum.size();
  for(int l=0; l<nsamples; l++) {
    m_sumSquares[l] = 0;
    m_sum[l]=0;
  }
  m_nped=0;
}


