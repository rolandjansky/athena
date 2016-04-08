/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
 : zDirection(kZ_FORWARD), stationType(T1), 
   octantNumber(0),regionType(FORWARD), 
   moduleNumber(0), rNumber(0)
////////////////////
{
}

//////////////////////////////////////////////////////////////////
TGCIndex::TGCIndex(TGCZDirection iz, TGCStationType ist, int ioct, 
		   TGCRegionType irgn, int imd, int ir)
 : zDirection(iz), stationType(ist), octantNumber(ioct),
   regionType(irgn), moduleNumber(imd), rNumber(ir)
/////////////////////////////////////////////////////////////////
{
}

///////////////////////////////////////////////////////////////////////
void TGCIndex::SetIndex(TGCZDirection iz, TGCStationType ist, int ioct, 
			TGCRegionType irgn, int imd, int ir)
///////////////////////////////////////////////////////////////////////
{
  zDirection= iz;
  stationType= ist;
  octantNumber= ioct;
  regionType= irgn; 
  moduleNumber= imd;
  rNumber= ir;
}

////////////////////////////
void TGCIndex::Print() const
////////////////////////////
{
  std::cout << "  " << gkTgcZdirName[zDirection] << "-" 
	    << strStation[stationType] << "-"
	    << strRegion[regionType] << ":" << octantNumber << "-"
	    << strModule[moduleNumber] << "-"
	    << rNumber;
}


} //end of namespace bracket
