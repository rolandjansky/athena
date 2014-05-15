/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TgcDigit.h

#ifndef TgcDigitUH
#define TgcDigitUH

// Ketevi A. Assamagan
// September 27, 2002
// Nov 03, 05 YH add bcid
//
// TGC digitization. Holds a channel ID.

#include <iosfwd>
#include <inttypes.h>
#include "MuonDigitContainer/MuonDigit.h"
#include "MuonIdHelpers/TgcIdHelper.h"

class TgcDigit : public MuonDigit {

public:  // functions

  // Default constructor.
  TgcDigit();

  // Full Constructor 
  TgcDigit(const Identifier& id);
  TgcDigit(const Identifier& id, uint16_t bctag);

  // Is this a valid digit?
  bool is_valid(const TgcIdHelper * tgcHelper) const;

  // get BC Tag
  uint16_t bcTag() const;
  enum {BC_UNDEFINED=0, BC_PREVIOUS, BC_CURRENT, BC_NEXT};

private: // bctag
  uint16_t  m_bcTag;

};

#endif
