/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSC_PREPDATATOXAOD_H
#define CSC_PREPDATATOXAOD_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "Identifier/Identifier.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include <string>
#include <map>
#include <vector>

class CSC_PrepDataToxAOD : public AthAlgorithm  {

public:
  // Constructor with parameters:
  CSC_PrepDataToxAOD(const std::string &name,ISvcLocator *pSvcLocator);
  // Basic algorithm methods:
  virtual StatusCode initialize();
  virtual StatusCode execute();
private:
  SG::ReadHandleKey<Muon::CscPrepDataContainer> m_cscPrds;
  SG::WriteHandleKey<xAOD::TrackMeasurementValidationContainer> m_trackMeasVal;
};


#endif 
