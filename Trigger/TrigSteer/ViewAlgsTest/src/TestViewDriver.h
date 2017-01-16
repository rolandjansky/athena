// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TestViewDriver_h
#define ViewAlgs_TestViewDriver_h

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthContainers/ConstDataVector.h"
//#include "xAODTrigger/TrigCompositeContainer.h"
//#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "AthViews/View.h"

#include "./TestEDM.h"

class TestViewDriver : public AthAlgorithm {
public:
  TestViewDriver(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

private:
  SG::ReadHandle< TrigRoiDescriptorCollection > m_roisContainer;
  SG::WriteHandle< std::vector<SG::View*> > m_views;

  SG::WriteHandle< TestClusterContainer > m_outputClusterContainer;
  SG::WriteHandle< TestClusterAuxContainer > m_outputClusterContainerAux;
  //  SG::WriteHandle< xAOD::TrigCompositeContainer > m_outputProxyContainer;
  //  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_outputProxyContainerAux;
};

#endif

