/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCNSW.h
*/
// ====================================================================
#ifndef TGC_NSW_H
#define TGC_NSW_H

#include <vector>
#include <memory>

#include "TrigT1TGC/TGCNumbering.h"

namespace LVL1TGCTrigger {

class NSWTrigOut;

// ====================================================================
//
// class definition
//
// ====================================================================
 
class TGCNSW{
public:
  TGCNSW();
  virtual ~TGCNSW();
 
  TGCNSW(const TGCNSW& right);
  const TGCNSW& operator=(const TGCNSW& right);
  bool operator==(const TGCNSW& right) const
  { return (this==&right); }
  bool operator!=(const TGCNSW& right) const
  { return (this!=&right); }

  enum { NumberOfNSWTriggerProcesser = 16 };

  std::shared_ptr<const NSWTrigOut> getOutput(TGCRegionType region ,int side, int TGC_TriggerSector) const;
  void  setOutput(int side, int NSWTriggerProcesser,int NSWeta_8bit,int NSWphi_6bit,int NSWDtheta_5bit); //eta:0.005 phi:10mrad Dtheta:1mrad 
  void  eraseOutput();
  int   getNumberOfNSWTriggerProcesser() const { return NumberOfNSWTriggerProcesser; };   

  void print() const;
  enum { Aside=0 , Cside=1 };
  protected:
  std::shared_ptr<NSWTrigOut> m_buffer[2][NumberOfNSWTriggerProcesser];//buffer[Aside or Cside][NSW Trigger Processor]

};


} //end of namespace bracket

#endif
