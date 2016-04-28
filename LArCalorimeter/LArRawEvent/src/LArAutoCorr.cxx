/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     LArAutoCorr.cxx
 PACKAGE:  offline/LArCalorimeter/LArRawEvent
 
 AUTHORS:  F.Tarrade
 CREATED:  Jan. 22th 2009
  
 PURPOSE:  Intermediate object used to handle data
           to be used for calculation of autocorrelation
           elements.
            
********************************************************************/
// Include files
#include "LArRawEvent/LArAutoCorr.h"
 
//----------------------------------------------------------------------------
void  LArAutoCorr::set_min(const short min)
//----------------------------------------------------------------------------
{
  m_min = min;
}

//----------------------------------------------------------------------------
void  LArAutoCorr::set_max(const short max)
//----------------------------------------------------------------------------
{
  m_max = max;
} 

//----------------------------------------------------------------------------
int LArAutoCorr::get_nentries() const
//----------------------------------------------------------------------------
{
  return m_nped;
} 


//----------------------------------------------------------------------------
const short & LArAutoCorr::get_min() const
//----------------------------------------------------------------------------
{
  return m_min;
}

//----------------------------------------------------------------------------
const short & LArAutoCorr::get_max() const
//----------------------------------------------------------------------------
{
  return m_max;
}


//----------------------------------------------------------------------------
double  LArAutoCorr::get_mean() const
//----------------------------------------------------------------------------
{
  double mean = 0;

  int nsamples = m_sum.size();
  for(int i=0; i<nsamples; i++)
    mean += m_sum[i];
  mean /= ((double)(nsamples*m_nped));

  return mean;
}


//----------------------------------------------------------------------------
const  std::vector<double> & LArAutoCorr::get_cov(int normalize, int phys) 
//----------------------------------------------------------------------------
{ 
  int nsamples = m_sum.size();
  int k =0;
  if (m_nped==0) return m_cov;
  for(int i=0;i<nsamples;i++){
      for(int j=i;j<nsamples;j++,k++)
	m_cov_temp[k]= (double)((m_matrix[k]/(m_nped)) - ((m_sum[i]*m_sum[j])/(m_nped*m_nped)));
  }

  //MGV normalize covariance elements if required
  if (normalize == 1 && phys == 0 ){
    k=0;
    int si,sj;
    si=0;
    for(int i=0;i<nsamples;i++){
      sj=si;
      for(int j=i;j<nsamples;j++,k++) {
	if (m_sum[i]!=.0 && m_sum[j]!=.0) 
	  m_cov_temp[k]= m_cov_temp[k]/(double)sqrt((m_matrix[si]/(m_nped)-(m_sum[i]*m_sum[i])/(m_nped*m_nped))*(m_matrix[sj]/(m_nped)-(m_sum[j]*m_sum[j])/(m_nped*m_nped)));
	sj+=nsamples-j;
      }
      si+=nsamples-i;
    }
  }
  //MGV

  if (phys==0) {
    double sum;
    int s;
    for(int diag =1;diag<nsamples;diag++)
      {
        sum =0;
        s = 0;
        for(int i=0; i < nsamples-diag;i++)
	  {	
	    sum += m_cov_temp[s + diag];
	    s += nsamples - i;
	  }
        sum = sum/(nsamples-diag);
        m_cov[diag-1]= sum;
      }
    return m_cov;
  }
  else
    return m_cov_temp;
}

//----------------------------------------------------------------------------
double  LArAutoCorr::get_rms() const
//----------------------------------------------------------------------------
{
  double x=0, y=0;
  int k = 0;
  int nsamples = m_sum.size();

  for(int i=0; i<nsamples; i++)
    {
      x += m_sum[i];
      y += m_matrix[k];
      k += nsamples - i;// Index of diagonal element
    }
  
  x/= (double) (nsamples*m_nped);
  y/=(double) (nsamples*m_nped);
  
  double noise = sqrt(y- x*x);
  
  return noise;
}

//----------------------------------------------------------------------------
void  LArAutoCorr::add(const std::vector<short>& samples, size_t maxnsamples)
//----------------------------------------------------------------------------
{
  unsigned int nsamples = std::min(samples.size(),maxnsamples);
  int k=0;
 
  if(m_sum.size()<nsamples)
    {
    
      m_cov.resize(nsamples - 1);
      m_cov_temp.resize((nsamples*(nsamples+1))/2);
      m_sum.resize(nsamples);
      m_matrix.resize((nsamples*(nsamples+1))/2);
    }

  for(unsigned int i=0; i<nsamples; i++)
    {
      for(unsigned int j=i; j<nsamples; j++,k++)
	m_matrix[k] += ((double)(samples[j]*samples[i]));
  
      m_sum[i] += ((double) samples[i]);
    }
  m_nped++;
}

//----------------------------------------------------------------------------
void  LArAutoCorr::correl_zero()
//----------------------------------------------------------------------------
{
  int j =0;
  int nsamples = m_sum.size();
  for(int l=0; l<nsamples; l++)
    {
      for(int k=l; k<nsamples; k++,j++)
	m_matrix[j] = 0;
      m_sum[l]=0;
    }
  m_nped=0;
}
