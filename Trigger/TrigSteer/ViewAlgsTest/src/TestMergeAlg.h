// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TestMergeAlg_h
#define ViewAlgs_TestMergeAlg_h

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadHandleKeyArray.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

//#include "AthViews/View.h"

#include "./TestEDM.h"

class TestMergeAlg : public AthAlgorithm {
public:
  TestMergeAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();


private:

  //  std::string m_inputCollNames;
  SG::ReadHandleKeyArray<xAOD::TrigCompositeContainer> m_inputVectorProxyColl;

  SG::WriteHandle< TrigRoiDescriptorCollection > m_outputRoisContainer;
  SG::WriteHandle< xAOD::TrigCompositeContainer > m_outputProxyContainer;
  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_outputProxyAuxContainer;

  float m_distance;
  //  bool  m_createMergedRoI;
  StatusCode validCollections(const std::vector<SG::ReadHandle<xAOD::TrigCompositeContainer>>&) const;
};



#endif
