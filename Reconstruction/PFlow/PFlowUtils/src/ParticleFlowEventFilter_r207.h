/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEFLOWEVENTFILTER_r207_H
#define PARTICLEFLOWEVENTFILTER_r207_H

#include "AthenaBaseComps/AthAlgorithm.h"

class ParticleFlowEventFilter_r207 : public AthAlgorithm {

public:
  ParticleFlowEventFilter_r207(const std::string& name, ISvcLocator* pSvcLocator);
  ~ParticleFlowEventFilter_r207() {}

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

};
#endif
