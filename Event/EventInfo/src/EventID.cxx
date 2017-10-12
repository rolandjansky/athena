/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventInfo/src/EventID.cxx
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
  EventIDBase() 
{}

EventID::EventID(number_type run_number, 
                 uint64_t event_number) :
  EventIDBase(run_number, event_number)
{}

EventID::EventID(number_type run_number, 
		 uint64_t event_number,
                 number_type time_stamp) :
  EventIDBase(run_number, event_number, time_stamp)
{}

EventID::EventID(number_type run_number, 
		 uint64_t event_number,
		 number_type time_stamp,
                 number_type time_stamp_ns_offset,
                 number_type lumi_block,
                 number_type bunch_crossing_id) :
  EventIDBase(run_number, event_number, time_stamp, time_stamp_ns_offset,
              lumi_block, bunch_crossing_id)
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
  EventIDBase(run_number, event_number, time_stamp, time_stamp_ns_offset,
              lumi_block, bunch_crossing_id),
    m_detector_mask0       (detector_mask0),
    m_detector_mask1       (detector_mask1),
    m_detector_mask2       (detector_mask2),
    m_detector_mask3       (detector_mask3)
{}


EventID::~EventID()
{}



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

