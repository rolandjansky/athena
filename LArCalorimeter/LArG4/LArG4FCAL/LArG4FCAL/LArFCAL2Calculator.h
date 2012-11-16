/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArFCAL2Calculator
// Prepared 20-Feb-2003 Bill Seligman (from Peter Loch)

// Defines constants specific to a single FCAL module.

#ifndef LArFCAL2Calculator_H
#define LArFCAL2Calculator_H

#include "LArG4FCAL/LArFCALCalculatorBase.h"
#include "globals.hh"

class LArFCAL2Calculator: public LArFCALCalculatorBase
{
public:
  
  // This class follows the singleton design pattern; there can be
  // only one calculator class for this module.
  static LArFCAL2Calculator* GetInstance();

  // destructor
  virtual ~LArFCAL2Calculator() { };
  
protected:
  
  // The constructor is protected according to the singleton pattern.
  LArFCAL2Calculator();
  
private:
  
  // Private instance required by the singleton pattern.
  static LArFCAL2Calculator* m_instance;

};
#endif
