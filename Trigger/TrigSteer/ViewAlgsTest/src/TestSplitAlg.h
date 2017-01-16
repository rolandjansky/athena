// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TestSplitAlg_h
#define ViewAlgs_TestSplitAlg_h

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"


//#include "AthViews/View.h"

#include "./TestEDM.h"

class TestSplitAlg : public AthAlgorithm {
public:
  TestSplitAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

private:
  SG::ReadHandle< xAOD::TrigCompositeContainer > m_inputProxyContainer;
  SG::ReadHandle< xAOD::TrigCompositeContainer > m_referenceProxyContainer;

  SG::WriteHandle< xAOD::TrigCompositeContainer > m_outputProxyContainer;
  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_outputProxyAuxContainer;

  // std::vector< SG::WriteHandle< xAOD::TrigCompositeContainer > > m_outputVectorProxyColl;
  // std::vector< SG::WriteHandle< xAOD::TrigCompositeAuxContainer > > m_outputVectorProxyAuxColl;
  // std::string m_outputCollNames;

};



#endif
