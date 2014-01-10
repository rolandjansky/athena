/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_EVENTINFONTCOMPONENT_H
#define TRIGMINBIASNTUPLE_EVENTINFONTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>
#include <string>

namespace NtComponent {
  class EventInfoNtComponent: public NtupleComponent {
  public:
    EventInfoNtComponent(NtupleAlgorithm *mainAlg,
			 NtComponentParameters parameters);
    virtual ~EventInfoNtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:

    std::string m_eventInfoKey;

    // ntuple variables

    /** Event ID */
    unsigned int m_runNumber;
    unsigned int m_eventNumber;
    unsigned int m_time_stamp;
    unsigned int m_time_stamp_ns_offset; 
    unsigned int m_lumiBlock;
    unsigned int m_bunch_crossing_id;
    unsigned int m_detector_mask0;
    unsigned int m_detector_mask1;

    /** Trigger Info */
    bool m_trigger_info;
    unsigned int m_statusElement;
    unsigned int m_extendedLevel1ID;
    unsigned int m_level1TriggerType;

    // Vector sizes needed for Root analysis
    unsigned int m_nlevel1TriggerInfo;
    unsigned int m_nlevel2TriggerInfo;
    unsigned int m_neventFilterInfo;
    std::vector<unsigned int> *m_level1TriggerInfo;
    std::vector<unsigned int> *m_level2TriggerInfo;
    std::vector<unsigned int> *m_eventFilterInfo;
  };
}

#endif
