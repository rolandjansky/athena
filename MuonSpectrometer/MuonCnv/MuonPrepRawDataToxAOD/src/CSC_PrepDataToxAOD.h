/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CSC_PrepDataToxAOD.h
//   Header file for class CSC_PrepDataToxAOD
///////////////////////////////////////////////////////////////////

#ifndef CSC_PREPDATATOXAOD_H
#define CSC_PREPDATATOXAOD_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include <map>
#include <vector>

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "Identifier/Identifier.h"

#include "xAODTracking/TrackMeasurementValidationContainer.h"

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

class CSC_PrepDataToxAOD : public AthAlgorithm  {

public:
  // Constructor with parameters:
  CSC_PrepDataToxAOD(const std::string &name,ISvcLocator *pSvcLocator);

  // Basic algorithm methods:
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  SG::ReadHandleKey<Muon::CscPrepDataContainer> m_cscPrds;
  SG::WriteHandleKey<xAOD::TrackMeasurementValidationContainer> m_trackMeasVal;
  ToolHandle<Muon::MuonIdHelperTool>    m_idHelper;
};


#endif 
