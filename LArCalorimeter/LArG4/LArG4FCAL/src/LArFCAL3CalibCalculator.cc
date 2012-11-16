/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArG4FCAL/LArFCAL3CalibCalculator.h"
#include "LArG4FCAL/LArFCALCalibCalculatorBase.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4Step.hh"
#include "globals.hh"

#include <algorithm>

namespace LArG4 {

  namespace FCAL {

    LArFCAL3CalibCalculator* LArFCAL3CalibCalculator::m_calculator = 0;

    LArFCAL3CalibCalculator* LArFCAL3CalibCalculator::GetCalculator()

    {
      if (m_calculator == 0 )
	{
	  m_calculator = new LArFCAL3CalibCalculator();
	}
      return m_calculator;
    }
    
    LArFCAL3CalibCalculator::LArFCAL3CalibCalculator()
    {

      m_deltaX = 9.0*CLHEP::mm;
      m_deltaY = 9.0*CLHEP::mm*sin(60*CLHEP::deg);

      m_FCalSampling = 3;
    }
    LArFCAL3CalibCalculator::~LArFCAL3CalibCalculator(){} 
  }

}
