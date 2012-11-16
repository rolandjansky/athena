/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArG4FCAL/LArFCAL3Calculator.h"
#include "LArG4FCAL/LArFCALCalculatorBase.h"

#include "globals.hh"


// Standard implementation of a singleton pattern.

LArFCAL3Calculator* LArFCAL3Calculator::m_instance = 0;

LArFCAL3Calculator* LArFCAL3Calculator::GetInstance()
{
  if (m_instance == 0) 
    {
      m_instance = new LArFCAL3Calculator();
    }
  return m_instance;
}


// constructor
LArFCAL3Calculator::LArFCAL3Calculator()
{
  m_FCalSampling = 3;
}
