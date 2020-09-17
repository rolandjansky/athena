/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ServiceAccessor_H
#define ServiceAccessor_H


#include <iostream>
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/INTupleSvc.h"

inline INTupleSvc* ntupleSvc()
{
  auto findNtupleSvc = []()
    {
      INTupleSvc* nS = nullptr;
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      StatusCode status = svcLocator->service("NTupleSvc",nS);
      if (status.isFailure())
        std::cout<<" ntupleSvc(); could not access NTupleSvc"<<std::endl;
      return nS;
    };
  static INTupleSvc* const nS = findNtupleSvc();
  return nS;
}

#endif
