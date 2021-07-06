/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCTMDBOut.h"

#include <iostream>

namespace LVL1TGCTrigger {

TGCTMDBOut::TGCTMDBOut()
 : m_sideID(0), m_moduleID(-1), m_hit_56(TM_NA), m_hit_6(TM_NA)
{
}

TGCTMDBOut::TGCTMDBOut(const int side, const int module, const TileModuleHit hit56, const TileModuleHit hit6)
 : m_sideID(side), m_moduleID(module), m_hit_56(hit56), m_hit_6(hit6)
{
}

void TGCTMDBOut::print() const
{
  std::cout << "Side=" << m_sideID
            << " :: ModuleID=" << m_moduleID
            << " :: Hit_56=" << m_hit_56
            << " :: Hit_6=" << m_hit_6
            << std::endl;
}


}   // end of namespace
