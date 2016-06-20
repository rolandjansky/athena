/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArFCAL1H62004CalibCalculator.h"
#include "LArG4FCAL/LArFCALCalibCalculatorBase.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4Step.hh"
#include "globals.hh"

#include <algorithm>

LArFCAL1H62004CalibCalculator* LArFCAL1H62004CalibCalculator::m_calculator = 0;

LArFCAL1H62004CalibCalculator* LArFCAL1H62004CalibCalculator::GetCalculator() {
      if (m_calculator == 0 )
	{
	  m_calculator = new LArFCAL1H62004CalibCalculator();
	}
      return m_calculator;
}
    
LArFCAL1H62004CalibCalculator::LArFCAL1H62004CalibCalculator() {

      m_deltaX = 7.5*CLHEP::mm;
      m_deltaY = 7.5*CLHEP::mm*sin(60*CLHEP::deg);

      m_FCalSampling = 1;
}

LArFCAL1H62004CalibCalculator::~LArFCAL1H62004CalibCalculator(){}   
