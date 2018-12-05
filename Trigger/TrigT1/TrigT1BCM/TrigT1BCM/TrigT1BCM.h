/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_T1_BCM_H
#define TRIG_T1_BCM_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"

#include <string>
#include <vector>

namespace LVL1 {
  /** @class TrigT1BCM

  @author Bilge M. Demirkoz <demirkoz@cern.ch>

  An algorithm to simulate the level 1 BCM trigger.  This algorithm
  records a data object of BcmCTP type into StoreGate.  The object
  contains the input bits for the CTP simulation.

  */  
  class TrigT1BCM : public AthAlgorithm {
  public:
    TrigT1BCM(const std::string& name, ISvcLocator* pSvcLocator);

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    
  private:
    /** A data member to retain a connection to the level 1
	configuration service */
    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;

    /** A data member to contain the name of the BcmL1 RDO
	that contains the input BCM information.  This data member is
	used as a job option property.
    */
    std::string m_bcmL1ContainerName;
    
    /** A data member to contain a pointer to the BcmL1 RDO
	that contains the input BCM information. */ 
    const BCM_RDO_Container* m_bcmRDO;
    
    bool m_6bit_flag;
    int m_TimeWindow;
    int m_WideTimeWindow;
    int m_OOTPulseX;
    int m_ITPulseX;
    int m_WITPulseX;
    int m_cablestarts[10]; // threshold cable starts  
    bool m_badDataFound;

  };
}

#endif
