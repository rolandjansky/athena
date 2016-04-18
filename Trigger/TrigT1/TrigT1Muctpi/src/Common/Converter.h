// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Converter.h 707664 2015-11-13 05:36:00Z ssnyder $
#ifndef TRIGT1MUCTPI_CONVERTER_H
#define TRIGT1MUCTPI_CONVERTER_H

// STL include(s):
#include <vector>
#include <iostream>

// Gaudi/Athena include(s):
#include "GaudiKernel/StatusCode.h"

// Forward declaration(s):
namespace LVL1MUONIF {
  class Lvl1MuCTPIInput;
}

namespace LVL1MUCTPI {

   /**
    *  @short Class that can be used to convert a Muon_ROI and MuCTPI_RDO objects into Lvl1MuCTPIInput
    *
    *         When converting RoIs:
    *         First the muon RoIs should be extracted from the LVL1_ROI object into
    *         a vector of 'unsigned int'-s. The Converter::convertRoIs() function is
    *         then able to fill a Lvl1MuCTPIInput object with the data extracted from
    *         the RoIs.
    *
    *         When converting the data words from MuCTPI_RDO:
    *         The data words can be passed to the Converter::converRDO() function
    *         difectly with MuCTPI_RDO::dataWord().
    *
    *         I've tried making it as light-weight as possible, so that it wouldn't
    *         depend on the rest of the actual MuCTPI simulation.
    *
    *    @see LVL1MUONIF::Lvl1MuCTPIInput
    *    @see MuCTPI_RDO
    *    @see LVL1_ROI
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 707664 $
    * $Date: 2015-11-13 06:36:00 +0100 (Fri, 13 Nov 2015) $
    */
   class Converter {

   public:
      /// This function converts the RoIs in the RoIBResult back to the input format
      static StatusCode convertRoIs( const std::vector< unsigned int >& roi_vector,
                                     LVL1MUONIF::Lvl1MuCTPIInput* muctpi_input );
      /// This function converts the data words in the RDO back to the input format
      static StatusCode convertRDO( const std::vector< unsigned int >& rdo_vector,
                                    unsigned int bcid,
                                    LVL1MUONIF::Lvl1MuCTPIInput* muctpi_input );

   private:
      enum SectorType { Barrel, Endcap, Forward };
      static SectorType getSectorType( unsigned int sector_address );
      static unsigned int getRoISectorAddress( unsigned int roi_word );
      static bool isRoIFirstCand( unsigned int roi_word );

      static unsigned int getRDOSectorAddress( unsigned int rdo_word );
      static unsigned int getRDOBCID( unsigned int rdo_word );
      static bool isRDOFirstCand( unsigned int rdo_word );

      //
      // The following are private, helper classes to ease the conversion:
      //

      class HelperSector {

      public:
         HelperSector() { m_word = 0xc7ffffff; m_sector = 0; }
         HelperSector& operator= ( const HelperSector& right );
         virtual ~HelperSector() { }

         virtual void addFirst( unsigned int roi_word ) = 0;
         virtual void addSecond( unsigned int roi_word ) = 0;
         unsigned int sectorWord() const { return m_word; }
         unsigned int sectorAddress() const { return m_sector; }
         enum MuonSubSystem { A_side = 1, C_side = 0 };
         MuonSubSystem subSystem() const;

         virtual unsigned int sectorID() const = 0;
         virtual std::string system() const = 0;

         std::string toString() const;

      protected:
         unsigned int m_word;
         unsigned int m_sector;

      }; // class HelperSector

      class HelperRoISector : public HelperSector {

      public:
         virtual void addFirst( unsigned int roi_word );
         virtual void addSecond( unsigned int roi_word );

      }; // class HelperRoISector

      class HelperRoIBarrelSector : public HelperRoISector {

      public:
         virtual unsigned int sectorID() const;
         virtual std::string system() const { return "Barrel"; }

      }; // class HelperRoIBarrelSector

      class HelperRoIEndcapSector : public HelperRoISector {

      public:
         virtual unsigned int sectorID() const;
         virtual std::string system() const { return "Endcap"; }

      }; // class HelperRoIEndcapSector

      class HelperRoIForwardSector : public HelperRoISector {

      public:
         virtual unsigned int sectorID() const;
         virtual std::string system() const { return "Forward"; }

      }; // class HelperRoIForwardSector

      class HelperRDOSector : public HelperSector {

      public:
         virtual void addFirst( unsigned int roi_word );
         virtual void addSecond( unsigned int roi_word );

      }; // class HelperRDOSector

      class HelperRDOBarrelSector : public HelperRDOSector {

      public:
         virtual unsigned int sectorID() const;
         virtual std::string system() const { return "Barrel"; }

      }; // class HelperRDOBarrelSector

      class HelperRDOEndcapSector : public HelperRDOSector {

      public:
         virtual unsigned int sectorID() const;
         virtual std::string system() const { return "Endcap"; }

      }; // class HelperRDOEndcapSector

      class HelperRDOForwardSector : public HelperRDOSector {

      public:
         virtual unsigned int sectorID() const;
         virtual std::string system() const { return "Forward"; }

      }; // class HelperRDOForwardSector


      //
      // Constants:
      //
      static const unsigned int RoIandOverlapMask;
      static const unsigned int PtMask;
      static const unsigned int RoI_SectorAddressMask;
      static const unsigned int RDO_SectorAddressMask;
      static const unsigned int RoI_FirstCandMask;
      static const unsigned int RDO_FirstCandMask;
      static const unsigned int MoreCandInRoI;
      static const unsigned int MoreCandInSec;
      static const unsigned int Cand1InverseMask;
      static const unsigned int Cand2InverseMask;

   }; // class Converter

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_CONVERTER_H
