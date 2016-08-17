/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEPULSESIMULATOR_TILESAMPLEBUFFER_H
#define TILEPULSESIMULATOR_TILESAMPLEBUFFER_H

#include "TObject.h"
#include <vector>

class TArrayD;


class TileSampleBuffer : public TObject{
 public:
  TileSampleBuffer();
  TileSampleBuffer(TArrayD* xValues);
  TileSampleBuffer(unsigned int nSamples, double sampleStart, double stepLength);
  virtual ~TileSampleBuffer();

  inline void   setValue(unsigned int i, double val) {if(i<m_y.size()) m_y[i]=(float)val;}
  float getValue(unsigned int i);
  double getTime(unsigned int i);
  inline unsigned int size(){return m_x.size();}
  unsigned int getMaxValueIndex();
  void print();
  inline void   setValueNoise(unsigned int i, double val) {if(i<m_noiseVector.size()) m_noiseVector[i]=(float)val;}
  float getValueNoise(unsigned int i);
  void getValueVector(std::vector<float> &vec) {vec = m_y;}
  
 private:
  std::vector<double> m_x;
  std::vector<float> m_y;
  std::vector<float> m_noiseVector;
};


inline float TileSampleBuffer::getValue(unsigned int i)
{
  if(i<m_y.size()) return m_y[i];
  else            return -999.;
}

inline double TileSampleBuffer::getTime(unsigned int i) {
  if(i<m_x.size()) return m_x[i];
  else            return -999.;
}
inline float TileSampleBuffer::getValueNoise(unsigned int i)
{
  if(i<m_noiseVector.size()) return m_noiseVector[i];
  else            return -999.;
}

#endif
