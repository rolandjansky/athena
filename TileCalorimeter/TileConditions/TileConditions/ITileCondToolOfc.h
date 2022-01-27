/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_ITILECONDTOOLOFC_H
#define TILECONDITIONS_ITILECONDTOOLOFC_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"


//--------------------------------------------------------------
typedef struct { // OFC weights
  float w_a[99], w_b[99], w_c[99], g[99], dg[99];   // <--- only for one gain
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
                                   TileOfcWeightsStruct& weights,
                                   const EventContext& ctx) const  = 0;
};

#endif
