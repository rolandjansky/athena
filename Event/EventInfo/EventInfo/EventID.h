/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFO_EVENTID_H
# define EVENTINFO_EVENTID_H

/**
 * @file EventID.h
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


/**
 * @class EventID
 *
 * @brief This class provides a unique identification for each event,
 * in terms of run/event number and/or a time stamp.
 *
 */

class EventID {
public:

    typedef unsigned int       number_type;
  
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
    explicit EventID(const EventID& id);
    virtual ~EventID();
    //@}
  
    /// run number - 32 bit unsigned
    number_type   run_number           (void) const;

    /// event number - 64 bit unsigned
    uint64_t      event_number         (void) const;

    /// time stamp - posix time in seconds from 1970, 32 bit unsigned
    number_type   time_stamp           (void) const; 

    /// time stamp ns - ns time offset for time_stamp, 32 bit unsigned
    number_type   time_stamp_ns_offset (void) const; 

    /// luminosity block identifier, 32 bit unsigned
    number_type   lumi_block           (void) const;

    /// bunch crossing ID,  32 bit unsigned
    number_type   bunch_crossing_id    (void) const;

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

    /// set run number
    void   set_run_number              (number_type runNumber);

    /// set event number
    void   set_event_number            (uint64_t eventNumber);

    /// set time stamp
    void   set_time_stamp              (number_type timeStamp); 

    /// set time stamp in ns
    void   set_time_stamp_ns_offset    (number_type timeStampNs); 

    /// set luminosity block identifier
    void   set_lumi_block              (number_type lumiBlock);

    /// set bunch crossing ID
    void   set_bunch_crossing_id       (number_type bcid);

    /// set detector mask
    void   set_detector_mask           (number_type detectorMask0, 
                                        number_type detectorMask1,
                                        number_type detectorMask2 = 0,
                                        number_type detectorMask3 = 0);

    /// Comparison operators
    friend bool operator<(const EventID& lhs, const EventID& rhs);
    friend bool operator>(const EventID& lhs, const EventID& rhs);
    friend bool operator==(const EventID& lhs, const EventID& rhs);
    friend bool operator!=(const EventID& lhs, const EventID& rhs);
    friend bool operator<=(const EventID& lhs, const EventID& rhs);
    friend bool operator>=(const EventID& lhs, const EventID& rhs);

    /// Extraction operators
    friend std::ostream& operator<<(std::ostream& os, const EventID& rhs);

private:

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

    /// detector mask2 - additional trigger bits
    number_type   m_detector_mask2;

    /// detector mask3 - additional trigger bits
    number_type   m_detector_mask3;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>

inline 
EventID::number_type 	
EventID::run_number	(void) const
{
    return m_run_number;
}

inline 
uint64_t
EventID::event_number	(void) const
{
    return m_event_number;
}

inline 
EventID::number_type 	
EventID::time_stamp	(void) const
{
    return m_time_stamp;
}

inline 
EventID::number_type 	
EventID::time_stamp_ns_offset (void) const
{
    return m_time_stamp_ns_offset;
}

inline 
EventID::number_type 	
EventID::lumi_block	(void) const
{
    return m_lumiBlock;
}

inline 
EventID::number_type
EventID::bunch_crossing_id    (void) const
{
    return m_bunch_crossing_id;
}

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

inline bool operator<(const EventID& lhs, const EventID& rhs) {
  // We are assuming that ALL events will have run and event numbers,
  // and never just a time stamp.
  // FIXME: any use for also ordering by lumi-block ?
  return lhs.m_run_number<rhs.m_run_number ||
         ( lhs.m_run_number==rhs.m_run_number && 
           lhs.m_event_number<rhs.m_event_number) ;
}

inline bool operator==(const EventID& lhs, const EventID& rhs) {
    // We assume that equality via run/event numbers is sufficient
  return lhs.m_run_number   == rhs.m_run_number && 
         lhs.m_event_number == rhs.m_event_number;
}
inline bool operator>(const EventID& lhs, const EventID& rhs) {
  return !( (lhs < rhs) || (lhs == rhs));
}
inline bool operator!=(const EventID& lhs, const EventID& rhs) {
  return !(lhs == rhs);
}
inline bool operator<=(const EventID& lhs, const EventID& rhs) {
  return !(lhs > rhs);
}
inline bool operator>=(const EventID& lhs, const EventID& rhs) {
  return !(lhs < rhs);
}

inline std::ostream& operator<<(std::ostream& os, const EventID& rhs) {
  os << "[Run,Evt,Lumi,Time,BunchCross,DetMask] = [" 
     << rhs.m_run_number 
     << "," << rhs.m_event_number 
     << "," << rhs.m_lumiBlock
     << "," << rhs.m_time_stamp 
     << ":" << rhs.m_time_stamp_ns_offset
     << "," << rhs.m_bunch_crossing_id
     << ",0x" << std::hex << rhs.detector_mask() 
     << ",0x" << std::hex << rhs.detector_mask2()
     << ",0x" << std::hex << rhs.detector_mask3() << std::dec
     << "]";
  return os;
}


//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // EVENTINFO_EVENTID_H






