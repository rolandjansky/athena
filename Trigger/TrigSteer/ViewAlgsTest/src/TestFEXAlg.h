/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// -*- c++ -*-
#ifndef ViewAlgs_TestFexAlg_h
#define ViewAlgs_TestFexAlg_h

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

//#include "AthViews/View.h"

#include "./TestEDM.h"

class TestFEXAlg : public AthAlgorithm {
public:
  TestFEXAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

private:
  SG::ReadHandle< xAOD::TrigCompositeContainer > m_roisContainer;
  SG::WriteHandle< TestClusterContainer > m_outputClusterContainer;
  SG::WriteHandle< TestClusterAuxContainer > m_outputClusterContainerAux;
  SG::WriteHandle< xAOD::TrigCompositeContainer > m_outputProxyContainer;
  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_outputProxyContainerAux;

};



#endif
