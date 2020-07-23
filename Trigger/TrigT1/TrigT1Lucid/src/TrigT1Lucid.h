/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_T1_LUCID_H
#define TRIG_T1_LUCID_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

#include "LUCID_RawEvent/LUCID_DigitContainer.h"
#include <string>

namespace LVL1 {

  /* @class TrigT1Lucid
 
  @author Jacob Groth-Jensen <jacob.groth-jensen@hep.lu.se>
  
  An algorithm to simulate the level 1 LUCID trigger.  This algorithm
  records a data object of LucidCTP type into StoreGate.  The object
  contains the input bits for the CTP simulation. */ 
  
  class TrigT1Lucid: public AthAlgorithm {
  
  public:

    TrigT1Lucid(const std::string& name, ISvcLocator* pSvcLocator);

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    
  private:

    /** A data member to retain a connection to the level 1
	configuration service */
    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;

    /** A data member to contain the name of the LUCID digit container
	that contains the input LUCID information.  This data member is
	used as a job option property
    */
    std::string m_LUCID_LVL1T1ContainerName;

    /** A data member to contain a pointer to the LUCID_DigitContainer
	that contains the input LUCID information. */
    const LUCID_DigitContainer* m_digitContainer;
    
    /** thresholds for the level 1 LUCID trigger */
    float          m_threshold_a; 
    float          m_threshold_c; 
    int            m_cablestart_a;
    int            m_cablestart_c;
    unsigned short m_qdcThreshold;

    /** A flag to prevent the trigger simulation from running over bad
	input data.  */
    bool           m_badDataFound;

  };
}

#endif
