/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCTMDBOut.cc
                                      
*/
// ====================================================================

#include <iostream>
#include <iomanip>

#include "TrigT1TGC/TGCTMDBOut.h"

namespace LVL1TGCTrigger {

// ====================================================================
//
// class description
//
// ====================================================================

//////////////////////
TGCTMDBOut::TGCTMDBOut()
  : sideID(0), moduleID(-1), hit_56(0), hit_6(0)
//////////////////////
{
}

///////////////////////////////////////////////////////////////
TGCTMDBOut::TGCTMDBOut(int side, int module, int hit56, int hit6)
  : sideID(side), moduleID(module), hit_56(hit56), hit_6(hit6)
///////////////////////////////////////////////////////////////
{
}


///////////////////////////////////////////////////////////////
TGCTMDBOut::TGCTMDBOut(const TGCTMDBOut& right)
/////////////////////////////////////////////////////////////
    :sideID(right.sideID), 
     moduleID(right.moduleID),
     hit_56(right.hit_56),
     hit_6(right.hit_6)
{
}


/////////////////////////////////////////////////////////////
TGCTMDBOut& TGCTMDBOut::operator=(const TGCTMDBOut& right)
/////////////////////////////////////////////////////////////
{
  if (this != &right) {
    sideID    = right.sideID; 
    moduleID  = right.moduleID;
    hit_56    = right.hit_56;
    hit_6     = right.hit_6;
  }
  return *this;
}

/////////////////////////////
void TGCTMDBOut::Print() const
/////////////////////////////
{
  std::cout << "Side=" << sideID
            << " :: ModuleID=" << moduleID
            << " :: Hit_56=" << hit_56
            << " :: Hit_6=" << hit_6
           << std::endl; 
}


} //end of namespace bracket
