/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CTP_EVENTINFO_H
#define TRIGT1CTP_EVENTINFO_H

// STL include(s):
#include <string>
#include <memory>

namespace LVL1CTP {

  /**
   *
   *   @short Just an idea of how to store event related information in the CTP sim...
   *
   *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
   *
   * @version \$Id: EventInfo.h,v 1.6 2009-01-18 04:35:55 ssnyder Exp $
   *
   */

  class EventInfo {

  public:

    static EventInfo& instance();                        //!< get singleton instance

    void clear();                                        //!< clear all data members
    const std::string dump() const;                      //!< dump all data members to string

    unsigned int runNumber() const;                      //!< get run number
    void setRunNumber( unsigned int value );             //!< set run number

    unsigned int eventNumber() const;                    //!< get event number
    void setEventNumber( unsigned int value );           //!< set event number

    unsigned int eventType() const;                      //!< get LVL1 event type
    void setEventType( unsigned int type );              //!< set LVL1 event type

    unsigned int triggerType() const;                    //!< get LVL1 trigger type
    void setTriggerType( unsigned int type );            //!< set LVL1 trigger type

    unsigned int extendedL1ID() const;                   //!< get extended LVL1 ID
    void setExtendedL1ID( unsigned int l1id );           //!< set extended LVL1 ID

    unsigned int bcid() const;                           //!< get bunch crossing ID
    void setBCID( unsigned int bcid );                   //!< set bunch crossing ID

    unsigned int time() const;                           //!< get time in sec
    void setTime( unsigned int time );                   //!< set time in sec

    unsigned int timeNs() const;                         //!< get time offset in ns
    void setTimeNs( unsigned int time );                 //!< set time offset in ns

  protected:
    
    //! default constructor
    EventInfo();        

  private:

    static std::auto_ptr<EventInfo> m_instance;

    unsigned int m_runNumber;       //!< run number
    unsigned int m_eventNumber;     //!< event number
    unsigned int m_eventType;       //!< LVL1 event type
    unsigned int m_triggerType;     //!< LVL1 trigger type
    unsigned int m_l1id;            //!< extended LVL1 ID
    unsigned int m_bcid;            //!< bunch crossing ID 
    unsigned int m_utcTime;         //!< universal time in sec
    unsigned int m_timeNsOffset;    //!< time offset in ns

  }; // class EventInfo

} // namespace LVL1CTP

#endif // TRIGT1CTP_EVENTINFO_H
