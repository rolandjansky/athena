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

EventIncident::EventIncident(const std::string& source,
			     const std::string& type,
			     const EventContext& ctx) 
  : Incident(source, type, ctx)
{}

EventIncident::~EventIncident() 
{}
