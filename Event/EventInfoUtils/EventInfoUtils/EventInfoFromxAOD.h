// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFOFROMXAOD_H
#define EVENTINFOFROMXAOD_H

#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "xAODEventInfo/EventInfo.h"


/// Create EventID object from xAOD::EventInfo
EventID eventIDFromxAOD( const xAOD::EventInfo* xaod );

/// Create EventType object from xAOD::EventInfo
EventType eventTypeFromxAOD( const xAOD::EventInfo* xaod );


#endif


