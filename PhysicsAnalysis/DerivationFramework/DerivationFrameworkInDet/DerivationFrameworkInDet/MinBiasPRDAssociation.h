/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MINBIASD3PD_PRDASSOCIATION_H
#define MINBIASD3PD_PRDASSOCIATION_H


#include "AthenaKernel/CLASS_DEF.h"

class MinBiasPRDAssociation {
 public:
  MinBiasPRDAssociation();
  ~MinBiasPRDAssociation();

  void reset();

  int nPixelUA = 0;
  int nBlayerUA = 0;
  int nPixelBarrelUA = 0;
  int nPixelEndCapAUA = 0;
  int nPixelEndCapCUA = 0;

  int nSCTUA = 0;
  int nSCTBarrelUA = 0;
  int nSCTEndCapAUA = 0;
  int nSCTEndCapCUA = 0;

  int nTRTUA = 0;
  int nTRTBarrelUA = 0;
  int nTRTEndCapAUA = 0;
  int nTRTEndCapCUA = 0;
};

CLASS_DEF( MinBiasPRDAssociation, 95564017, 1 )

#endif
