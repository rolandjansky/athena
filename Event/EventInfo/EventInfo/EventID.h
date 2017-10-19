/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFO_EVENTID_H
# define EVENTINFO_EVENTID_H

/**
 * @file EventInfo/EventID.h
 *
 * @brief This class provides a unique identification for each event,
 * in terms of run/event number and/or a time stamp.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Paolo Calafiura <pcalafiura@lbl.gov>
 *
 * $Id: EventID.h,v 1.10 2007-12-04 18:22:32 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include <iostream>
#include <stdint.h>

#include "GaudiKernel/EventIDBase.h"

/**
 * @class EventID
 *
 * @brief This class provides a unique identification for each event,
 * in terms of run/event number and/or a time stamp.
 *
 */

class EventID : public EventIDBase {
public:

  typedef EventIDBase::number_type       number_type;
  
    /// \name structors
    //@{
    EventID();
    EventID(number_type run_number, 
            uint64_t event_number);
    EventID(number_type run_number, 
            uint64_t event_number,
            number_type time_stamp);
    EventID(number_type run_number, 
            uint64_t event_number,
            number_type time_stamp,
            number_type time_stamp_ns_offset,
            number_type lumi_block,
            number_type bunch_crossing_id);
    EventID(number_type run_number, 
            uint64_t event_number,
            number_type time_stamp,
            number_type time_stamp_ns_offset,
            number_type lumi_block,
            number_type bunch_crossing_id,
            number_type detector_mask0,
            number_type detector_mask1,
            number_type detector_mask2 = 0,
            number_type detector_mask3 = 0);
    // Use default copy constructor.
    virtual ~EventID();
    //@}
  
    /// detector mask0 - bit field indicating which TTC zones have been built into the event, one
    /// bit per zone, 32 bit unsigned
    number_type   detector_mask0       (void) const;

    /// detector mask1 - bit field indicating which TTC zones have been built into the event, one
    /// bit per zone, 32 bit unsigned
    number_type   detector_mask1       (void) const;

    /// detector mask0 - bit field indicating which TTC zones have been built into the event, one
    /// bit per zone, 32 bit unsigned
    number_type   detector_mask2       (void) const;

    /// detector mask1 - bit field indicating which TTC zones have been built into the event, one
    /// bit per zone, 32 bit unsigned
    number_type   detector_mask3       (void) const;

    /// detector mask - as a 64 bit number - only provides access to the combined 64 bits det mask,
    /// the newer 64 bits, adding up to the 128 bits, must be accessed directly by mask2,3
    uint64_t      detector_mask        (void) const;


    /// set detector mask
    void   set_detector_mask           (number_type detectorMask0, 
                                        number_type detectorMask1,
                                        number_type detectorMask2 = 0,
                                        number_type detectorMask3 = 0);

    /// Extraction operators
    friend std::ostream& operator<<(std::ostream& os, const EventID& rhs);

private:

    /// detector mask0 - bit field indicating which TTC zones have been
    /// built into the event, one bit per zone, 32 bit unsigned
  number_type   m_detector_mask0 {0};

    /// detector mask1 - bit field indicating which TTC zones have been
    /// built into the event, one bit per zone, 32 bit unsigned
  number_type   m_detector_mask1 {0};

    /// detector mask2 - additional trigger bits
  number_type   m_detector_mask2 {0};

    /// detector mask3 - additional trigger bits
  number_type   m_detector_mask3 {0};
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>


inline 
EventID::number_type
EventID::detector_mask0       (void) const
{
    return m_detector_mask0;
}

inline 
EventID::number_type
EventID::detector_mask1       (void) const
{
    return m_detector_mask1;
}

inline 
EventID::number_type
EventID::detector_mask2       (void) const
{
    return m_detector_mask2;
}

inline 
EventID::number_type
EventID::detector_mask3       (void) const
{
    return m_detector_mask3;
}

inline
uint64_t
EventID::detector_mask        (void) const
{
    uint64_t result = m_detector_mask1;
    result = result << 32;
    result |= m_detector_mask0;
    return (result);
}

inline std::ostream& operator<<(std::ostream& os, const EventID& rhs) {
  os << "[Run,Evt,Lumi,Time,BunchCross,DetMask] = [" 
     << rhs.run_number() 
     << "," << rhs.event_number()
     << "," << rhs.lumi_block()
     << "," << rhs.time_stamp()
     << ":" << rhs.time_stamp_ns_offset()
     << "," << rhs.bunch_crossing_id()
     << ",0x" << std::hex << rhs.detector_mask() 
     << ",0x" << std::hex << rhs.detector_mask2()
     << ",0x" << std::hex << rhs.detector_mask3() << std::dec
     << "]";
  return os;
}


//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // EVENTINFO_EVENTID_H






