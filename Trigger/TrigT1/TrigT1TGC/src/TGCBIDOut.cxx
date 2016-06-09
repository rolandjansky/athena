/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCBIDOut.hh"

namespace LVL1TGCTrigger {

  TGCBIDOut::TGCBIDOut():TGCASDOut(),bid(0)
  {
  }
  

  TGCBIDOut::TGCBIDOut(const TGCASDOut* ASDOut):TGCASDOut(*ASDOut),bid(0)
  {
  }



} //end of namespace bracket
