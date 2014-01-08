/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyComcan_H
#define MuonboyComcan_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyComcan

   this class gives access to reconstruction results

  @author samusog@cern.ch
  
  */

class MuonboyComcan{
public:
    MuonboyComcan();
   ~MuonboyComcan();

public:
///////////////////////////////////

   void   Print(std::ostream* out);

   void    SetNBCAN(int Value_i);
   
   void    SetCOXCAN(int TrackNber,int ParameterNberI,int ParameterNberJ,double Value_i);
   void    SetPAXCAN(int TrackNber,int ParameterNber,double Value_i);
   void    SetVXXCAN(int TrackNber,int ComponentNber,double Value_i);

   void    SetCHICAN(int TrackNber,double Value_i);
   void    SetNDFCAN(int TrackNber,int    Value_i);
   void    SetCXXCAN(int TrackNber,int FitKind,double Value_i);

   void    SetNBMSXX(int TrackNber,int    Value_i);
   void    SetNZXCAN(int TrackNber,int    Value_i);
   void    SetNSXCAN(int TrackNber,int    Value_i);
   void    SetNZOCAN(int TrackNber,int    Value_i);
   void    SetNHXCAN(int TrackNber,int    Value_i);
   void    SetISECAN(int TrackNber,int    Value_i);
   void    SetFLGCAN(int TrackNber,int    Value_i);


   int    NBCAN();
   int    NFICAN();

   int    FLGCAN(int TrackNber);

   double CHICAN(int TrackNber);
   double CXXCAN(int TrackNber,int FitKind);
   double VMUCAN(int TrackNber,int ComponentNber);

   double COXCAN(int TrackNber,int ParameterNberI,int ParameterNberJ);
   double PAXCAN(int TrackNber,int ParameterNber);
   double VXXCAN(int TrackNber,int ComponentNber);

   double COYCAN(int TrackNber,int ParameterNberI,int ParameterNberJ);
   double PAYCAN(int TrackNber,int ParameterNber);
   double VYYCAN(int TrackNber,int ComponentNber);

   double COZCAN(int TrackNber,int ParameterNberI,int ParameterNberJ);
   double PAZCAN(int TrackNber,int ParameterNber);
   double VZZCAN(int TrackNber,int ComponentNber);

   int    NBMSXX(int TrackNber);
   double RRMSXX(int TrackNber,int MulScatNber);
   double X0MSXX(int TrackNber,int MulScatNber);
   double S1MSXX(int TrackNber,int MulScatNber);
   double S2MSXX(int TrackNber,int MulScatNber);
   double THMSXX(int TrackNber,int MulScatNber);
   double ENMSXX(int TrackNber);
   double EAMSXX(int TrackNber,int MulScatNber);
   double EBMSXX(int TrackNber,int MulScatNber);
   double VinVouXX (int TrackNber,int MulScatNber,int Ivinvouparam);
   double VinVouXX2(int TrackNber,int MulScatNber,int Ivinvouparam);
   int    LLMSXX(int TrackNber,int MulScatNber);

   double RZXCAN(int TrackNber,int Index);
   double RZICAN(int TrackNber,int Index);
   double RZISIM(int TrackNber,int Index);
   double EZIFIT(int TrackNber,int Index);
   double RSXCAN(int TrackNber,int Index);
   double RSISIM(int TrackNber,int Index);
   double ESIFIT(int TrackNber,int Index);
   double XDCCAN(int TrackNber,int Index);
   double YDCCAN(int TrackNber,int Index);
   double ZDCCAN(int TrackNber,int Index);
   double ENCCAN(int TrackNber,int Index);
   double RDCCAN(int TrackNber,int Index);
   double SDCCAN(int TrackNber,int Index);
   double XDCdCAN(int TrackNber,int Index);
   double YDCdCAN(int TrackNber,int Index);
   double ZDCdCAN(int TrackNber,int Index);
   double XSDCCAN(int TrackNber,int Index);
   double YSDCCAN(int TrackNber,int Index);
   double ZSDCCAN(int TrackNber,int Index);
   double ESNCCAN(int TrackNber,int Index);
   double RSDCCAN(int TrackNber,int Index);
   double XSDCdCAN(int TrackNber,int Index);
   double YSDCdCAN(int TrackNber,int Index);
   double ZSDCdCAN(int TrackNber,int Index);
   int    NDFCAN(int TrackNber);
   int    ISECAN(int TrackNber);
   int    ITICAN(int TrackNber,int Index);
   int    NWICAN(int TrackNber,int Index);
   int    ITXCAN(int TrackNber,int Index);
   int    NWXCAN(int TrackNber,int Index);
   int    MWXCAN(int TrackNber,int Index);
   int    NZXCAN(int TrackNber);
   int    NZOCAN(int TrackNber);
   int    IZXCAN(int TrackNber,int Index);
   int    NSXCAN(int TrackNber);
   int    ISXCAN(int TrackNber,int Index);

//Holes   
   int    NHXCAN(int TrackNber);
   int    IHXCAN(int TrackNber,int HoleNber);
   int    TYHCAN(int TrackNber,int HoleNber);
   double ENHCAN(int TrackNber,int HoleNber);
   double XHACAN(int TrackNber,int HoleNber);
   double YHACAN(int TrackNber,int HoleNber);
   double ZHACAN(int TrackNber,int HoleNber);
   double XHdCAN(int TrackNber,int HoleNber);
   double YHdCAN(int TrackNber,int HoleNber);
   double ZHdCAN(int TrackNber,int HoleNber);

   int    Index1stCo(int TrackNber, int index);
   int    Index2ndCo(int TrackNber, int index);

private:
///////////////////////////////////

   static const int NotDef = -1;

};

#endif

