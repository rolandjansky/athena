/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MINBIASD3PD_PRDASSOCIATION_H
#define MINBIASD3PD_PRDASSOCIATION_H


#include "CLIDSvc/CLASS_DEF.h"

class MinBiasPRDAssociation {
 public:
  MinBiasPRDAssociation();
  ~MinBiasPRDAssociation();

  void reset();

  int nPixelUA;
  int nBlayerUA;
  int nPixelBarrelUA;
  int nPixelEndCapAUA;
  int nPixelEndCapCUA;

  int nSCTUA;
  int nSCTBarrelUA;
  int nSCTEndCapAUA;
  int nSCTEndCapCUA;

  int nTRTUA;
  int nTRTBarrelUA;
  int nTRTEndCapAUA;
  int nTRTEndCapCUA;
};

CLASS_DEF( MinBiasPRDAssociation, 95564017, 1 )

#endif
