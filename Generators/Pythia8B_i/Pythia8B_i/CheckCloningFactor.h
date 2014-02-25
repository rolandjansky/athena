/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

/// Dump MC event record
class CheckCloningFactor : public AthAlgorithm {
public:

  CheckCloningFactor(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  std::string m_key;
  double m_tolerance;
  int m_eventCount;
  std::vector<double> bqKinematics[3]; 
};

