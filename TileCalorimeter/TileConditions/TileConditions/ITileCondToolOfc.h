/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_ITILECONDTOOLOFC_H
#define TILECONDITIONS_ITILECONDTOOLOFC_H

#include "GaudiKernel/IAlgTool.h"


//--------------------------------------------------------------
typedef struct { // OFC weights
  float w_a[9], w_b[9], w_c[9], g[9], dg[9];   // <--- only for one gain
  int n_samples;
  bool of2;
} TileOfcWeightsStruct;
//--------------------------------------------------------------


class ITileCondToolOfc : public virtual IAlgTool {
 public:

  virtual ~ITileCondToolOfc() {}

  static const InterfaceID& interfaceID() { 
    static const InterfaceID IID_ITileCondToolOfc("ITileCondToolOfc", 1 , 0);
    return IID_ITileCondToolOfc; 
  }

  virtual const TileOfcWeightsStruct * getOfcWeights(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float& phase, bool of2) = 0 ;

  virtual int getNSamples(void) = 0 ;

};

#endif
