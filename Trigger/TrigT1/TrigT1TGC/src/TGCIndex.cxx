/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCIndex.cc
                                      QCJP, 1999
*/
// ====================================================================

#include <iostream>
#include "TrigT1TGC/TGCIndex.h"

namespace LVL1TGCTrigger {

// ====================================================================
//
//      constants and globals
//
// ====================================================================

static const char* strStation[5]= { "N/A", "T1", "T2", "T3", "TI" };
static const char* strRegion[3]= { "N/A", "F", "E" };
static const char* strModule[7]= { "N/A", "A", "B", "C", "D", "E", "F" };


// ====================================================================
//
// class description
//
// ====================================================================

////////////////////
TGCIndex::TGCIndex()
 : m_zDirection(kZ_FORWARD), m_stationType(T1), 
   m_octantNumber(0),m_regionType(FORWARD), 
   m_moduleNumber(0), m_rNumber(0)
////////////////////
{
}

//////////////////////////////////////////////////////////////////
TGCIndex::TGCIndex(TGCZDirection iz, TGCStationType ist, int ioct, 
		   TGCRegionType irgn, int imd, int ir)
 : m_zDirection(iz), m_stationType(ist), m_octantNumber(ioct),
   m_regionType(irgn), m_moduleNumber(imd), m_rNumber(ir)
/////////////////////////////////////////////////////////////////
{
}

///////////////////////////////////////////////////////////////////////
void TGCIndex::SetIndex(TGCZDirection iz, TGCStationType ist, int ioct, 
			TGCRegionType irgn, int imd, int ir)
///////////////////////////////////////////////////////////////////////
{
  m_zDirection= iz;
  m_stationType= ist;
  m_octantNumber= ioct;
  m_regionType= irgn; 
  m_moduleNumber= imd;
  m_rNumber= ir;
}

////////////////////////////
void TGCIndex::Print() const
////////////////////////////
{
  std::cout << "  " << gkTgcZdirName[m_zDirection] << "-" 
	    << strStation[m_stationType] << "-"
	    << strRegion[m_regionType] << ":" << m_octantNumber << "-"
	    << strModule[m_moduleNumber] << "-"
	    << m_rNumber;
}


} //end of namespace bracket
