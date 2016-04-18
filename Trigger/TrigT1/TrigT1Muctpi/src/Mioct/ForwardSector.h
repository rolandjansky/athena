// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ForwardSector.h 701446 2015-10-19 15:19:09Z wengler $
#ifndef TRIGT1MUCTPI_FORWARDSECTOR_H
#define TRIGT1MUCTPI_FORWARDSECTOR_H

// STL include(s):
#include <string>

// Local include(s):
#include "Sector.h"
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   /**
    *  @short This class handles an 32-bit Input word from the Forward Front-End-Electronics. 
    *
    *         This  class  complies to the Sector   Interface class.  It
    *         specializes  the  base class with some  decoding functions
    *         which are  special for the  Forward Sector data-format. It
    *         provides a  print  facility to  view  the contents  of the
    *         data-word in readable format.
    *
    *    @see Sector
    *    @see SectorID
    * @author $Author: krasznaa $
    *
    * $Revision: 701446 $
    * $Date: 2015-10-19 17:19:09 +0200 (Mon, 19 Oct 2015) $
    */
   class ForwardSector : public Sector {

   public:
      /**
       * The only constructor of a Forward sector, identified by its
       * sector number and hemisphere
       * @param hemisphere The Hemisphere the sector is in
       * @param sectorNumber The sector number
       */
      ForwardSector( Hemisphere hemisphere, unsigned int sectorNumber,
                     EventReader* reader );
      ForwardSector( const ForwardSector& sector );
      ForwardSector & operator = ( const ForwardSector & ) = delete;

      ~ForwardSector();
      /**
       * Implementation of the corresponding abstract method in the
       * baseclass Sector.
       * @see Sector::set()
       * @param bitfield This is the value to which the
       *                 Forward Sector 32-bit bitfield is set.
       *                 It is checked that none of the reserved bits are set.
       */
      void set( const unsigned int bitfield );
      /**
       * This access funtion retrieves the ROI-number of the first candidate.
       */
      unsigned int getROI1() const;
      /**
       * This access funtion retrieves the ROI-number of the second candidate.
       */
      unsigned int getROI2() const;
      void print( std::string& ForwardDescription ) const;
      std::string getDetectorString() const { return "Forward"; }
      virtual SectorID getSectorID() const;

   private:
      mutable MsgLogger m_logger;

      /**
       * some constants important for checking the validity of
       * the SectorID
       */
      static const unsigned int MIN_SECTOR;
      static const unsigned int MAX_SECTOR;

   }; // class ForwardSector

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_FORWARDSECTOR_H
