/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArFCAL2H62004CalibCalculator_H
#define LArFCAL2H62004CalibCalculator_H

#include "LArFCALH62004CalibCalculatorBase.h"

#include "globals.hh"


class LArFCAL2H62004CalibCalculator : public  LArFCALH62004CalibCalculatorBase {

    public:

      static LArFCAL2H62004CalibCalculator* GetCalculator();

      virtual ~LArFCAL2H62004CalibCalculator();

    protected:

      LArFCAL2H62004CalibCalculator();
    private:

      static LArFCAL2H62004CalibCalculator* m_calculator;

    };
#endif
