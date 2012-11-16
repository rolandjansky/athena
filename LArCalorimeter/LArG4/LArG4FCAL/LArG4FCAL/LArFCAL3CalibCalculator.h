/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LArFCAL3CalibCalculator_H
#define LArFCAL3CalibCalculator_H

//#include "LArG4FCAL/LArFCAL3CalibCalculator.h"
#include "LArG4FCAL/LArFCALCalibCalculatorBase.h"

#include "globals.hh"


// Standard implementation of a singleton pattern.
namespace LArG4 {

  namespace FCAL {

    class LArFCAL3CalibCalculator : public  LArFCALCalibCalculatorBase {

    public:

      static LArFCAL3CalibCalculator* GetCalculator();

      virtual ~LArFCAL3CalibCalculator();
      
    protected: 

      LArFCAL3CalibCalculator();
    private:

      static LArFCAL3CalibCalculator* m_calculator;

    };
    
  }
  
}
#endif
