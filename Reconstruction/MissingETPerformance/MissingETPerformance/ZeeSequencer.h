/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZEESEQUENCER_H
#define ZEESEQUENCER_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ITHistSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"
#include "MissingETPerformance/MissingETEventSelector.h"

//#include "AnalysisTools/AnalysisTools.h"

class ZeeSequencer : public AthAlgorithm {

 public:

  ZeeSequencer(const std::string& name, ISvcLocator* pSvcLocator);
  ~ZeeSequencer();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 private:
  ToolHandle<MissingETEventSelector> _eventSelector;
};

#endif // ZEESEQUENCER_H
