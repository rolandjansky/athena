/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  : m_sideID(0), m_moduleID(-1), m_hit_56(0), m_hit_6(0)
//////////////////////
{
}

///////////////////////////////////////////////////////////////
TGCTMDBOut::TGCTMDBOut(int side, int module, int hit56, int hit6)
  : m_sideID(side), m_moduleID(module), m_hit_56(hit56), m_hit_6(hit6)
///////////////////////////////////////////////////////////////
{
}


///////////////////////////////////////////////////////////////
TGCTMDBOut::TGCTMDBOut(const TGCTMDBOut& right)
/////////////////////////////////////////////////////////////
    :m_sideID(right.m_sideID), 
     m_moduleID(right.m_moduleID),
     m_hit_56(right.m_hit_56),
     m_hit_6(right.m_hit_6)
{
}


/////////////////////////////////////////////////////////////
TGCTMDBOut& TGCTMDBOut::operator=(const TGCTMDBOut& right)
/////////////////////////////////////////////////////////////
{
  if (this != &right) {
    m_sideID    = right.m_sideID; 
    m_moduleID  = right.m_moduleID;
    m_hit_56    = right.m_hit_56;
    m_hit_6     = right.m_hit_6;
  }
  return *this;
}

/////////////////////////////
void TGCTMDBOut::Print() const
/////////////////////////////
{
  std::cout << "Side=" << m_sideID
            << " :: ModuleID=" << m_moduleID
            << " :: Hit_56=" << m_hit_56
            << " :: Hit_6=" << m_hit_6
           << std::endl; 
}


} //end of namespace bracket
