/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZEEOFFLINEVALIDATION_H
#define ZEEOFFLINEVALIDATION_H

#include "TrigEgammaValidation/FEXValidationAlgo.h"

#include "GaudiKernel/Algorithm.h"
#include "TH1F.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include <string>
#include <map>

class ZeeOfflineValidation : public FEXValidationAlgo {

 public:
  ZeeOfflineValidation(const std::string & name, ISvcLocator * pSvcLocator);
  ~ZeeOfflineValidation();

  StatusCode childInitialize();
  StatusCode childExecute();
  StatusCode childFinalize();

};

#endif
