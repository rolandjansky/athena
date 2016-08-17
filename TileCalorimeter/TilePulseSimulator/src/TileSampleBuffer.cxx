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
  m_x.resize(nSamples,0.);
  m_y.resize(nSamples,0.);
  for(unsigned int i=0; i<nSamples; ++i){
    m_x[i] = xValues->At(i);
  }
}

TileSampleBuffer::TileSampleBuffer(unsigned int nSamples, double sampleStart, double stepLength)
{
  m_x.resize(nSamples,0.);
  m_y.resize(nSamples,0.);
  m_noiseVector.resize(nSamples,0.);
  for(unsigned int i=0; i<nSamples; ++i){
    m_x[i] = sampleStart + i*stepLength;
  }
}

TileSampleBuffer::~TileSampleBuffer()
{
}

unsigned int TileSampleBuffer::getMaxValueIndex()
{
  unsigned int iMax(0);
  double max(-1e100);
  for(unsigned int i=0; i<m_y.size(); ++i){
    if(m_y[i]>max) {
      max=m_y[i];
      iMax = i;
    }
  }
  return iMax;
}

void TileSampleBuffer::print(){
  std::cout << "Content of TileSampleBuffer: " << std::endl;
  for(unsigned int i=0; i<m_y.size(); ++i){
    std::cout << m_x[i] << "\t" << m_y[i] << std::endl;
  }
}
