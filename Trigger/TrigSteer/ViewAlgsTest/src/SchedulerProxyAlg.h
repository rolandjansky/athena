// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_SchedulerProxyAlg_h
#define ViewAlgs_SchedulerProxyAlg_h

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthContainers/ConstDataVector.h"

#include "./TestEDM.h"

class SchedulerProxyAlg : public AthAlgorithm {
public:
  SchedulerProxyAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

private:
  SG::ReadHandleKey<ConstDataVector<TrigRoiDescriptorCollection> > m_roisContainer;
  SG::WriteHandleKey< TestClusterContainer > m_outputClusterContainer;
  SG::WriteHandleKey< TestClusterAuxContainer > m_outputClusterContainerAux;

};



#endif
