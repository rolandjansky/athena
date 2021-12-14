/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCBIS78.h
*/
// ====================================================================
#ifndef TGC_BIS78_H
#define TGC_BIS78_H

#include <vector>
#include <memory>

#include "TrigT1TGC/TGCNumbering.h"

namespace LVL1TGCTrigger {

class BIS78TrigOut;

// ====================================================================
//
// class definition
//
// ====================================================================
 
class TGCBIS78{
public:
  TGCBIS78();
  virtual ~TGCBIS78();
 
  TGCBIS78(const TGCBIS78& right);
  const TGCBIS78& operator=(const TGCBIS78& right);
  bool operator==(const TGCBIS78& right) const
  { return (this==&right); }
  bool operator!=(const TGCBIS78& right) const
  { return (this!=&right); }

  enum { NumberOfBIS78PadBoard = 8 };

  std::shared_ptr<const BIS78TrigOut> getOutput(TGCRegionType region , int TGC_TriggerSector) const;
  void  setOutput(int BIS78PadBoard,uint8_t BIS78eta_6bit,uint8_t BIS78phi_6bit,uint8_t BIS78Deta_3bit,uint8_t BIS78Dphi_3bit,uint8_t BIS78flag3over3eta_1bit,uint8_t BIS78flag3over3phi_1bit); //
  void  eraseOutput();

  void print() const;
  protected:
  std::shared_ptr<BIS78TrigOut> m_buffer[NumberOfBIS78PadBoard];//buffer[BIS78 Trigger Processor]

};


} //end of namespace bracket

#endif
