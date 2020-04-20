/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDigitContainer/sTgcDigit.h"

sTgcDigit::sTgcDigit() : m_bcTag (BC_UNDEFINED), m_charge(-1), m_time(0) { }
 
//**********************************************************************
// Full constructor from Identifier.
sTgcDigit::sTgcDigit(const Identifier& id, float time)
  : MuonDigit(id),
    m_bcTag(BC_CURRENT),
    m_charge(-1.),
    m_time(time),
    m_isDead(false),
    m_isPileup(false) { } 
//**********************************************************************
sTgcDigit::sTgcDigit(const Identifier& id, uint16_t bctag, float time)
  : MuonDigit (id),
    m_bcTag (bctag),
    m_charge(-1.),
    m_time(time),
    m_isDead(false),
    m_isPileup(false) { }    
//**********************************************************************
sTgcDigit::sTgcDigit(const Identifier& id, uint16_t bctag, float time, float charge, bool isDead, bool isPileup)
  : MuonDigit (id),
    m_bcTag(bctag),
    m_charge(charge),
    m_time(time),
    m_isDead(isDead),
    m_isPileup(isPileup) { }    
// //**********************************************************************
// Validity check.

bool sTgcDigit::is_valid( const sTgcIdHelper *sTgcHelper ) const {
  return ( (sTgcHelper->valid(m_muonId)) );
}
// //**********************************************************************
// get BC tag
uint16_t sTgcDigit::bcTag() const {
  return m_bcTag;
}
// get isDead
bool sTgcDigit::isDead() const {
  return m_isDead;
}
// get isPileup
bool sTgcDigit::isPileup() const {
  return m_isPileup;
}
// get the charge
float sTgcDigit::charge() const { 
  return m_charge;
}

int sTgcDigit::charge_6bit() const { 
 float ADC = 0.02734; // = saturation / (2^6);
 return ((int)(m_charge/ADC));
}

int sTgcDigit::charge_10bit() const { 
 float ADC = 0.00171; // = saturation / (2^10);
 return ((int)(m_charge/ADC));
}

// get the charge
float sTgcDigit::time() const { 
  return m_time; 
}

// Set bcTag
void sTgcDigit::set_bcTag(uint16_t newbcTag) { 
  m_bcTag = newbcTag; 
}

// Set the charge
void sTgcDigit::set_charge(float newCharge) { 
  m_charge = newCharge; 
}

// Set the time
void sTgcDigit::set_time(float newTime) { 
  m_time = newTime; 
}

// Set isDead
void sTgcDigit::set_isDead(bool newIsDead) { 
  m_isDead = newIsDead; 
}
// Set isPileup
void sTgcDigit::set_isPileup(bool newIsPileup) { 
  m_isPileup = newIsPileup; 
}





