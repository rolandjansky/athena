/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFO_MERGEDEVENTINFO_H
# define EVENTINFO_MERGEDEVENTINFO_H 1
/**
 * @file MergedEventInfo.h
 *
 * @brief This class provides general information about an event. 
 *  It extends MergedEventInfo with a list of sub-evts (the original
 *  and the bkg ones) 
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov>
 *
 * $Id: MergedEventInfo.h,v 1.2 2005-01-12 11:07:52 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>
#ifndef GAUDIKERNEL_CLASSID_H
# include "GaudiKernel/ClassID.h"
#endif

#ifndef EVENTINFO_EVENTINFO_H
# include "EventInfo/EventInfo.h"
#endif
#ifndef EVENTINFO_EVENTID_H
# include "EventInfo/EventID.h"
#endif
//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class EventType;
class TriggerInfo;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/** @class MergedEventInfo
 *
 * @brief This class provides general information about an event. 
 *  It extends MergedEventInfo with a list of sub-evts (the original
 *  and the bkg ones) 
 *
 **/

class MergedEventInfo : public EventInfo {
public:

  /// \name structors
  //@{
  MergedEventInfo();  ///< POOL required
  // Use default copy constructor.
  /// the constructor to be used
  MergedEventInfo(const EventInfo& origEvent, 
		  EventID::number_type newRunNo,
		  EventID::number_type newEvtNo,
		  EventID::number_type newTimeStamp=0);
  virtual ~MergedEventInfo();
  //@}

  /// \name DataObject-like clid accessors
  //@{
  static const CLID& classID();
  const CLID& clID() const;
  //@}
  
  /// \name Event information accessors
  //@{
  ///the new identification of the event.
  const EventID* event_ID() const;
  //INHERITED EventType* event_type() const;
  //INHERITED TriggerInfo* trigger_info() const;
  ///the original identification of the event.
  const EventID* origEvent_ID() const;
  //@}

private:
  EventID 	m_newEventID;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline const EventID*
MergedEventInfo::event_ID() const
{
    return &m_newEventID;
}

inline const EventID*
MergedEventInfo::origEvent_ID() const
{
    return EventInfo::event_ID();
}



inline const CLID& 
MergedEventInfo::clID() const
{ 
  return classID(); 
}

#ifndef CLIDSVC_CLASSDEF_H
# include "SGTools/CLASS_DEF.h"
#endif
CLASS_DEF( MergedEventInfo , 220174395 , 1 )

inline const CLID& 
MergedEventInfo::classID() 
{ 
  return ClassID_traits<MergedEventInfo>::ID(); 
}

#endif // MERGEDEVENTINFO_MERGEDEVENTINFO_H


