/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONEFFICIENCYCORRECTION_TESTATHENAELECTRONALG_H
#define ELECTRONEFFICIENCYCORRECTION_TESTATHENAELECTRONALG_H 

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// forward declarations
class IAsgElectronEfficiencyCorrectionTool;

class testAthenaElectronAlg: public ::AthAlgorithm { 
 public: 
  testAthenaElectronAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~testAthenaElectronAlg(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 
 /// The tool handles our electron efficiency corrections
 ToolHandle< IAsgElectronEfficiencyCorrectionTool > m_electronSF;
}; 

#endif //> !ELECTRONEFFICIENCYCORRECTION_TESTATHENAELECTRONALG_H
