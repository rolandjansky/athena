/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCafJobs/ShapeInfo.h"

#include "LArCafJobs/ClassCounts.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TMath.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


double ShapeInfo::m_timeShift = 0;

// Samples, phases :
// 5samples, 17phases means that for each sample we have 17 points ........x........ where x is the sample
// However the edge points correspond to the samples before and after so we have
// For 5 samples  ........x.......x.......x.......x.......x........ (7 points in-between but 8 before and after)
// So for N samples, P phases, we have Npoints = P' + (N- 1)*P' + 1 + P' = (N+1)*P' + 1 
// where P' = (P-1)/2.

ShapeInfo::ShapeInfo(unsigned char nSamples, unsigned char binSize, unsigned char nPhases, float shift)
  : m_nSamples(nSamples), m_binSize(binSize), m_nPhases(nPhases), m_shift(shift)
{
  ClassCounts::incrementInstanceCount("ShapeInfo"); 
  unsigned int nPoints = nIntervals()*(nSamples - 1) + nPhases; 
  m_values.resize(nPoints, 0);
}


ShapeInfo::ShapeInfo(const ShapeInfo& other)
 : m_values(other.m_values),
   m_nSamples(other.m_nSamples), m_binSize(other.m_binSize), 
   m_nPhases(other.m_nPhases), m_shift(other.m_shift) 
{
  ClassCounts::incrementInstanceCount("ShapeInfo"); 
}


ShapeInfo::~ShapeInfo()
{ 
  ClassCounts::decrementInstanceCount("ShapeInfo"); 
}


bool ShapeInfo::isValid() const
{
  return (m_values.size() > 0);
}


double ShapeInfo::value(unsigned int i) const 
{ 
  if (i < m_values.size()) return double(m_values[i])/32767;
  if (i == m_values.size() && m_values.size() >= 2) 
    return 2*value(i - 1) - value(i - 2);
  return -1E99;
}

unsigned char ShapeInfo::phase(unsigned int i)  const
{
  return (i % nIntervals());
}


void ShapeInfo::set(unsigned char iSample, unsigned char iPhase, float value)
{
  unsigned int index = iPhase + iSample*nIntervals();
  m_values[index] = int(value*32767 + 0.5) + (value > 0 ? 0 : -1);
}
