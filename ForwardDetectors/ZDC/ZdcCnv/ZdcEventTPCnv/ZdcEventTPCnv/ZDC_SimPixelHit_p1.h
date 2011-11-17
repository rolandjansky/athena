/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_SIMPIXELHIT_P1_H
#define ZDC_SIMPIXELHIT_P1_H

class ZDC_SimPixelHit_p1 {

 public:
  
  ZDC_SimPixelHit_p1() {
    Side=0;
    ModNo=0;
    PixNo=0;
    Nphotons=0;
    Edep=0;
  };
  
  friend class ZDC_SimPixelHitCnv_p1;
  
 private:
  
  int   Side;
  int   ModNo;
  int   PixNo;
  int   Nphotons;
  float Edep;
};

#endif
