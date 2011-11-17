/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_SIMSTRIPHIT_P1_H
#define ZDC_SIMSTRIPHIT_P1_H

class ZDC_SimStripHit_p1 {

 public:
  
  ZDC_SimStripHit_p1() {
    Side=0;
    ModNo=0;
    Nphotons=0;
    Edep=0;
  };
  
  friend class ZDC_SimStripHitCnv_p1;
  
 private:
  
  int   Side;
  int   ModNo;
  int   Nphotons;
  float Edep;
};

#endif
