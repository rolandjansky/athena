/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_BASEALG_H
#define ATHEXHIVE_BASEALG_H 1


#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandle.h"
#include "AthExHive/HiveDataObj.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthExHive/IHiveExSvc.h"
#include "rGen.h"

#include <string>

class HiveAlgBase  :  public AthAlgorithm, public rGen {
  
public:
  
 HiveAlgBase (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgBase();

  bool isClonable() const override { return true; }
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  // StatusCode execute();
  // StatusCode finalize();

protected:

  int sleep_for(int);
  int sleep();

  ServiceHandle<IHiveExSvc> m_hes;
  
private:

  Gaudi::Property<int> m_time{this, "Time", 0, "default alg sleep time"};
  

};
#endif
