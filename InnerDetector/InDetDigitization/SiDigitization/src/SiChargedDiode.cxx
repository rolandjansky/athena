/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
SiChargedDiode::SiChargedDiode(const InDetDD::SiCellId & diode, const InDetDD::SiReadoutCellId & roCell, int flagword, SiChargedDiode * nextInCluster) 
  :  m_diode(diode),
     m_readoutCell(roCell),
     m_word(flagword),
     m_nextInCluster(nextInCluster)
{}

// add another charge
// If the current charge is large than max, update
// If we update, replace the track parameters
void SiChargedDiode::add(const SiCharge &charge) {
  const float currentCharge = std::abs(charge.charge());
  if (currentCharge > m_maxCharge) {
    m_maxCharge = currentCharge;
    // update the track distance parameters
    m_trackDistance = charge.trackDistance();
  }

  // add the charge
  m_totalCharge.add(charge);
}

///////////////////////////////////////////////////////////////////
// Input/Output stream functions:
///////////////////////////////////////////////////////////////////
std::ostream &operator<<(std::ostream &out,const SiChargedDiode &chargedDiode)
{
  out << "Diode=" << chargedDiode.diode()
      << " " << chargedDiode.totalCharge();
  return out;
}

