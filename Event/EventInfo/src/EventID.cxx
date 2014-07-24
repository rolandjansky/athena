/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventID.cxx
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Paolo Calafiura <pcalafiura@lbl.gov>
 *
 * $Id: EventID.cxx,v 1.8 2007-12-04 12:37:23 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "EventInfo/EventID.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

EventID::EventID() :
    m_run_number          (0),
    m_event_number        (0),
    m_time_stamp          (0),
    m_time_stamp_ns_offset(0),
    m_lumiBlock           (0),
    m_bunch_crossing_id   (0),
    m_detector_mask0      (0),
    m_detector_mask1      (0),
    m_detector_mask2      (0),
    m_detector_mask3      (0)

{}

EventID::EventID(number_type run_number, 
		 uint64_t event_number)
    :
    m_run_number          (run_number),
    m_event_number        (event_number),
    m_time_stamp          (0),
    m_time_stamp_ns_offset(0),
    m_lumiBlock           (0),
    m_bunch_crossing_id   (0),
    m_detector_mask0      (0),
    m_detector_mask1      (0),
    m_detector_mask2      (0),
    m_detector_mask3      (0)

{}

EventID::EventID(number_type run_number, 
		 uint64_t event_number,
		 number_type time_stamp)
    :
    m_run_number          (run_number),
    m_event_number        (event_number),
    m_time_stamp          (time_stamp),
    m_time_stamp_ns_offset(0),
    m_lumiBlock           (0),
    m_bunch_crossing_id   (0),
    m_detector_mask0      (0),
    m_detector_mask1      (0),
    m_detector_mask2      (0),
    m_detector_mask3      (0)

{}

EventID::EventID(number_type run_number, 
		 uint64_t event_number,
		 number_type time_stamp,
                 number_type time_stamp_ns_offset,
                 number_type lumi_block,
                 number_type bunch_crossing_id)
    :
    m_run_number           (run_number),
    m_event_number         (event_number),
    m_time_stamp           (time_stamp),
    m_time_stamp_ns_offset (time_stamp_ns_offset),
    m_lumiBlock            (lumi_block),
    m_bunch_crossing_id    (bunch_crossing_id),
    m_detector_mask0       (0),
    m_detector_mask1       (0),
    m_detector_mask2       (0),
    m_detector_mask3       (0)

{}


EventID::EventID(number_type run_number, 
                 uint64_t event_number,
                 number_type time_stamp,
                 number_type time_stamp_ns_offset,
                 number_type lumi_block,
                 number_type bunch_crossing_id,
                 number_type detector_mask0,
                 number_type detector_mask1,
                 number_type detector_mask2,
                 number_type detector_mask3) 
    :
    m_run_number           (run_number),
    m_event_number         (event_number),
    m_time_stamp           (time_stamp),
    m_time_stamp_ns_offset (time_stamp_ns_offset),
    m_lumiBlock            (lumi_block),
    m_bunch_crossing_id    (bunch_crossing_id),
    m_detector_mask0       (detector_mask0),
    m_detector_mask1       (detector_mask1),
    m_detector_mask2       (detector_mask2),
    m_detector_mask3       (detector_mask3)
{}


EventID::EventID(const EventID& id) :
    m_run_number           (id.m_run_number),
    m_event_number         (id.m_event_number),
    m_time_stamp           (id.m_time_stamp),
    m_time_stamp_ns_offset (id.m_time_stamp_ns_offset),
    m_lumiBlock            (id.m_lumiBlock),
    m_bunch_crossing_id    (id.m_bunch_crossing_id),
    m_detector_mask0       (id.m_detector_mask0),
    m_detector_mask1       (id.m_detector_mask1),
    m_detector_mask2       (id.m_detector_mask2),
    m_detector_mask3       (id.m_detector_mask3)
{}

EventID::~EventID()
{}


void   
EventID::set_run_number              (number_type runNumber)
{
    m_run_number = runNumber;
}

void
EventID::set_event_number            (uint64_t eventNumber)
{
    m_event_number = eventNumber;
}

void
EventID::set_time_stamp              (number_type timeStamp)
{
    m_time_stamp = timeStamp;
}

void
EventID::set_time_stamp_ns_offset    (number_type timeStampNs)
{
    m_time_stamp_ns_offset = timeStampNs;
}

void
EventID::set_lumi_block              (number_type lumiBlock)
{
    m_lumiBlock = lumiBlock;
}


void
EventID::set_bunch_crossing_id       (number_type bcid)
{
    m_bunch_crossing_id = bcid;
}

void   
EventID::set_detector_mask           (number_type detectorMask0, 
                                      number_type detectorMask1,
                                      number_type detectorMask2,
                                      number_type detectorMask3)
{
    m_detector_mask0 = detectorMask0;
    m_detector_mask1 = detectorMask1;
    m_detector_mask2 = detectorMask2;
    m_detector_mask3 = detectorMask3;
}

