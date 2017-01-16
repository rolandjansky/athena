// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_SchedulerProxyAlg_h
#define ViewAlgs_SchedulerProxyAlg_h

#include <string>
#include "AthViews/AthViewAlgorithm.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthContainers/ConstDataVector.h"

#include "./TestEDM.h"

class SchedulerProxyAlg : public AthViewAlgorithm {
public:
  SchedulerProxyAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

private:
  SG::ReadHandle<ConstDataVector<TrigRoiDescriptorCollection> > m_roisContainer;
  //SG::ReadHandle<TrigRoiDescriptorCollection> m_roisContainer;
  SG::WriteHandle< TestClusterContainer > m_outputClusterContainer;
  SG::WriteHandle< TestClusterAuxContainer > m_outputClusterContainerAux;

};



#endif
