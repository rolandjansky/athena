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

{
}

//
//_______________________________________________________________
TileSampleGenerator::TileSampleGenerator(TilePulseShape* ps, TileSampleBuffer* buf)
  : _ps(ps)
  , _buf(buf)
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
				      TF1* pdf, bool addNoise, double itOffset = 0, double otOffset = 50)
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
