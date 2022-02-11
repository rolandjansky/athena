/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  HiveAlgBase.h
 * @brief Base class for AthExHive example Algs to provide functionality
 * to sleep for a certain amount of time, which is set via a common 
 * Property.
 * Alg can be configured to use CPU via CPUCrunchSvc  instead of just 
 * sleeping if "Crunch" Property is set True
 */


#ifndef ATHEXHIVE_BASEALG_H
#define ATHEXHIVE_BASEALG_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ICPUCrunchSvc.h"
#include "AthExHive/IHiveExSvc.h"

#include <string>

class HiveAlgBase  :  public AthAlgorithm {
  
public:
  
  HiveAlgBase (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HiveAlgBase();

  // make Algs clonable
  virtual bool isClonable() const override { return true; }

  virtual  StatusCode initialize() override;

protected:

  // cause Alg to sleep for time defined by "Time" Property
  unsigned int sleep();

  // Handle to HiveExSvc, which accumulates how much time each Alg sleeps
  ServiceHandle<IHiveExSvc> m_hes;

  // Handle to CPUCrunchSvc, which burns CPU time
  ServiceHandle<ICPUCrunchSvc> m_ccs;

  // Handle to random number service
  ServiceHandle<IAthRNGSvc> m_rngSvc;

private:

  Gaudi::Property<unsigned int> m_time{this, "Time", 0, "default alg sleep time in ms"};
  Gaudi::Property<bool> m_doCrunch{this, "Crunch", false, "Crunch instead of Sleep"};
  

};
#endif
