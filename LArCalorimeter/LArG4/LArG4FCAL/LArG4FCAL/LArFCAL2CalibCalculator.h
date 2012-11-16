/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LArFCAL2CalibCalculator_H
#define LArFCAL2CalibCalculator_H

//#include "LArG4FCAL/LArFCAL2CalibCalculator.h"
#include "LArG4FCAL/LArFCALCalibCalculatorBase.h"

#include "globals.hh"


// Standard implementation of a singleton pattern.
namespace LArG4 {

  namespace FCAL {

    class LArFCAL2CalibCalculator : public  LArFCALCalibCalculatorBase {

    public:

      static LArFCAL2CalibCalculator* GetCalculator();

      virtual ~LArFCAL2CalibCalculator();
      
    protected: 

      LArFCAL2CalibCalculator();
    private:

      static LArFCAL2CalibCalculator* m_calculator;

    };
    
  }
  
}
#endif
