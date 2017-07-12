// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TestViewDriver_h
#define ViewAlgs_TestViewDriver_h

#include <vector>
#include <string>
#include "GaudiKernel/IAlgResourcePool.h"
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
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roisContainer;

  SG::WriteHandleKey< ConstDataVector<TrigRoiDescriptorCollection> > m_roisViewOutput;
  // this is problematic and we need to use plain string for the moment
  SG::ReadHandleKey< TestClusterContainer > m_clustersViewInputHandle;
  std::string m_clustersViewInput;

  SG::WriteHandleKey< std::vector<SG::View*> > m_views;
  
  ServiceHandle<IAlgResourcePool> m_viewAlgorithmsPool;
  std::vector<std::string> m_viewAlgorithmNames;
  int m_roITypeInViews;
  
};

#endif

