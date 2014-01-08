/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyCointr_H
#define MuonboyCointr_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyCointr

   this class gives access to reconstruction results

  @author samusog@cern.ch
  
  */

class MuonboyCointr{
public:
    MuonboyCointr();
   ~MuonboyCointr();

public:
///////////////////////////////////

   void   Print(std::ostream* out);

   int    NBINTR();
   
   double PTINTR(int SegmentNber,int ComponentNber);
   double VEINTR(int SegmentNber,int ComponentNber);
   double CHINTR(int SegmentNber);
   double T0INTR(int SegmentNber);
   double ET0INTR(int SegmentNber);
   
   int    MlNber(int SegmentNber);
   int    ITINTR(int SegmentNber);
   int    NUINTR(int SegmentNber);
   int    NAINTR(int SegmentNber);
   
   double RdigINTR(int SegmentNber,int HitNber);
   double RtraINTR(int SegmentNber,int HitNber);
   double XDINTR(int SegmentNber,int HitNber);
   double YDINTR(int SegmentNber,int HitNber);
   double ZDINTR(int SegmentNber,int HitNber);
   double RDINTR(int SegmentNber,int HitNber);
   double XDdINTR(int SegmentNber,int HitNber);
   double YDdINTR(int SegmentNber,int HitNber);
   double ZDdINTR(int SegmentNber,int HitNber);
   int    IUINTR(int SegmentNber,int HitNber);
   double DXINTR(int SegmentNber,int ComponentNber);
   double DYINTR(int SegmentNber,int ComponentNber);
   double DZINTR(int SegmentNber,int ComponentNber);
   double ERINTR(int SegmentNber,int ComponentNber);
   int    IERINTR(int SegmentNber);
  
   int    IKINTR(int SegmentNber);
  
//Holes   
   int    NHINTR(int SegmentNber);
   int    IHINTR(int SegmentNber,int HoleNber);
   int    TYHINTR(int SegmentNber,int HoleNber);
   double XHINTR(int SegmentNber,int HoleNber);
   double YHINTR(int SegmentNber,int HoleNber);
   double ZHINTR(int SegmentNber,int HoleNber);
   double XHdINTR(int SegmentNber,int HoleNber);
   double YHdINTR(int SegmentNber,int HoleNber);
   double ZHdINTR(int SegmentNber,int HoleNber);

   std::string StationName(int SegmentNber);
   int         ItypStationName(int SegmentNber);

   int    LinearIndex(int SegmentNber,int HitNber);
   double Rdigi(int SegmentNber,int HitNber);
   double Rtrak(int SegmentNber,int HitNber);

private:
///////////////////////////////////

   static const int NotDef = -1;

};

#endif
