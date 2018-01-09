// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TestViewDriver_h
#define ViewAlgs_TestViewDriver_h

#include <vector>
#include <string>
#include "GaudiKernel/IScheduler.h"
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
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roisContainer { this, "RoIsContainer", "RoIsContainer", "Input RoIs" };

  SG::WriteHandleKey< ConstDataVector<TrigRoiDescriptorCollection> > m_roisViewOutput { this, "RoIsViewOutput", "RoIsViewOutput", "Name of the collection that will be placed in each view" };
  // this is problematic and we need to use plain string for the moment
  SG::ReadHandleKey< TestClusterContainer > m_clustersViewInputHandle;
  Gaudi::Property< std::string > m_clustersViewInput { this, "ClustersViewInput", "ClustersViewInput", "" };
  Gaudi::Property< std::string > m_viewNodeName { this, "ViewNodeName", "", "Name of CF node to attach views to" };

  SG::WriteHandleKey< std::vector< SG::View* > > m_views { this, "Views", "Views", "Name of the generated view collection" };
  
  ServiceHandle< IScheduler > m_scheduler { this, "Scheduler", "AvalancheSchedulerSvc", "The Athena scheduler" };
  Gaudi::Property< int > m_roITypeInViews { this, "RoITypeInViews", 1, "0 - place TrigRoiDesciptor in views, 1 - place Collections wiht single RoI, 2 - place entrie collection in the view, 3 - place SuperRoI in single view " };

};

#endif

