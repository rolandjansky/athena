/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonDigit.h

#ifndef MuonDigitUH
#define MuonDigitUH

// Ketevi A. Assamagan
// November 2002
//
// Muon Digit holds an identifier.

#include <iosfwd>
#include "Identifier/Identifiable.h"
#include "Identifier/Identifier.h"

class MuonDigit : public Identifiable {

protected:  // data

  // ID.
  Identifier m_muonId;

public:  // functions

  MuonDigit () { }
  MuonDigit(const Identifier& id) {m_muonId = id;}
  void setID(const Identifier id) {m_muonId = id;}
  Identifier identify() const {return m_muonId;}

};

#endif


