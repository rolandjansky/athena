// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TestFexAlgView_h
#define ViewAlgs_TestFexAlgView_h

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "AthViews/View.h"

#include "./TestEDM.h"

namespace AthViews {
class TestFEXAlgView : public ::AthAlgorithm {

public:
  TestFEXAlgView(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

private:

  TestFEXAlgView(); //default constructor

  SG::ReadHandle< xAOD::TrigCompositeContainer > m_inputContainer;
  SG::WriteHandle< TestClusterContainer > m_outputClusterContainer;
  SG::WriteHandle< TestClusterAuxContainer > m_outputClusterContainerAux;
  SG::WriteHandle< xAOD::TrigCompositeContainer > m_outputProxyContainer;
  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_outputProxyContainerAux;
  

};

}

#endif
