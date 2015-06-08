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

//<<<<<< FORWARD DECLS                                                  >>>>>>

class EventInfo;

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
  ///@param event    the EventInfo of the event in question
  ///@param source   the name of the service/algorithm firing
  ///@param type     e.g. "BeginEvent"
  EventIncident(const EventInfo& event, 
		const std::string& source,
		const std::string& type=std::string("BeginEvent"));
  #ifdef ATHENAHIVE
  EventIncident(const EventInfo& event, 
		const std::string& source,
		const std::string& type,
		EventContext& ctx);
  #endif
  // Use default copy constructor.
  virtual ~EventIncident();
  //@}

  /// \name Event information accessors
  //@{
  ///the unique identification of the event.
  const EventInfo& eventInfo () const;
  //@}

private:
  const EventInfo& m_eventInfo;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
inline const EventInfo&
EventIncident::eventInfo () const
{
    return m_eventInfo;
}
#endif // EVENTINFO_EVENTINCIDENT_H


