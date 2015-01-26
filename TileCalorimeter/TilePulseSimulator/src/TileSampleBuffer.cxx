/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TilePulseSimulator/TileSampleBuffer.h"

#include "TArrayD.h"
#include "TMath.h"

#include <iostream>

TileSampleBuffer::TileSampleBuffer()
{
}

TileSampleBuffer::TileSampleBuffer(TArrayD* xValues)
{
  unsigned int nSamples = xValues->GetSize();
  _x.resize(nSamples,0.);
  _y.resize(nSamples,0.);
  for(unsigned int i=0; i<nSamples; ++i){
    _x[i] = xValues->At(i);
  }
}

TileSampleBuffer::TileSampleBuffer(unsigned int nSamples, double sampleStart, double stepLength)
{
  _x.resize(nSamples,0.);
  _y.resize(nSamples,0.);
  _NoiseVector.resize(nSamples,0.);
  for(unsigned int i=0; i<nSamples; ++i){
    _x[i] = sampleStart + i*stepLength;
  }
}

TileSampleBuffer::~TileSampleBuffer()
{
}

unsigned int TileSampleBuffer::getMaxValueIndex()
{
  unsigned int iMax(0);
  double max(-1e100);
  for(unsigned int i=0; i<_y.size(); ++i){
    if(_y[i]>max) {
      max=_y[i];
      iMax = i;
    }
  }
  return iMax;
}

void TileSampleBuffer::print(){
  std::cout << "Content of TileSampleBuffer: " << std::endl;
  for(unsigned int i=0; i<_y.size(); ++i){
    std::cout << _x[i] << "\t" << _y[i] << std::endl;
  }
}
