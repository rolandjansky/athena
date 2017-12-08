/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//athena friendly provider of egamma calibrations - applies the egammaCalibTool to collections 
//author: will buttinger

// Dear emacs, this is -*- c++ -*-
#ifndef CALIBRATEDEGAMMAPROVIDER_H
#define CALIBRATEDEGAMMAPROVIDER_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "ElectronPhotonFourMomentumCorrection/IEgammaCalibrationAndSmearingTool.h"

#include "xAODCore/ShallowCopy.h"

#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/PhotonContainer.h" 
#include "xAODEgamma/EgammaContainer.h" 

namespace CP {

class CalibratedEgammaProvider : public AthAlgorithm {

public:
  /// Regular Algorithm constructor
  CalibratedEgammaProvider( const std::string& name, ISvcLocator* svcLoc );
  
  /// Function initialising the algorithm
  virtual StatusCode initialize();
  /// Function executing the algorithm
  virtual StatusCode execute();
  
private:
   std::string m_inputEventInfo;
  std::string m_inputKey;
  std::string m_outputKey;
  
  ToolHandle<CP::IEgammaCalibrationAndSmearingTool> m_tool;

}; // class 

} //namespace
#endif // CALIBRATEDEGAMMAPROVIDER_H
