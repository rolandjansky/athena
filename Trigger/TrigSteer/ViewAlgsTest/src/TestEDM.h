// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TestEDM_h
#define ViewAlgs_TestEDM_h


#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"


typedef xAOD::TrigComposite TestCluster;
typedef xAOD::TrigCompositeContainer TestClusterContainer;
typedef xAOD::TrigCompositeAuxContainer TestClusterAuxContainer;
namespace TestEDM {
  void setClusterEt(TestCluster*, float et);
  float getClusterEt(const TestCluster*);
  
  void setClusterEta(TestCluster*, float et);
  float getClusterEta(const TestCluster*);
  
  void setClusterPhi(TestCluster*, float et);
  float getClusterPhi(const TestCluster*);
}


#endif


