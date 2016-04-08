/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCReadoutIndex.cc
                                      QCJP, 1999
*/
// ====================================================================

#include <iostream>
#include <iomanip>
#include "TrigT1TGC/TGCReadoutIndex.h"

namespace LVL1TGCTrigger {

// ====================================================================
//
// class description
//
// ====================================================================

//////////////////////////////////
TGCReadoutIndex::TGCReadoutIndex()
 : zDirection(kZ_FORWARD), octantNumber(0),
   moduleNumber(0), rNumber(0), layerNumber(0)
//////////////////////////////////
{
}

///////////////////////////////////////////////////////////
TGCReadoutIndex::TGCReadoutIndex(TGCZDirection iz, int ioct, 
				 int imd, int ir, int ilyr)
 : zDirection(iz), octantNumber(ioct),
   moduleNumber(imd), rNumber(ir), layerNumber(ilyr)
///////////////////////////////////////////////////////////
{
}

/////////////////////////////////////////////////////////////
TGCReadoutIndex::TGCReadoutIndex(TGCIndex tgcindex, int ilyr)
 : zDirection(kZ_FORWARD), octantNumber(0),
   moduleNumber(0), rNumber(0), layerNumber(0)
/////////////////////////////////////////////////////////////
{
  SetIndex(tgcindex, ilyr);
}

///////////////////////////////////////////////////////////
void TGCReadoutIndex::SetIndex(TGCIndex tgcindex, int ilyr)
///////////////////////////////////////////////////////////
{
  zDirection= tgcindex.GetZDirection();
  octantNumber= tgcindex.GetOctantNumber()-1; // 0..7

  TGCStationType station= tgcindex.GetStationType();
  TGCRegionType region= tgcindex.GetRegionType();
  int module= tgcindex.GetModuleNumber();
  int rIndex= tgcindex.GetRNumber();

  // module# convention
  //         <---- phi ----      
  //
  //     7 6 4 3 1 0       11 10  9
  //      8   5   2      14  13 12
  // 
  //     [M1,M2, M3]         [EI/FI]  
  
  // module mapping      N/A  A  B  C  D  E  F
  const int modmapE[7] = { -1, 0, 1, 3, 4, 6, 7 };
  const int modmapF[4] = { -1, 2, 5, 8 };

  const int modmapEI[4] = { -1,  9, 10, 11 };
  const int modmapFI[4] = { -1, 12, 13, 14 };

  if( station!=TI ) {        // T1, T2, T3
    if( region==ENDCAP ) {
      moduleNumber= modmapE[module];
      // rNumber (reversed order, 0-offset)
      //if(station==T1) rNumber= 4- rIndex;
      //else rNumber= 5- rIndex;
      rNumber= 5- rIndex;

    } else if( region==FORWARD ) {
      moduleNumber= modmapF[module];
      rNumber= rIndex-1;
    }
  } else {    // TI
    if( region==ENDCAP ) {
      moduleNumber= modmapEI[module];
      rNumber= rIndex-1;  // 0-offset

    } else if( region==FORWARD ) {
      moduleNumber= modmapFI[module];
      rNumber= rIndex-1;
    }
  }

  // layer# 
  // 0, (1), 2, ... 5, 6 (pivot) / 7, 8 (EI/FI)
  //                        N/A  T1  T2  T3  TI
  const int lyr_offset[5]= { 0,  -1,  2,  4,  6 };
  if ((station<0) || (station>4)) {
    layerNumber= ilyr + lyr_offset[0];
  } else {
    layerNumber= ilyr + lyr_offset[station];
  }
}

///////////////////////////////////
void TGCReadoutIndex::Print() const
///////////////////////////////////
{
  std::cout << "  " << gkTgcZdirName[zDirection] << "-" 
            << std::setw(1) << octantNumber << "-"
            << std::setw(2) << moduleNumber << "-" 
            << rNumber << "-"
            << layerNumber;
}


} //end of namespace bracket
