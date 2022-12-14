/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCNumbering_h
#define TrigT1TGC_TGCNumbering_h

#include <string>

namespace LVL1TGC {

/** The number of sides of TGC (A- or C-side) */
static constexpr unsigned int kNSide = 2;

/** The number of octants per side */
static constexpr unsigned kNOctant = 8;

}   // namespace LVL1TGC

namespace LVL1TGCTrigger {

enum TGCZDirection {
  kZ_FORWARD = 0,         // 0
  kZ_BACKWARD,            // 1
  kTotalNumTGCZDirection  // 2
};
const std::string gkTgcZdirName[kTotalNumTGCZDirection] = {"F", "B"};

enum TGCStationType { 
  T1=1,
  T2,
  T3,
  TI,
  TotalNumTGCStationType
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

enum TGCHighPtBoardType { WHPB=0, SHPB, TotalNumHighPtBoardType };

} //end of namespace bracket

#endif // TGCNumbering_hh
