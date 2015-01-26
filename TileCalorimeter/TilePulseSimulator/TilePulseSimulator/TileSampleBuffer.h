/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILESAMPLEBUFFER__H
#define TILESAMPLEBUFFER__H

#include "TObject.h"
#include <vector>

class TArrayD;


class TileSampleBuffer : public TObject{
 public:
  TileSampleBuffer();
  TileSampleBuffer(TArrayD* xValues);
  TileSampleBuffer(unsigned int nSamples, double sampleStart, double stepLength);
  virtual ~TileSampleBuffer();

  inline void   setValue(unsigned int i, double val) {if(i<_y.size()) _y[i]=(float)val;}
  float getValue(unsigned int i);
  double getTime(unsigned int i);
  inline unsigned int size(){return _x.size();}
  unsigned int getMaxValueIndex();
  void print();
  inline void   setValueNoise(unsigned int i, double val) {if(i<_NoiseVector.size()) _NoiseVector[i]=(float)val;}
  float getValueNoise(unsigned int i);
  void getValueVector(std::vector<float> &vec) {vec = _y;}
  
 private:
  std::vector<double> _x;
  std::vector<float> _y;
  std::vector<float> _NoiseVector;
};


inline float TileSampleBuffer::getValue(unsigned int i)
{
  if(i<_y.size()) return _y[i];
  else            return -999.;
}

inline double TileSampleBuffer::getTime(unsigned int i) {
  if(i<_x.size()) return _x[i];
  else            return -999.;
}
inline float TileSampleBuffer::getValueNoise(unsigned int i)
{
  if(i<_NoiseVector.size()) return _NoiseVector[i];
  else            return -999.;
}

#endif
