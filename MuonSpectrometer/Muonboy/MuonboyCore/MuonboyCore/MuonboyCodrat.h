/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyCodrat_H
#define MuonboyCodrat_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyCodrat

  @author samusog@cern.ch
  
  */

class MuonboyCodrat{
public:
    MuonboyCodrat();
   ~MuonboyCodrat();

public:
///////////////////////////////////

   int    GetNMATMA();
   int    GetNBDRAT(                 int& ISC);
   double GetCCDRAT(int& IDU,        int& ISC);
   double GetFFDRAT(int& IDU,        int& ISC);
   double GetGGDRAT(int& IDU,        int& ISC);
   double GetEZDRAT(int& IDU,        int& ISC);
   double GetSSDRAT(int& IDU,int& J, int& ISC);
   double GetZZDRAT(int& IDU,int& J, int& ISC);
   double GetTTDRAT(int& IDU,int& J, int& ISC);
   int    GetIWDRAT(int& IDU,int& J, int& ISC);
   int    GetJFDRAT(int& IDU,        int& ISC);

private:
///////////////////////////////////

   static const int NotDef = -1;

};

#endif
