// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BarrelSector.h 364083 2011-05-06 09:09:55Z krasznaa $
#ifndef TRIGT1MUCTPI_BARRELSECTOR_H
#define TRIGT1MUCTPI_BARRELSECTOR_H

// STL include(s):
#include <string>

// Local include(s):
#include "../Common/Detector.h"
#include "Sector.h"
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   /**
    *  @short This class handles an 32-bit Input word from the Barrel Front-End-Electronics.
    *
    *         This  class  complies to the Sector   Interface class.  It
    *         specializes  the  base class with some  decoding functions
    *         which are  special for the  Barrel Sector  data-format. It
    *         provides a  print  facility to  view  the contents  of the
    *         data-word in readable format.
    *
    *    @see Sector
    * @author Thirsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 364083 $
    * $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $
    */
   class BarrelSector : public Sector {

   public:
      /**
       * The only constructor of a Barrel sector, identified by its
       * sector number and hemisphere
       * @param hemisphere   The Hemisphere the sector is in
       * @param sectorNumber The sector number
       */
      BarrelSector( Hemisphere hemisphere, unsigned int sectorNumber,
                    EventReader* reader );
      BarrelSector( const BarrelSector& sector );
      ~BarrelSector();

      /**
       * Implementation of the corresponding abstract method in the
       * baseclass Sector.
       * @see Sector#set
       * @param bitField This is the value to which the
       *                 Barrel Sector 32-bit bitfield is set.
       *                 It is checked that none of the reserved bits are set.
       */
      void set( const unsigned int bitField );
      /**
       * This access funtion retrieves the ROI-number of the first candidate.
       */
      unsigned int getROI1() const;
      /**
       * This access funtion retrieves the ROI-number of the second candidate.
       */
      unsigned int getROI2() const;
      /**
       * Acces function to the bit indicating overlap of the
       * first candidate with the neigbouring Barrel Sector.
       * @return boolean value of the flag
       */
      bool isCand1BarrelOverlap() const;
      /**
       * Access function to the bit indicating overlap of the
       * second candidate with the neigbouring Barrel Sector.
       * @return boolean value of the flag
       */
      bool isCand2BarrelOverlap() const;
      /**
       * Access function to the bit indicating overlap of the
       * first candidate with a neigbouring Endcap Sector.
       * @return boolean value of the flag
       */
      bool isCand1EndcapOverlap() const;
      /**
       * Access function to the bit indicating overlap of the
       * second candidate with a neigbouring Endcap Sector.
       * @return boolean value of the flag
       */
      bool isCand2EndcapOverlap() const;

      void print( std::string& barrelDescription ) const;
      std::string getDetectorString() const { return "Barrel"; }
      virtual SectorID getSectorID() const;

   private:
      mutable MsgLogger m_logger;

      /**
       * some constants important for checking the validity of
       * the SectorID
       */
      static const unsigned int MIN_SECTOR;
      static const unsigned int MAX_SECTOR;

   }; // class BarrelSector

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_BARRELSECTOR_H
