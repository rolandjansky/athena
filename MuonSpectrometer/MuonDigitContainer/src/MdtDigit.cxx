/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtDigit.cxx

#include "MuonDigitContainer/MdtDigit.h"

//**********************************************************************
// Local definitions
//**********************************************************************
namespace {

// Range of TDC.
const int tdcmin = 0;
const int tdcmax = 4096;

}
//**********************************************************************
// Member functions.
//**********************************************************************

// Default constructor.

MdtDigit::MdtDigit()
: MuonDigit(), m_tdc(), m_adc(), m_isMasked(false) { }
 
//**********************************************************************

// Full constructor from Identifier.

MdtDigit::MdtDigit(const Identifier& id, int tdc)
: MuonDigit(id), m_tdc(tdc), m_adc(), m_isMasked(false) { }

//**********************************************************************

// Full constructor from Identifier for the combined measurement mode

MdtDigit::MdtDigit(const Identifier& id, int tdc, int adc)
: MuonDigit(id), m_tdc(tdc), m_adc(adc), m_isMasked(false) { }

//**********************************************************************

// Full constructor including masked flag

MdtDigit::MdtDigit(const Identifier& id, int tdc, int adc, bool isMasked)
: MuonDigit(id), m_tdc(tdc), m_adc(adc), m_isMasked(isMasked) { }

//**********************************************************************

// Validity check.

bool MdtDigit::is_valid(const MdtIdHelper * mdtHelper) const {
  return ( (m_tdc>=tdcmin && m_tdc<=tdcmax) && (mdtHelper->valid(m_muonId)));
}

