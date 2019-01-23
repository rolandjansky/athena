/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
 : m_zDirection(kZ_FORWARD), m_octantNumber(0),
   m_moduleNumber(0), m_rNumber(0), m_layerNumber(0)
//////////////////////////////////
{
}

///////////////////////////////////////////////////////////
TGCReadoutIndex::TGCReadoutIndex(TGCZDirection iz, int ioct, 
				 int imd, int ir, int ilyr)
 : m_zDirection(iz), m_octantNumber(ioct),
   m_moduleNumber(imd), m_rNumber(ir), m_layerNumber(ilyr)
///////////////////////////////////////////////////////////
{
}

/////////////////////////////////////////////////////////////
TGCReadoutIndex::TGCReadoutIndex(TGCIndex tgcindex, int ilyr)
 : m_zDirection(kZ_FORWARD), m_octantNumber(0),
   m_moduleNumber(0), m_rNumber(0), m_layerNumber(0)
/////////////////////////////////////////////////////////////
{
  SetIndex(tgcindex, ilyr);
}

///////////////////////////////////////////////////////////
void TGCReadoutIndex::SetIndex(TGCIndex tgcindex, int ilyr)
///////////////////////////////////////////////////////////
{
  m_zDirection= tgcindex.GetZDirection();
  m_octantNumber= tgcindex.GetOctantNumber()-1; // 0..7

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
      m_moduleNumber= modmapE[module];
      // m_rNumber (reversed order, 0-offset)
      //if(station==T1) m_rNumber= 4- rIndex;
      //else m_rNumber= 5- rIndex;
      m_rNumber= 5- rIndex;

    } else if( region==FORWARD ) {
      m_moduleNumber= modmapF[module];
      m_rNumber= rIndex-1;
    }
  } else {    // TI
    if( region==ENDCAP ) {
      m_moduleNumber= modmapEI[module];
      m_rNumber= rIndex-1;  // 0-offset

    } else if( region==FORWARD ) {
      m_moduleNumber= modmapFI[module];
      m_rNumber= rIndex-1;
    }
  }

  // layer# 
  // 0, (1), 2, ... 5, 6 (pivot) / 7, 8 (EI/FI)
  //                        N/A  T1  T2  T3  TI
  const int lyr_offset[5]= { 0,  -1,  2,  4,  6 };
  if ((station<0) || (station>4)) {
    m_layerNumber= ilyr + lyr_offset[0];
  } else {
    m_layerNumber= ilyr + lyr_offset[station];
  }
}

///////////////////////////////////
void TGCReadoutIndex::Print() const
///////////////////////////////////
{
  std::cout << "  " << gkTgcZdirName[m_zDirection] << "-" 
            << std::setw(1) << m_octantNumber << "-"
            << std::setw(2) << m_moduleNumber << "-" 
            << m_rNumber << "-"
            << m_layerNumber;
}


} //end of namespace bracket
