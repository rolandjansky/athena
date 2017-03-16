/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LARCALIBCALCULATORSVCIMP_H__
#define __LARCALIBCALCULATORSVCIMP_H__

#include "ILArCalibCalculatorSvc.h"
#include "AthenaBaseComps/AthService.h"

class LArCalibCalculatorSvcImp: public AthService, virtual public ILArCalibCalculatorSvc
{
public:
  LArCalibCalculatorSvcImp(const std::string& name, ISvcLocator * pSvcLocator);

  /** Query interface method to make athena happy */
  virtual StatusCode queryInterface(const InterfaceID&, void**) override final;
};

#endif
