/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  DeclareInterfaceID(ITileCondToolOfc, 1, 0);

  virtual ~ITileCondToolOfc() {}

  virtual StatusCode getOfcWeights(unsigned int drawerIdx,
                                   unsigned int channel,
                                   unsigned int adc,
                                   float& phase,
                                   bool of2,
                                   TileOfcWeightsStruct& weights) const  = 0;
};

#endif
