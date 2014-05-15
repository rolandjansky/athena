/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtDigit.h

#ifndef MdtDigitUH
#define MdtDigitUH

// David Adams
// November 2001
//
// MDT digitization. Holds a channel ID and a TDC value.

#include <iosfwd>
#include "MuonDigitContainer/MuonDigit.h"
#include "MuonIdHelpers/MdtIdHelper.h"

class MdtDigit : public MuonDigit {

private:  // data

// TDC value.
  int m_tdc;

  // ADC value for combined measurement mode
  int m_adc;

  // Masked digit flag
  bool m_isMasked;

public:  // functions

  // Default constructor.
  MdtDigit();

  // Full constructor --- From Identifier.
  MdtDigit(const Identifier& id, int tdc);

  // Full constructor for combined measurement mode tdc+adc
  MdtDigit(const Identifier& id, int tdc, int adc);

  // Full constructor for masked digits
  MdtDigit(const Identifier& id, int tdc, int adc, bool isMasked);
  
  // Is this a valid digit?
  bool is_valid(const MdtIdHelper * mdtHelper) const;

  // Return the TDC.
  int tdc() const { return m_tdc; }

  // Return the ADC
  int adc() const { return m_adc; } 

  // Return the isMasked flag
  bool is_masked() const {return m_isMasked;}

};

#endif
