/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyComusc_H
#define MuonboyComusc_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyComusc

   this class gives access to reconstruction results

  @author samusog@cern.ch
  
  */

class MuonboyComusc{
public:
    MuonboyComusc();
   ~MuonboyComusc();

public:
///////////////////////////////////

   void Print1();
   void Print2();

   double RRMUSC(int Index);
   double RRMASC(int Index);
   double X0MASC(int Index);
   double S1MASC(int Index);
   double S2MASC(int Index);
   double THMASC(int Index);
   double ALMASC(int Index);
   double BLMASC(int Index);
   double VinVouMASC(int Ivinvouparam,int Index);
   double ENMASC();
   double SIGENMASC();
   double RRMESU(int Index);
   double DDMESU(int Index);
   double FFMESU(int Index);
   int    NBMUSC();
   int    JVMUSC(int Index);
   int    NBMASC();
   int    JVMASC(int Index);
   int    LLMASC(int Index);
   int    IDMASC(int Index);
   int    NBMESU();

   double XposMASC(int Index);
   double YposMASC(int Index);
   double ZposMASC(int Index);
   double XdirMASC(int Index);
   double YdirMASC(int Index);
   double ZdirMASC(int Index);
   double PmomMASC(int Index);
   double ChgeMASC(int Index);
   int    IValMASC(int Index);


   void ResetMBCOMUSC();

private:
///////////////////////////////////

   static const int NotDef = -1;

   void SetNBMUSC(int NBMUSCi);
   void SetNBMASC(int NBMASCi);
   void SetNBMESU(int NBMESUi);
   
};

#endif

