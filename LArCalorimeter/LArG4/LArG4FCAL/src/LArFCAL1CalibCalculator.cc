/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArG4FCAL/LArFCAL1CalibCalculator.h"
#include "LArG4FCAL/LArFCALCalibCalculatorBase.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4Step.hh"
#include "globals.hh"

#include <algorithm>

namespace LArG4 {

  namespace FCAL {

    LArFCAL1CalibCalculator* LArFCAL1CalibCalculator::m_calculator = 0;

    LArFCAL1CalibCalculator* LArFCAL1CalibCalculator::GetCalculator()

    {
      if (m_calculator == 0 )
	{
	  m_calculator = new LArFCAL1CalibCalculator();
	}
      return m_calculator;
    }
    
    LArFCAL1CalibCalculator::LArFCAL1CalibCalculator()
    {

      m_deltaX = 7.5*CLHEP::mm;
      m_deltaY = 7.5*CLHEP::mm*sin(60*CLHEP::deg);

      m_FCalSampling = 1;
    }
    LArFCAL1CalibCalculator::~LArFCAL1CalibCalculator(){}   
  }

}
