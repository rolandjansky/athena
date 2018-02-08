// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTATHENAEGAMMACALIBTOOL_H
#define TESTATHENAEGAMMACALIBTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "EgammaAnalysisInterfaces/IEgammaCalibrationAndSmearingTool.h"

class testAthenaEgammaCalibTool : public AthAlgorithm {

public:
  /// Regular Algorithm constructor
  testAthenaEgammaCalibTool( const std::string& name, ISvcLocator* svcLoc );
  
  /// Function initialising the algorithm
  virtual StatusCode initialize();
  /// Function executing the algorithm
  virtual StatusCode execute();
  
private:
  std::string m_sg_electrons;
  
  ToolHandle<CP::IEgammaCalibrationAndSmearingTool> m_EgammaCalibrationAndSmearingTool;

}; // class testAthenaEgammaCalibTool

#endif // TESTATHENAEGAMMACALIBTOOL_H

