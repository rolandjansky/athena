// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SectorLogicSource.h 681356 2015-07-08 12:17:52Z wengler $
#ifndef TRIGT1MUCTPI_SECTORLOGICSOURCE_H
#define TRIGT1MUCTPI_SECTORLOGICSOURCE_H

// STL include(s):
#include <string>

// Local include(s):
#include "EventSource.h"

// Forward declaration(s):
namespace LVL1MUONIF {
  class Lvl1MuCTPIInput;
  class Lvl1MuSectorLogicData;
}

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class Sector;

   /**
    *  @short The EventSource for the SectorLogicReader
    *
    *         This EventSource consists of an Lvl1MuCTPIInput object
    *         as provided by the simulation of the Muon Trigger
    *         system simulation. The pointer to the input object is
    *         set with the setSectorLogicSource method. A pointer
    *         to the sector in the input object which hold the data
    *         word for the sector to be filled is retrieved using the
    *         getSectorLogicPointer method.
    *
    *    @see SectorLogicReader
    *    @see Lvl1MuCTPIInput
    * @author Thorsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 681356 $
    * $Date: 2015-07-08 14:17:52 +0200 (Wed, 08 Jul 2015) $
    */
   class SectorLogicSource : public EventSource {

   public:
      SectorLogicSource();
      /**
       * Sets the pointers to the Lvl1MuCTPIInput which holds the input
       * data from the Sector Logic
       */
      void setBcidOffset( int bcidOffset) { m_bcidOffset= bcidOffset; }
      int getBcidOffset() { return m_bcidOffset; }
      void setSectorLogicSource( const LVL1MUONIF::Lvl1MuCTPIInput* );
      /**
       * Gets the pointer to the Lvl1MuCTPIInput which holds the input
       * data from the
       */
      const LVL1MUONIF::Lvl1MuCTPIInput* getSectorLogicSource() const;
      /**
       * Retrieves the pointer of the Lvl1MuSectorLogicData object in
       * which the dataword of the argument Sector can be found.
       */
      const LVL1MUONIF::Lvl1MuSectorLogicData& getSectorLogicAddress( const Sector* Sector );
      /**
       * This function is dummy. It only exists in order to be able
       * to use the dynamic_cast for the EventSource objects. (At least
       * one abstract method needs to be defined in the base class in
       * order to be able to use the dynamic_cast).
       */
      virtual const char* printSource() const { return "printSource is a dummy"; }

   private:
      const LVL1MUONIF::Lvl1MuCTPIInput* m_lvl1MuCTPIInput;
      int m_bcidOffset;

   }; // class SectorLogicSource

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_SECTORLOGICSOURCE_H
