/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_LVL1CONFIGNTCOMPONENT_H
#define TRIGGERMENUANALYSIS_LVL1CONFIGNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"

#include "GaudiKernel/ServiceHandle.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

#include <vector>
#include <string>

namespace NtComponent {
  class Lvl1ConfigNtComponent: public NtupleComponent {
  public:
    Lvl1ConfigNtComponent(NtupleAlgorithm *mainAlg,
			  NtComponentParameters parameters);
    virtual ~Lvl1ConfigNtComponent();
    
    StatusCode setupServices(void);
    StatusCode book(void);
    StatusCode fill(void);

  private:

    ServiceHandle<TrigConf::ILVL1ConfigSvc>* m_configSvc;

    std::string m_eventInfoKey;
    // NTUPLE variables 
    uint32_t m_npit; 
    std::vector<std::string>* m_pit_name;
    std::vector<uint16_t>* m_pit_number;
    std::vector<std::string>* m_item_name;
    std::vector<int>* m_item_ctpId;

    int m_runNumber;
  };
}

#endif
