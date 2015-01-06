/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MergedEventInfo.cxx
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov>
 *
 * $Id: MergedEventInfo.cxx,v 1.2 2005-01-12 11:07:53 schaffer Exp $
 */
#include "EventInfo/MergedEventInfo.h"

MergedEventInfo::MergedEventInfo() : 
  EventInfo(), m_newEventID() {}

MergedEventInfo::MergedEventInfo(const EventInfo& origEvent, 
				 EventID::number_type newRunNo,
				 EventID::number_type newEvtNo,
				 EventID::number_type newTimeStamp) :
  EventInfo(origEvent), m_newEventID(newRunNo, newEvtNo, newTimeStamp) {}

MergedEventInfo::~MergedEventInfo() {}

