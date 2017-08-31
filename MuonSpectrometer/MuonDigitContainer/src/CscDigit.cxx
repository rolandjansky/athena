/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** CscDigit.cxx */

#include "MuonDigitContainer/CscDigit.h"
#include <algorithm>

/** Member functions */

/** Default constructor */
CscDigit::CscDigit()
: MuonDigit(), m_charge(0), m_time(0) { }
 

/** Full constructor from Identifier */

CscDigit::CscDigit(const Identifier& id, int charge)
  : MuonDigit(id), m_charge(charge), m_time(0) { }

CscDigit::CscDigit(const Identifier& id, int charge, float time)
  : MuonDigit(id), m_charge(charge), m_time(time) { }

CscDigit::CscDigit(const Identifier& id, std::vector<float> sampleCharges)
  : MuonDigit(id), m_sampleCharges(sampleCharges) {

  std::vector<float, std::allocator<float> >::iterator pkit
    = std::max_element( m_sampleCharges.begin(), m_sampleCharges.end() );

  m_charge = *pkit;
  m_time   = 0.0; // obsolete

}

/** Validity check */
bool CscDigit::is_valid(const CscIdHelper * cscHelper) const {
  return (m_charge > 0 && cscHelper->valid(m_muonId));
}

