/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TrigT1TGC/TGCIndex.h"

namespace LVL1TGCTrigger {

TGCIndex::TGCIndex()
 : m_zDirection(kZ_FORWARD), m_stationType(T1), 
   m_octantNumber(0),m_regionType(FORWARD), 
   m_moduleNumber(0), m_rNumber(0)
{
}

TGCIndex::TGCIndex(TGCZDirection iz, TGCStationType ist, int ioct, 
		   TGCRegionType irgn, int imd, int ir)
 : m_zDirection(iz), m_stationType(ist), m_octantNumber(ioct),
   m_regionType(irgn), m_moduleNumber(imd), m_rNumber(ir)
{
}

void TGCIndex::SetIndex(TGCZDirection iz, TGCStationType ist, int ioct, 
			TGCRegionType irgn, int imd, int ir)
{
  m_zDirection= iz;
  m_stationType= ist;
  m_octantNumber= ioct;
  m_regionType= irgn; 
  m_moduleNumber= imd;
  m_rNumber= ir;
}

void TGCIndex::Print() const
{
  static const char* const strStation[5]= { "N/A", "T1", "T2", "T3", "TI" };
  static const char* const strRegion[3]= { "N/A", "F", "E" };
  static const char* const strModule[7]= { "N/A", "A", "B", "C", "D", "E", "F" };

  std::cout << "  " << gkTgcZdirName[m_zDirection] << "-" 
	    << strStation[m_stationType] << "-"
	    << strRegion[m_regionType] << ":" << m_octantNumber << "-"
	    << strModule[m_moduleNumber] << "-"
	    << m_rNumber;
}


} //end of namespace bracket
