/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyCofito_H
#define MuonboyCofito_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyCofito

   this class gives access to reconstruction results

  @author samusog@cern.ch
  
  */

class MuonboyCofito{
public:
    MuonboyCofito();
   ~MuonboyCofito();

public:
///////////////////////////////////

   int     GetNDRAMA()        ;
   int     GetNMATMA()        ;
   int     GetNBDRAT(int ISC) ;

   int     GetNBFIXE()        ; 
   int     GetIDFIXE(int NN)  ;
   int     GetISCFIXE(int NN) ;

   double  GetSSTRAKDO(int ID,int J,int ISC) ;
   double  GetZZTRAKDO(int ID,int J,int ISC) ; 
   double  GetTTTRAKDO(int ID,int J,int ISC) ;

   int     GetKOUNTERDRA(int ID,int ISC)        ;

   double  GetSSDRAT(int ID,int J,int ISC) ;
   double  GetZZDRAT(int ID,int J,int ISC) ; 
   double  GetTTDRAT(int ID,int J,int ISC) ;

   double  GetCCDRAT(int ID,int ISC) ;
   double  GetFFDRAT(int ID,int ISC) ;
   double  GetGGDRAT(int ID,int ISC) ;
   double  GetEZDRAT(int ID,int ISC) ;
   int     GetJFDRAT(int ID,int ISC) ;
   int     GetIWDRAT(int ID,int J,int ISC) ;

private:
///////////////////////////////////

   static const int NotDef = -1;

};

#endif
