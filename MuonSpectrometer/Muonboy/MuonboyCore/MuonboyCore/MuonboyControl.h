/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyControl_H
#define MuonboyControl_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyControl

   this class gives access to reconstruction results

  @author samusog@cern.ch
  
  */

class MuonboyControl{
public:
    MuonboyControl();
   ~MuonboyControl();

public:
///////////////////////////////////

   int    GetAtCosm();
   void   SetAtCosm    (int    AtCosmi);

private:
///////////////////////////////////

   static const int NotDef = -1;

};

#endif
