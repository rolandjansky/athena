/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TgcDigit.cxx

#include "MuonDigitContainer/TgcDigit.h"

//**********************************************************************
// Member functions.
//**********************************************************************

// Default constructor.

TgcDigit::TgcDigit() : m_bcTag (BC_UNDEFINED)
{ }
 
//**********************************************************************

// Full Constructor 

TgcDigit::TgcDigit(const Identifier& id)
  : MuonDigit (id),
    m_bcTag (BC_CURRENT) { }

TgcDigit::TgcDigit(const Identifier& id, uint16_t bctag)
  : MuonDigit (id),
    m_bcTag (bctag) { }

//**********************************************************************

// Validity check.

bool TgcDigit::is_valid(const TgcIdHelper * tgcHelper) const {
  return (tgcHelper->valid(m_muonId));
}

// get BC tag
uint16_t TgcDigit::bcTag() const {
  return m_bcTag;
}
