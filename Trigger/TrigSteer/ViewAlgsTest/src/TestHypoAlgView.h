// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TestHypoAlgView_h
#define ViewAlgs_TestHypoAlgView_h

#include <string>
#include "AthViews/AthViewAlgorithm.h"
//#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "AthViews/View.h"
#include "./TestHypoTool.h"

namespace AthViews {

class TestHypoAlgView : public ::AthViewAlgorithm {
public:
  TestHypoAlgView(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

private:
  ToolHandleArray<TestHypoTool> m_hypos;
  
  SG::ReadHandle< xAOD::TrigCompositeContainer >     m_inputProxyContainer;
  SG::WriteHandle< xAOD::TrigCompositeContainer >    m_output;
  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_outputAux;
};

}

#endif
