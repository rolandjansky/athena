// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Sector.h 796872 2017-02-13 15:03:25Z fwinkl $
#ifndef TRIGT1MUCTPI_SECTOR_H
#define TRIGT1MUCTPI_SECTOR_H

#include <string>
#include "CxxUtils/bitscan.h"

// Local include(s):
#include "../Common/Detector.h"
#include "../Common/MuctpiBitMasks.h"
#include "SectorID.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class PtMultiplicitySet;
   class EventReader;

   /************************************************************************
    *
    *    $Date: 2017-02-13 16:03:25 +0100 (Mon, 13 Feb 2017) $ 
    *
    *    @short Base class for a Sector of the Muon Trigger chambers.
    *
    *           This is the abstract base class for a sector in the 
    *           MUCTPI system. It contains the acutal data of the sector 
    *           and an identifier. For the information which can be 
    *           retrieved equally for all kinds of Sectors, access 
    *           functions are provided.
    *
    *      @see SectorID
    *      @see PtMultiplicitySet
    *      @see BarrelSector
    *      @see EndcapSector
    *      @see ForwardSector
    *      @see MuctpiBitMasks.h
    *      @see SectorConstants.h
    *      @see Detector.h
    *   @author $Author: krasznaa $
    *  @version $Revision: 796872 $
    *
    **//////////////////////////////////////////////////////////////////////
   class Sector {

   public:
      /**
       * The only constructor of a Sector, identified by its
       * sector number and hemisphere
       * @param theHemisphere   The Hemisphere the sector is in
       * @param theSectorNumber The sector number
       * @param reader          Helper class for reading the MuCTPI input
       */
      Sector( const Hemisphere   theHemisphere, 
              const unsigned int theSectorNumber,
              EventReader*       reader );
      virtual ~Sector();
      /**
       * This function is abstract because when setting the bitfield
       * a check is performed that none of the reserved bits is set. 
       * Since these bits are at different positions in the different
       * sector species, the implementation of this function has to 
       * be done in the corresponding subclasses.
       */
      virtual void set( const unsigned int bitfield ) = 0;
      /**
       * The get function returns a const reference to the bitfield - 
       * dont want to copy it for performance reasons 
       * @return Reference to the BitFIeld
       */
      unsigned int* getBitFieldPtr() { return &m_bitField; }
      /**
       * Extract the threshold of Candidate 1.
       */
      unsigned int getPtCand1() const { return getValue(Pt1Mask); }
      /**
       * Extract the threshold of Candidate 2.
       */
      unsigned int getPtCand2() const { return getValue(Pt2Mask); }
      virtual unsigned int getROI1() const = 0;
      virtual unsigned int getROI2() const = 0; 
      /**
       * Determined if candidate 1 of the Sector overlaps with a barrel 
       * sector. The default implementation returns false so that there
       * is no implementation needed for the forward sector which is not
       * overlapping.
       * @return true if there is overlap.
       */
      virtual bool isCand1BarrelOverlap() const { return false; }
      /**
       * Determined if candidate 2 of the Sector overlaps with a barrel 
       * sector. The default implementation returns false so that there
       * is no implementation needed for the forward sector which is not
       * overlapping.
       * @return true if there is overlap.
       */
      virtual bool isCand2BarrelOverlap() const { return false; }
      /**
       * Determined if candidate 1 of the Sector overlaps with an endcap 
       * sector. The default implementation returns false so that there
       * is no implementation needed for the forward sector which is not
       * overlapping.
       * @return true if there is overlap.
       */
      virtual bool isCand1EndcapOverlap() const { return false; }
      /**
       * Determined if candidate 2 of the Sector overlaps with an endcap 
       * sector. The default implementation returns false so that there
       * is no implementation needed for the forward sector which is not
       * overlapping.
       * @return true if there is overlap.
       */
      virtual bool isCand2EndcapOverlap() const { return false; }
      /**
       * This function prints the identification of the Sector into
       * the string it returns.
       * @return string which contains the identification of the Sector.
       */
      void printID( std::string& IDString ) const;
     /**
      * get the ID of the sector as a string of type B30, FA12 ...
      */
      std::string getIDString() const;
      virtual void print( std::string& TextDescription ) const = 0;
      virtual std::string getDetectorString() const = 0;
      std::string getRapidityString() const;
      unsigned int getSectorNumber() const { return m_sectorNumber; }
      unsigned int getBCID() const { return getValue( BCIDMask ); }
      virtual SectorID getSectorID() const = 0;

      void setCand1Supressed( bool flag ) const { m_cand1Supressed = flag; }
      bool getCand1Supressed() const { return m_cand1Supressed; }
      void setCand2Supressed( bool flag ) const  { m_cand2Supressed = flag; }
      bool getCand2Supressed() const { return m_cand2Supressed; }

   protected:
      /**
       * Contains the data of the current event.
       */
      unsigned int m_bitField;
      bool m_registered;
      /**
       * This function retrieves information from the bitfield of a sector.
       *
       * @param mask the bitmask as defined in MuctpiBitMasks.h.
       * @return the value of the bitfield defined by the mask.
       * @see MuctpiBitMasks.h
       */
      unsigned int getValue( const BitMask mask ) const {
        return (m_bitField & mask) >> CxxUtils::count_trailing_zeros(mask);
      }

      Hemisphere   m_rapidityRegion;
      unsigned int m_sectorNumber;
      EventReader* m_reader;

      mutable bool m_cand1Supressed;
      mutable bool m_cand2Supressed;

   }; // class Sector

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_SECTOR_H
