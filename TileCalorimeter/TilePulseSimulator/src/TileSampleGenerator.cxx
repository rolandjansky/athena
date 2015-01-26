/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TilePulseSimulator/TileSampleGenerator.h"
#include "TilePulseSimulator/TilePulseShape.h"
#include "TilePulseSimulator/TileSampleBuffer.h"

#include "TRandom3.h"
#include "TArrayD.h"
#include "TMath.h"
#include "TF1.h"

#include <iostream>
#include <cstdio>

using namespace std;

//
//_______________________________________________________________
TileSampleGenerator::TileSampleGenerator()
  : _ps(0)
  , _buf(0)
  , m_DEBUG(false)

{
}

//
//_______________________________________________________________
TileSampleGenerator::TileSampleGenerator(TilePulseShape* ps, TileSampleBuffer* buf, bool dd_DEBUG)
  : _ps(ps)
  , _buf(buf)
  , m_DEBUG(dd_DEBUG)
{
}

//
//_______________________________________________________________
TileSampleGenerator::~TileSampleGenerator()
{

}

//
//_______________________________________________________________
void TileSampleGenerator::fillSamples(double t0, double pedestal, 
				      double amplitude1, double amplitude2,
				      TF1* pdf, bool addNoise, double itOffset, double otOffset)
{
  for(unsigned int i=0; i<_buf->size(); ++i)
  {
    double x1 = _buf->getTime(i)-t0+itOffset;
    double y1 = _ps->eval(x1) * amplitude1;
    double x2 = _buf->getTime(i)-t0+otOffset;
    double y2 = _ps->eval(x2) * amplitude2;
    double y = y1 + y2 + pedestal;

    double noise(0), val(y);
    if(addNoise) {
        noise = pdf->GetRandom();
        val += noise;
    }

    _buf->setValueNoise(i,(noise));
    _buf->setValue(i,val);
  }
}

//
//________________________________________________________
void TileSampleGenerator::fill7Samples(double t0, double pedestal, double amp_it,
                                       std::vector<float> amp_pu, TF1* pdf, bool addNoise, double itOffset)
{
  
  TileSampleBuffer* m_bufall = new TileSampleBuffer(21, -250., 25.);
  
  for( int i=0; i<7; i++){   //Loop over output samples
    double tin, amp_it_out; 
    int nPul=amp_pu.size();
    std::vector<int> t(nPul); 
    std::vector<float> amp_pu_out(nPul);
    
    double amp_total=pedestal;
    
        if(m_DEBUG)  std::cout << "sample to compute: " << i << "  " << amp_total << std::endl;
    
    for( int j=0; j<21; j++){     //Loop over PU pulses

      t[j]=m_bufall->getTime(17+i-j)-t0; 
      
          if(m_DEBUG)  std::cout << "pileupsample to compute " << j << std::endl;
          if(m_DEBUG)  std::cout << "                time in " << i << " " << j<< " " << (17+i-j)<< " " << " buf " << m_bufall->getTime(17+i-j) << "  time_out " << t[j]  << std::endl;
      
      if(t[j] < -75. || t[j] > 125.) continue;  //Limits of the PulseShape
      amp_pu_out[j] = _ps->eval(t[j]) * amp_pu.at(j);  // PU Contribution
      amp_total+=amp_pu_out[j];
      
          if(m_DEBUG)  std::cout << "                amp_pu  " << amp_pu.at(j) << "  ps  " << _ps->eval(t[j]) << "  amp_out " << amp_pu_out[j]<< std::endl;
          if(m_DEBUG)  std::cout << "                amp_total " << amp_total << std::endl;
    }
    
    tin = _buf->getTime(i)-t0+itOffset;
    amp_it_out = _ps->eval(tin) * amp_it; //Contribution from In-time Pulse
    amp_total+=amp_it_out;
    
          if(m_DEBUG)  std::cout << "      INTIME    amp_it  " << amp_it    << "  ps  " << _ps->eval(tin) << "  amp_it_out " << amp_it_out<< std::endl;
          if(m_DEBUG)  std::cout << "                amp_total " << amp_total << std::endl;
    
    
    double noise(0), val(amp_total);
    if(addNoise) {
      noise = pdf->GetRandom();
      val += noise;
    }
          if(m_DEBUG)  std::cout << "                                     FINAL     " << amp_total << std::endl;
    _buf->setValueNoise(i,(noise));
    _buf->setValue(i,val);
  }
}