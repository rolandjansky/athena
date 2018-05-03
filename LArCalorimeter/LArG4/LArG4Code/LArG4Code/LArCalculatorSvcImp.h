/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LARCALCULATORSVCIMP_H__
#define __LARCALCULATORSVCIMP_H__

#include "ILArCalculatorSvc.h"
#include "AthenaBaseComps/AthService.h"

class LArCalculatorSvcImp: public extends<AthService, ILArCalculatorSvc> {

public:

  LArCalculatorSvcImp(const std::string& name, ISvcLocator * pSvcLocator);

protected:
  // Birks' law
  bool m_BirksLaw;

  // Birks' law, constant k
  double m_Birksk;

  // OOTcut
  double m_OOTcut;

};

#endif
