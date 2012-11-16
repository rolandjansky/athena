/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArG4FCAL/LArFCAL1Calculator.h"
#include "LArG4FCAL/LArFCALCalculatorBase.h"

#include "globals.hh"


// Standard implementation of a singleton pattern.

LArFCAL1Calculator* LArFCAL1Calculator::m_instance = 0;

LArFCAL1Calculator* LArFCAL1Calculator::GetInstance()
{
  if (m_instance == 0) 
    {
      m_instance = new LArFCAL1Calculator();
    }
  return m_instance;
}
  
// constructor
LArFCAL1Calculator::LArFCAL1Calculator()
{
  m_FCalSampling = 1;
}
