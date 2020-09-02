/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_T1_ZDC_H
#define TRIG_T1_ZDC_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include <string>



namespace LVL1 {
  /** @class TrigT1ZDC

  @author William H. Bell <W.Bell@cern.ch>
  @author Bilge Demirkoz <demirkoz@cern.ch>

  An algorithm to simulate the level 1 ZDC trigger.  This algorithm
  records a data object of ZdcCTP type into StoreGate.  The object
  contains the input bits for the CTP simulation.

  */  
  class TrigT1ZDC : public AthAlgorithm {
  public:
    TrigT1ZDC(const std::string& name, ISvcLocator* pSvcLocator);

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    
  private:
    
    /** flags **/
    bool m_zdcIncludeLHCf;
    
    /** A data member to retain a connection to the level 1
	configuration service */
    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
    
    /** A data member to contain the name of the ZDC container
	that contains the input ZDC information.  This data member is
	used as a job option property
    */
    std::string m_zdcContainerName;

    /** A data member to contain a pointer to the container
	that contains the input ZDC information. */
    //const ZdcContainer* m_zdcContainer;

    /** The thresholds for the level 1 ZDC trigger. */
    float m_threshold_c; // backward threshold
    float m_threshold_a; // forward threshold 
    
    
    /** CTP trigger bit locators **/
    int m_cablestart_a; 
    int m_cablestart_c; 
    int m_cablestart_ac; //Legacy coincidence bit

    /** Internal counter to prit out statistics at finalise */
    int m_passedA, m_passedC, m_passedAC;
    
    /** A flag to prevent the trigger simulation from running over bad
	input data.  */
    //bool m_badDataFound;
    
    /** McEventCollection key.  Needed for truth based trigger
	simulation. */
    std::string m_mcEventCollectionKey;
  };
}

#endif
