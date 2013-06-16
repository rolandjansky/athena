// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventIDSingleton.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_EVENTIDSINGLETON_H
#define TRIGT1MUCTPI_EVENTIDSINGLETON_H

namespace LVL1MUCTPI {

  /**
   ************************************************************************
   *
   *    $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $ 
   *
   *    @short Keeps track of the EventID
   *
   *           The EventIDSingleton keeps track of the EventID. It is 
   *           implemented as a singleton, as the EventID is used in
   *           several places, but should only exist once.
   *
   *      @see EventID
   *   @author  Author: Thorsten Wengler $
   *  @version $Revision: 362102 $
   *
   *
   ************************************************************************
   */
  class EventIDSingleton {

  public:
    /**
     * This is the static memember function of the Singleton, which 
     * instantiates the only object of EventIDSingleton. 
     * @return Pointer to the only instantiation of the EventIDSingleton.
     */
    static EventIDSingleton* instance();
    ~EventIDSingleton();
    /**
     * reset the BCID to ZERO
     */
    void resetBCID() { m_BCID = 0; }
    /**
     * set the EventID to newBCID
     * @param newBCID the new Event ID
     */
    void BCID( unsigned int newBCID ) { m_BCID = newBCID; }
    /**
     * get the EventID to newBCID
     * @return the new Event ID
     */
    unsigned int BCID() { return m_BCID; }
    /**
     * reset the EventID to ZERO
     */
    void resetEvID() { m_eventID = 0; }
    /**
     * set the EventID to newID
     * @param newID the new Event ID
     */
    void setEvID( unsigned int newID ) { m_eventID = newID; }
    /**
     * Increment the EventID by one
     */
    void incrementEvID() { ++m_eventID; }
    /**
     * Get the current EventID
     * @return the current EventID
     */
    unsigned int getEvID() { return m_eventID; }
    /**
     * Get the trigger Type word
     * @return the trigger type word
     */
    unsigned int getTriggerType() { return m_triggerType; }
    /**
     * Set the trigger Type word
     * @param theType the new trigger type word
     */
    void setTriggerType( unsigned int theType ) { m_triggerType = theType; }
    /**
     * Get the monitor Event bit
     * @return if true event will be monitored, if false not
     */
    bool getMonitorEvent() { return m_monitorEventBit; }
    /**
     * Set the monitor Event bit
     * @param doMonitor if true event will be monitored, if false not
     */
    void setMonitorEvent( bool doMonitor ) { m_monitorEventBit = doMonitor; }

    unsigned int getRunNumber() { return m_runNumber; }
    void setRunNumber( unsigned int runNumber ) { m_runNumber = runNumber; }

    unsigned int getECRC() { return m_ECRC; }
    void setECRC( unsigned int ECRC ) { m_ECRC = ECRC; }

  private:

    static EventIDSingleton  *m_theOnlyEventID;

    /// it's a Singleton: so the constructor is private
    EventIDSingleton();
    unsigned int m_eventID;        // the actual EventID
    unsigned int m_ECRC;           // the event counter reset counter
    unsigned int m_triggerType;    // the trigger type word
    unsigned int m_BCID;           // the BCID
    unsigned int m_runNumber;      // the run number
    bool m_monitorEventBit;        // the Monitor selector

  }; // class EventIDSingleton

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_EVENTIDSINGLETON_H
