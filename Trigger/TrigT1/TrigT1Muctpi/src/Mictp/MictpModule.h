// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MictpModule.h 364083 2011-05-06 09:09:55Z krasznaa $
#ifndef TRIGT1MUCTPI_MICTPMODULE_H
#define TRIGT1MUCTPI_MICTPMODULE_H

// STL include(s):
#include <list>

// Local include(s):
#include "MictpData.h"
#include "../Common/EventID.h"
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class PtMultiplicitySet;

   /**
    *  @short Contains all relevant entities of the MICTP board
    *
    *         The MictpModule class groups together all relevant
    *         entities for the interface board MICTP of the MUCTPI
    *         to the Central Trigger Processor. It provides service
    *         to extract the data delivered to the CTP and to the
    *         Read Out Driver board MIROD. The MICTP gets its
    *         information from the MioctModule objects, via the
    *         MIBAK. Also in the software the MictpModule class is
    *         filled by a call to the method getPtMultiplicity in
    *         class Lvl1Mibak. There is only one MICTP board in the
    *         system.
    *
    *    @see MiordModule
    *    @see MioctModule
    *    @see Lvl1Mibak
    *
    * @author Thorsten Wengler
    *
    * $Revision: 364083 $
    * $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $
    */
   class MictpModule {

   public:
      /**
       * The constructor of MictpModule
       */
      MictpModule();
      /**
       * This method sets the Pt sums for all thresholds in the data
       * and the BCID that goes with it. The trailor word will hold
       * the Pt sums of the middle word, as the BCIDs to be read
       * out are opend up in symmetrical steps of 1 on either side
       * @param thePtSums Pointer to the total sum of pt
       *                  candidates per threshold calculated
       *                  from all MIOCTS
       * @param BCID      corresponding Bunch Id
       * @param ECRC      corresponding to Event Counter
       *                  Reset Counter
       */
      void setPtSums( PtMultiplicitySet* thePtSums, unsigned int BCID, unsigned int ECRC );
      /**
       * method to get the word sent by the Mictp to the CTP
       * @return CTP Data Word
       */
      unsigned int getMictpCTPWord() const { return m_CTPDataWord; }
      /**
       * This method return the data stored in the MictpModule as a
       * MictpData object, which groups the data for easy access in the
       * format found on the MIBAK backplane
       * @return data stored in the MICTP for easy access in
       *         MIBAK format
       */
      const MictpData* getMictpOnMibak() { return &m_mictpData; }

   private:
      std::list< unsigned int >   m_highBits;
      std::list< unsigned int >   m_dataWords;
      MictpData                   m_mictpData;
      unsigned int                m_CTPDataWord;
      EventID                     m_eventID;
      mutable MsgLogger           m_logger;

   }; // class MictpModule

} // namespace LVL1MUCTPI

#endif  // TRIGT1MUCTPI_MICTPMODULE_H
