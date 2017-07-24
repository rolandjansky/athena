/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ServiceAccessor_H
#define ServiceAccessor_H


#include <iostream>
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/INTupleSvc.h"

inline INTupleSvc* ntupleSvc()
{
  static INTupleSvc* nS=0;

  if (!nS)
    {
      StatusCode status;
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      status = svcLocator->service("NTupleSvc",nS);
      if (status.isFailure())
        std::cout<<" ntupleSvc(); could not access NTupleSvc"<<std::endl;
    }
  return nS;
}

#endif
