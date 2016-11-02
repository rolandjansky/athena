/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiChargedDiode.cxx
//   Implementation file for class SiChargedDiode
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 2.1 09/06/2001 David Calvet
// Davide Costanzo. Revisited version. 04-03-03
///////////////////////////////////////////////////////////////////

#include "SiDigitization/SiChargedDiode.h"


// Constructor with parameters:
SiChargedDiode::SiChargedDiode(const SiTotalCharge::alloc_t& alloc,
                               const InDetDD::SiCellId & diode, const InDetDD::SiReadoutCellId & roCell,  int flagword, SiChargedDiode * nextInCluster) 
  :  m_diode(diode),
     m_totalCharge(alloc),
     m_readoutCell(roCell),
     m_word(flagword),
     m_nextInCluster(nextInCluster)
{}


///////////////////////////////////////////////////////////////////
// Input/Output stream functions:
///////////////////////////////////////////////////////////////////
std::ostream &operator<<(std::ostream &out,const SiChargedDiode &chargedDiode)
{
  out << "Diode=" << chargedDiode.diode()
      << " " << chargedDiode.totalCharge();
  return out;
}

