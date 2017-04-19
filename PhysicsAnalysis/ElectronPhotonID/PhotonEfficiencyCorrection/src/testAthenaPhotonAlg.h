/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONEFFICIENCYCORRECTION_TESTATHENAPHOTONALG_H
#define PHOTONEFFICIENCYCORRECTION_TESTATHENAPHOTONALG_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// forward declarations
class IAsgPhotonEfficiencyCorrectionTool;

class testAthenaPhotonAlg: public ::AthAlgorithm { 
 public: 
  testAthenaPhotonAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~testAthenaPhotonAlg(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 
 /// The tool handle to our photon efficiency correction
 ToolHandle< IAsgPhotonEfficiencyCorrectionTool > m_photonSF;
}; 

#endif //> !PHOTONEFFICIENCYCORRECTION_TESTATHENAPHOTONALG_H
