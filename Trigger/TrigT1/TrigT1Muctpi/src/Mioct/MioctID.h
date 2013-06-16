// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MioctID.h 364083 2011-05-06 09:09:55Z krasznaa $
#ifndef TRIGT1MUCTPI_MIOCTID_H
#define TRIGT1MUCTPI_MIOCTID_H

// Local include(s):
#include "../Common/Detector.h"
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   // I forward declare the MioctID class to be able to declare the comparison
   // operator already...
   class MioctID;

   /// The ability to compare Mioct IDs comes in handy, when MioctID objects are
   /// used in std::map-s
   bool operator< ( const MioctID& id1, const MioctID& id2 );

   /// Printer operator
   MsgLogger& operator<<( MsgLogger& out, const MioctID& id );

  /**
   ************************************************************************
   *
   *    $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $ 
   *
   *    @short Contains all data to define a MioctModule
   *
   *           This class contains the identification of an Octant board
   *           and serves to generate the right Sectors for this board 
   *           by providing a set of functions which return the lowest 
   *           sector number for the Barrel-, Endcap-, and Forward 
   *           sectors of the module.
   *
   *      @see MioctSectorSet
   *      @see Detector.h
   *   @author $Author: krasznaa $
   *  @version $Revision: 364083 $
   *
   *
   ************************************************************************
   */
  class MioctID {

  public:
    /**
     * An ID is defined by two parameters.
     * @param aNumber the number of the octant board (1..8).
     * @param aHemisphere can be POSITIVE or NEGATIVE
     * @see Detector.h
     */
    MioctID( const unsigned int aNumber, const Hemisphere aHemisphere );
    /**
     * Access function to the MioctNumber of the MioctID.
     * @return The Number of the octant board.
     */
    unsigned int getNumber() const { return m_number; }
    /**
     * Access function to the Rapidity Region of the MioctID.
     * @return The Hemisphere of the octant board. (POSITIVE
     *         or NEGATIVE as defined in Detector.h)
     * @see Detector.h
     */
    Hemisphere getRapidityRegion() const { return m_rapidityRegion; }
    /**
     * This method returns the sector ID as build by the MIROD from
     * the MioctModuleID, the Hemisphere, and the sector ID in the 
     * MIOCT
     * @see MirodModule
     * @see MirodEventReader
     * @param sectorID The ID of the sector in the MIOCT
     * @return sector Address in MIROD Eventreader format
     */
    unsigned int getMirodSecAddress( unsigned int sectorID );
    /**
     * Get the first (lowest) Barrel-Sector number for the octant board.
     * @return The Sector ID number (0-31)
     */
    unsigned int getFirstBarrelID() const;
    /**
     * Get the first (lowest) Endcap-Sector number for the octant board.
     * @return The Sector ID number (0-47)
     */
    unsigned int getFirstEndcapID() const;
    /**
     * Get the first (lowest) Forward-Sector number for the octant board.
     * @return The Sector ID number (0-23)
     */
    unsigned int getFirstForwardID() const;

  private:

    Hemisphere      m_rapidityRegion;
    unsigned int    m_number;

    friend bool LVL1MUCTPI::operator< ( const MioctID& id1, const MioctID& id2 );

  }; // class MioctID

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIOCTID_H
