/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventIncident.cxx
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov>
 *
 * $Id: EventIncident.cxx,v 1.2 2005-01-12 11:07:53 schaffer Exp $
 */
#include "EventInfo/EventIncident.h"

EventIncident::EventIncident(const EventInfo& event,
			     const std::string& source,
			     const std::string& type) :
  Incident(source, type), m_eventInfo(event) {}

EventIncident::EventIncident(const EventIncident& e) :
  Incident(e), m_eventInfo(e.m_eventInfo) {}

EventIncident::~EventIncident() {}
