// -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEINTERFACE_IEVENTFEATUREALGORITHM_H
#define EVENTSHAPEINTERFACE_IEVENTFEATUREALGORITHM_H

//#include "GaudiKernel/IAlgorithm.h"

#include "NavFourMom/INavigable4MomentumCollection.h"
#include "EventShapeEvent/EventFeatureStore.h"

#include <string> 

class IEventFeatureAlgorithm //: virtual public IAlgorithm
{
public:

  virtual ~IEventFeatureAlgorithm() { }

  virtual const std::string& getFeatureStoreKey() const = 0; 

protected:

  virtual StatusCode appInitialize() = 0;
  virtual StatusCode appExecute(EventFeatureStore* pStore,
				const INavigable4MomentumCollection* pColl) = 0;
  virtual StatusCode appExecute(EventFeatureStore* pStore) = 0;
  virtual StatusCode appFinalize() = 0;

  virtual void executeMonitor(const std::string& toolName,
			      const std::string& toolType,
			      StatusCode         checkOut) = 0;

  virtual void executeSummary(bool cleanUp=false) = 0;

};
#endif
