/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCNumbering_hh
#define TGCNumbering_hh

// ====================================================================
/*
        TGCNumbering.hh
                                      QCJP, 1999
*/
// ====================================================================

namespace LVL1TGCTrigger {

enum TGCZDirection {
  Z_FORWARD=1, Z_BACKWARD, TotalNumTGCZDirection
};

enum TGCStationType { 
  First=1, Second, Third, EI, TotalNumTGCStationType,
  T1=1,    T2,     T3,    TI 
}; 

enum TGCRegionType { 
  Forward=1, Endcap, TotalNumTGCRegionType,
  FORWARD=1, ENDCAP
};

enum TGCSignalType { 
  WireGroup=1, Strip, TotalNumTGCSignalType,
  WIREGROUP=1, STRIP,
  WIRE=1
};

enum TGCSection{ INNER=1, OUTER, TotalNumTGCSection };
enum OpenMode { IN, OUT, TotalNumOpenMode };

const int NumberOfRegionType = 2;
enum TGCTDType { TripletChamber, DoubletChamber, TotalNumTDType};

enum TGCModuleType{
  ASD=0, PP, SB, HPT, SL, TotalNumModuleType,
  PatchPanel=1, SlaveBoard, HighPtBoard, SectorLogic
};

enum TGCSignalModuleType {
  WireTriplet=0, WireDoublet, StripTriplet, StripDoublet, WireInner, StripInner,
  TotalNumSignalModuleType,
  WT=0, WD, ST, SD, WI, SI
};

enum TGCSlaveBoardType { WTSB=0, WDSB, STSB, SDSB, WISB, SISB, TotalNumSlaveBoardType };
enum{ NumberOfSlaveBoardType = 6};

enum TGCForwardBackwardType { ForwardSector=0, BackwardSector=1, TotalNumForwardBackwardType };

} //end of namespace bracket

#endif // TGCNumbering_hh
