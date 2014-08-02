/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SINGLEELECTRONVALIDATION_H
#define SINGLEELECTRONVALIDATION_H

#include "TrigEgammaValidation/FEXValidationAlgo.h"

#include "GaudiKernel/Algorithm.h"
#include "TH1F.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include <string>
#include <map>

class SingleElectronValidation : public FEXValidationAlgo {

 public:
  SingleElectronValidation(const std::string & name, ISvcLocator * pSvcLocator);
  ~SingleElectronValidation();

  StatusCode childInitialize();
  StatusCode childExecute();

  StatusCode childFinalize();

 private:

};

#endif
