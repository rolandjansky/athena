// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventID.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_EVENTID_H
#define TRIGT1MUCTPI_EVENTID_H

namespace LVL1MUCTPI {

  class EventIDSingleton;

  /**
   ************************************************************************
   *
   *    $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $ 
   *
   *    @short A class which handels the EventID
   *
   *           The EventID class can be instantiated in any object that 
   *           needs access to the EventID. The actual EventID is kept
   *           in the EventIDSingleton, as their should be only one
   *
   *      @see EventIDSingleton
   *   @author Thorsten Wengler
   *  @version $Revision: 362102 $
   *
   ************************************************************************
   */
  class EventID {

  public:
    /**
     * The constructor 
     */
    EventID();
    /**
     * The destructor 
     */
    ~EventID();
    /**
     * reset the BCID to ZERO
     */
    void resetBCID();
    /**
     * set the BCID to newBCID
     * @param newBCID the new BCID
     */
    void BCID( unsigned int newBCID );
    /**
     * get the BCID
     * @return the BCID
     */
    unsigned int BCID();
    /**
     * reset the EventID to ZERO
     */
    void resetEvID();
    /**
     * set the EventID to newID
     * @param newID the new Event ID
     */
    void setEvID( unsigned int newID );
    /**
     * Increment the EventID by one
     */
    void incrementEvID();
    /**
     * Get the current EventID
     * @return the current EventID
     */
    unsigned int getEvID();
    /**
     * Get the trigger Type word
     * @return the trigger type word
     */
    unsigned int getTriggerType();
    /**
     * Set the trigger Type word
     * @param theType the new trigger type word
     */
    void setTriggerType( unsigned int theType );
    /**
     * Get the monitor Event bit
     * @return if true event will be monitored, if false not
     */
    bool getMonitorEvent();
    /**
     * Set the monitor Event bit
     * @param doMonitor if true event will be monitored, if false not
     */
    void setMonitorEvent( bool doMonitor );

    unsigned int getRunNumber();
    void setRunNumber( unsigned int runNumber );

    unsigned int getECRC();
    void setECRC( unsigned int ECRC );

  private:
    EventIDSingleton* m_eventIDKeeper;

  }; // class EventID

} // namespace LVL1MUCTPI

#endif  // TRIGT1MUCTPI_EVENTID_H
