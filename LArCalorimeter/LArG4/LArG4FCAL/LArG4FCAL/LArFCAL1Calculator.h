/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArFCAL1Calculator
// Prepared 20-Feb-2003 Bill Seligman (from Peter Loch)

// Defines constants specific to a single FCAL module.

#ifndef LArFCAL1Calculator_H
#define LArFCAL1Calculator_H

#include "LArG4FCAL/LArFCALCalculatorBase.h"
#include "globals.hh"

class LArFCAL1Calculator: public LArFCALCalculatorBase
{
public:
  
  // This class follows the singleton design pattern; there can be
  // only one calculator class for this module.
  static LArFCAL1Calculator* GetInstance();

  // destructor
  virtual ~LArFCAL1Calculator() { };
  
protected:
  
  // The constructor is protected according to the singleton pattern.
  LArFCAL1Calculator();
  
private:
  
  // Private instance required by the singleton pattern.
  static LArFCAL1Calculator* m_instance;

};
#endif
