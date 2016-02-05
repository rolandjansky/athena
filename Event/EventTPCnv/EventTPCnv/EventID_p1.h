/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_EVENTID_P1_H
#define EVENTTPCNV_EVENTID_P1_H

/**
 * @file EventID_p1.h
 *
 * @brief This class provides persistent representation for EventID
 * @author Marcin.Nowak@cern.ch
 */

#include <stdint.h>

class EventID_p1 {
public:

    typedef unsigned int       number_type;
  
    EventID_p1();

    /// run number
    number_type   m_run_number;
    /// event number
    uint64_t      m_event_number;
    /// posix time in seconds since 1970/01/01
    number_type   m_time_stamp;
    /// time stamp ns - ns time offset for time_stamp, 32 bit unsigned
    number_type   m_time_stamp_ns_offset; 
    /// luminosity block number: 
    /// the number which uniquely tags a luminosity block within a run
    number_type   m_lumiBlock;
    /// bunch crossing ID,  32 bit unsigned
    number_type   m_bunch_crossing_id;
    /// detector mask0 - bit field indicating which TTC zones have been
    /// built into the event, one bit per zone, 32 bit unsigned
    number_type   m_detector_mask0;
    /// detector mask1 - bit field indicating which TTC zones have been
    /// built into the event, one bit per zone, 32 bit unsigned
    number_type   m_detector_mask1;
    /// detector mask0 - bit field indicating which TTC zones have been built into the event, one
    /// bit per zone, 32 bit unsigned
    number_type   m_detector_mask2;

    /// detector mask1 - bit field indicating which TTC zones have been built into the event, one
    /// bit per zone, 32 bit unsigned
    number_type   m_detector_mask3;
};

inline EventID_p1::EventID_p1()
        :
        m_run_number(0),
        m_event_number(0),
        m_time_stamp(0),
        m_time_stamp_ns_offset(0), 
        m_lumiBlock(0),
        m_bunch_crossing_id(0),
        m_detector_mask0(0),
        m_detector_mask1(0),
        m_detector_mask2(0),
        m_detector_mask3(0)
{} 


#endif 






