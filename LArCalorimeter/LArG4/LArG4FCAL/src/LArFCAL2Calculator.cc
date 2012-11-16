/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArG4FCAL/LArFCAL2Calculator.h"
#include "LArG4FCAL/LArFCALCalculatorBase.h"

#include "globals.hh"


// Standard implementation of a singleton pattern.

LArFCAL2Calculator* LArFCAL2Calculator::m_instance = 0;

LArFCAL2Calculator* LArFCAL2Calculator::GetInstance()
{
  if (m_instance == 0) 
    {
      m_instance = new LArFCAL2Calculator();
    }
  return m_instance;
}

// constructor
LArFCAL2Calculator::LArFCAL2Calculator()
{
  m_FCalSampling = 2;
}
