// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TestViewMerger_h
#define ViewAlgs_TestViewMerger_h

#include <vector>
#include <string>
//#include "GaudiKernel/IAlgResourcePool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
//#include "AthenaBaseComps/AthReentrantAlgorithm.h"
//#include "AthContainers/ConstDataVector.h"
//#include "xAODTrigger/TrigCompositeContainer.h"
//#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "AthViews/View.h"

#include "./TestEDM.h"

//class TestViewMerger : public AthReentrantAlgorithm {
class TestViewMerger : public AthAlgorithm {
public:
  TestViewMerger( const std::string& name, ISvcLocator* pSvcLocator );
  StatusCode initialize();
  StatusCode execute();
  //  StatusCode execute_r( const EventContext& ctx ) const;

private:
  SG::ReadHandleKey< std::vector<SG::View*> > m_viewsKey; // will be used in evt context
  SG::ReadHandleKey< TestClusterContainer > m_clustersViewInputKey; // vill be used in each view, needs to be renounced
  SG::WriteHandleKey< TestClusterContainer > m_clustersOutputKey;
};

#endif

