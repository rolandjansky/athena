/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LArFCAL1CalibCalculator_H
#define LArFCAL1CalibCalculator_H

//#include "LArG4FCAL/LArFCAL1CalibCalculator.h"
#include "LArG4FCAL/LArFCALCalibCalculatorBase.h"

#include "globals.hh"


// Standard implementation of a singleton pattern.
namespace LArG4 {

  namespace FCAL {

    class LArFCAL1CalibCalculator : public  LArFCALCalibCalculatorBase {

    public:

      static LArFCAL1CalibCalculator* GetCalculator();

      virtual ~LArFCAL1CalibCalculator();
      
    protected: 

      LArFCAL1CalibCalculator();
    private:

      static LArFCAL1CalibCalculator* m_calculator;

    };
    
  }
  
}
#endif
