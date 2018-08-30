 /*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */

#ifndef TRIGCOSTMONITORMT_ITRIGCOSTMTSVC_H
#define TRIGCOSTMONITORMT_ITRIGCOSTMTSVC_H

#include "GaudiKernel/IInterface.h"

class ITrigCostMTSvc : virtual public IInterface {
public:
  DeclareInterfaceID(ITrigCostMTSvc, 1, 0);

  virtual StatusCode beginAlg(const std::string& caller) = 0; 
  virtual StatusCode endAlg(const std::string& caller) = 0; 

};

#endif // TRIGCOSTMONITORMT_ITRIGCOSTMTSVC_H
