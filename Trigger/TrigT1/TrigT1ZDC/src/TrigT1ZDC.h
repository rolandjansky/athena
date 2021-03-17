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
    
    // Property to enable inclusion of LHCf
    Gaudi::Property<bool> m_zdcIncludeLHCf{this, "ZDCIncludeLHCf", true};
    
    // Property to choose between old and new config
    Gaudi::Property<bool> m_useNewConfig{this, "UseNewConfig",false, "When true, read the menu from detector store, when false use the L1ConfigSvc"};
    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc{this, "LVL1ConfigSvc", "LVL1ConfigSvc", "The LVL1ConfigSvc providing L1 configuration for Run 2"};
    
    // Property ZDC container name, that contains the input ZDC information.
    Gaudi::Property<std::string> m_zdcContainerName{this, "ZdcContainerName", "ZDC"};

    /** The thresholds for the level 1 ZDC trigger. */
    float m_threshold_c{-1}; // backward threshold
    float m_threshold_a{-1}; // forward threshold 
    
    
    /** CTP trigger bit locators **/
    int m_cablestart_a{-1}; 
    int m_cablestart_c{-1}; 
    int m_cablestart_ac{-1}; //Legacy coincidence bit

    /** Internal counter to prit out statistics at finalise */
    int m_passedA {0};
    int m_passedC {0};
    int m_passedAC{0};
    
    // Property McEventCollection key.  Needed for truth based trigger simulation.
    Gaudi::Property<std::string> m_mcEventCollectionKey{this, "McEventCollectionKey", "TruthEvent"};
  };
}

#endif
