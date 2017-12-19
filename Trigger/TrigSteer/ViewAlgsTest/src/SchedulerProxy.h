// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_SchedulerProxy_h
#define ViewAlgs_SchedulerProxy_h

#include <iostream>

#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthContainers/ConstDataVector.h"
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/EventContext.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "./TestEDM.h"

namespace SchedulerProxy {
  
  class Scheduler {
  public:
    Scheduler(){}
    StatusCode run(const std::vector<EventContext>& subEvents) {
      // I know this is ugly. It is just checking if handles etc pass the data around
      for ( auto context: subEvents )  {

	{
	  SG::ReadHandle<ConstDataVector<TrigRoiDescriptorCollection> > rois("RegionOfReco");
	  CHECK(rois.setProxyDict(context.getExtension<Atlas::ExtendedEventContext>()->proxy()));
	  
	  SG::WriteHandle< TestClusterContainer > clusterContainer("Clusters");
	  CHECK(clusterContainer.setProxyDict(context.getExtension<Atlas::ExtendedEventContext>()->proxy()));
	  ATH_CHECK (clusterContainer.record (CxxUtils::make_unique< TestClusterContainer >()) );
	  
	  SG::WriteHandle< TestClusterAuxContainer > clusterContainerAux("ClustersAux.");
	  CHECK(clusterContainerAux.setProxyDict(context.getExtension<Atlas::ExtendedEventContext>()->proxy()));
	  ATH_CHECK( clusterContainerAux.record (CxxUtils::make_unique< TestClusterAuxContainer>()) );
	  clusterContainer->setStore(clusterContainerAux.ptr());

	  
	  std::cout << "Scheduler     INFO Launching processing for View with RoIs\n";
	  for ( auto roi: *rois.cptr() ) {
	    std::cout << " ... " << *roi << std::endl;
	    TestCluster * cl = new TestCluster();
	    clusterContainer->push_back(cl);
	    TestEDM::setClusterEt (cl, roi->eta()*10.); // whatever values
	    TestEDM::setClusterEta(cl, roi->eta()+0.01);
	    TestEDM::setClusterPhi(cl, roi->phi()-0.05);      
	  }    
	}
      }
      std::cout << "Scheduler     INFO  finished processing views " << std::endl;
      return StatusCode::SUCCESS;
    }    
  };
}



#endif
