/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILESAMPLEGENERATOR__HPP__
#define TILESAMPLEGENERATOR__HPP__

#include "TObject.h"
#include "TH1F.h"
#include "TF1.h"

class TilePulseShape;
class TileSampleBuffer;
class TRandom3;


class TileSampleGenerator : public TObject{
 public:
  TileSampleGenerator();
  TileSampleGenerator(TilePulseShape* ps, TileSampleBuffer* buf);
  virtual ~TileSampleGenerator();

  inline void setPulseShape(TilePulseShape* ps){_ps=ps;}
  void fillSamples(double t0, double pedestal, double amplitude1, double amplitude2, TF1* pdf, bool addNoise, double itOffset, double otOffset);

 private:
  TilePulseShape* _ps;
  TileSampleBuffer* _buf;

 public:

};


#endif
