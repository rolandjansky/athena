/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFO_EVENTINCIDENT_H
# define EVENTINFO_EVENTINCIDENT_H 1
/**
 * @file EventIncident.h
 *
 * @brief an incident that carries the EventInfo object
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov>
 *
 * $Id: EventIncident.h,v 1.2 2005-01-12 11:07:52 schaffer Exp $
 */


//<<<<<< INCLUDES                                                       >>>>>>
#include <string>
#ifndef GAUDIKERNEL_INCIDENT_H
# include "GaudiKernel/Incident.h"
#endif

#include "GaudiKernel/EventContext.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/** @class EventIncident
 * @brief an incident that carries the EventInfo object
 * @author pcalafiura@lbl.gov - ATLAS Collaboration
 **/

class EventIncident : public Incident {
public:

  /// \name structors
  //@{
  /// standard constructor
  ///@param source   the name of the service/algorithm firing
  ///@param type     e.g. "BeginEvent"
  ///@param ctx      event context
  EventIncident(const std::string& source,
		const std::string& type,
		const EventContext& ctx);
  // Use default copy constructor.
  virtual ~EventIncident();
  //@}

private:
};

#endif // EVENTINFO_EVENTINCIDENT_H


