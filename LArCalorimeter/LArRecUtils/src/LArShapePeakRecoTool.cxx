/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecUtils/LArShapePeakRecoTool.h"

#include <algorithm>
#include <cmath>
#include <stdio.h>

// *****************************************************************************
// Tool to reconstruct ADCreco from the calibration profile (Inspired by EMTB )
// Author: S. Laplace - Nov. 2004
//
//******************************************************************************


LArShapePeakRecoTool::LArShapePeakRecoTool(const std::string& type, const std::string& name, 
						 const IInterface* parent) 
  : AthAlgTool(type, name, parent)
{}

StatusCode LArShapePeakRecoTool::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode LArShapePeakRecoTool::finalize()
{return StatusCode::SUCCESS;}

// input vector of samples must be pedestal substracted
std::vector<float> LArShapePeakRecoTool::peak (const std::vector<float>& samples, const std::vector<double>& wave ) const 
{
  static std::vector<float> solution; 
  solution.clear();

  float  shape_max,delay_max;
  float  sample_max,adc_max;
  int    i,s,s1,s2,d,d1,d2;
  double chi2,chi2_best,yi2,yigi,gi2,lambda=0;
  int    nbin;

  nbin = wave.size();

  // find maximum of delay profile
  const std::vector<double>::const_iterator wave_max=max_element(wave.begin(),wave.end());
  if (wave_max==wave.end())
    {ATH_MSG_ERROR( "Maximum of Delay Profile not found!"  );
    return solution; 
    }
  shape_max = *wave_max;

  delay_max = distance(wave.begin(),wave_max);

  //  std::cout << "wave[0] = " << wave[0] << std::endl;
  
  // parabola approximation of shape maximum
  d = (int) delay_max;
  GetShapeParMax(delay_max,shape_max,delay_max-1,wave[d-1],
		 delay_max,wave[d], delay_max+1,wave[d+1]);
  
  // find maximum of ADC samples
  
  const std::vector<float>::const_iterator it_max=max_element(samples.begin(),samples.end());
  if (it_max==samples.end())
    {ATH_MSG_ERROR( "Maximum ADC sample not found!"  );
    return solution; 
    }
  adc_max    = *it_max;
  sample_max = distance(samples.begin(),it_max);

  // Get sample limits
  //  GetShapeSampleLimits(s1,s2,sample_max,delay_max,nbin,samples.size());

  // find 2nd maximum of ADC samples
  std::vector<float>::const_iterator it_sample = samples.begin();
  std::vector<float>::const_iterator it_sample_end = samples.end();

  float adc_2ndmax = 0;
  int   sample_2ndmax = static_cast<int> (sample_max);
  for (;it_sample!=it_sample_end;it_sample++) { //Loop over sample vector
    if((*it_sample>adc_2ndmax) & (*it_sample<=adc_max)) {
      adc_2ndmax = *it_sample;
      sample_2ndmax = distance(samples.begin(),it_sample);
    }
  }

  // determine s1 and s2
  if(std::abs(sample_max-sample_2ndmax)>1) std::cout << "maxima are far away ? index(max) = " << sample_max << ", index(2nd max) = " << sample_2ndmax << ", samples = " << samples[0] << ", " << samples[1] << ", " << samples[2] << ", " << samples[3] << ", " << samples[4] << ", " << samples[5] << ", " << std::endl;

  if(sample_max-sample_2ndmax > 0) {
    s1 = sample_2ndmax - 1;
    s2 = static_cast<int> (sample_max + 1);
  }else{
    s1 = static_cast<int> (sample_max - 1);
    s2 = sample_2ndmax + 1;
  }

  //  std::cout << "sample_max = " << sample_max << ", sample_2ndmax = " << sample_2ndmax << ", s1 = " << s1 << ", s2 = " << s2 << std::endl;

  // Get delay Limits
  d1 = 0;
  d2 = nbin-1-(s2-s1)*24;

  //  std::cout << "d2 = " << d2 << ", s1 = " << s1 << ", s2 = " << s2 << std::endl;
  //  std::cout << "sample max = " << sample_max << std::endl;

  // Loop on all delays to find the best chi2
  // i.e. minimise 
  // chi2 = yi2-yigi*lambda = yi2-yigi*yigi/gi2
  // where yi : samples in ADC counts
  //       gi : normalised pulse shape
  // maximum is then given by the lagrange factor
  // lambda = yigi/gi2 in ADC counts

  chi2_best=1e30;
  int delay_best=-999;

  for(d=d1;d<=d2;d++)
    {
      yi2=0; gi2=0; yigi=0;
      
      for(s=s1;s<=s2;s++) {
	i = (s-s1)*24+d;
	yi2+=samples[s]*samples[s];
	gi2+=wave[i]*wave[i];
	yigi+=wave[i]*samples[s];
      }

      if(gi2<=0) chi2=chi2_best;
      else chi2=yi2-yigi*yigi/gi2;

      if(chi2<chi2_best)
	{
	  chi2_best=chi2;
	  delay_best=d;
	  lambda=yigi/gi2;
	}
    }

  if(delay_best<=-999) { return solution; }

  float adc_reco = lambda*shape_max;

  //  std::cout << "shape max = " << shape_max << ", adc max = " << adc_max << ", lambda = " << lambda << std::endl;
  //  std::cout << "delay best = " << delay_best << std::endl;

  float time_reco = delay_best;
  if(delay_best<adc_max) time_reco = delay_best;
  
  solution.push_back(adc_reco);
  solution.push_back(time_reco);

  return solution;
}

// Find maximum of three points shape with a Parabola approximation
void LArShapePeakRecoTool::GetShapeParMax(float &xmax, float &ymax, float x1, float y1, 
					  float x2, float y2, float x3, float y3) const
{
  float a,b,c;
  float x12, x13, xx12, xx13, y12, y13;
  float n,d;

  y13  = y1 - y3;
  y12  = y1 - y2;
  x13  = x1 - x3;
  x12  = x1 - x2;
  xx12 = x1*x1 - x2*x2;
  xx13 = x1*x1 - x3*x3;

  n    = y13  - x13/x12*y12;
  d    = xx13 - x13/x12*xx12;
  a    = n/d;
  b    = (y12 - a*xx12)/x12;
  c    = y1 - a*x1*x1 - b*x1;

  xmax   = -b/2/a;
  ymax   = a*xmax*xmax+b*xmax+c;
}

// Find sample Limits
void LArShapePeakRecoTool::GetShapeSampleLimits(int &s1, int &s2, float sample_max, 
						float delay_max, int nbin, int nsample) const
{
  s1=0; s2=nsample-1;

  s1 = (int) (sample_max - (delay_max - 5.)/25.) +1;
  if(s1<0) s1=0;
  s2 = (int) (sample_max + (nbin - delay_max - 5.)/25.);
  if(s2>=nsample) s2=nsample-1;

  while(s2-s1>=3) {
    if(s2-sample_max>sample_max-s1) s2--;
    else s1++;
  }

}

