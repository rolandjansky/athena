// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MirodEventReader.h 439423 2011-05-25 21:22:40Z krasznaa $
#ifndef TRIGT1MUCTPI_MIRODEVENTREADER_H
#define TRIGT1MUCTPI_MIRODEVENTREADER_H

// STL include(s):
#include <list>
#include <string>

// Local include(s):
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class MibakStreamEvent;

   /**
    *  @short This class implements the MIROD Event Reader
    *
    *         This class implements the functionality of the
    *         Event Reader of the MIROD Readout Driver. The data
    *         arrives in form of a pointer to a MibakStreamEvent
    *         and is checked and separated into candidate and
    *         event stream as on the actual board.
    *
    *    @see MirodModule
    *    @see MibakStreamEvent
    * @author Thorsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 439423 $
    * $Date: 2011-05-25 23:22:40 +0200 (Wed, 25 May 2011) $
    */
   class MirodEventReader {

   public:
      /**
       * constructor of MirodEventReader
       */
      MirodEventReader();
      /**
       * This method returns a pointer to the EventFiFo list.
       * @return Pointer to the list representing the
       *         EventFifo
       */
      const std::list< unsigned int >* getEventFifo() const { return &m_eventFiFo; }
      /**
       * This method returns a pointer to the CandidateFiFo lower bits.
       * @return Pointer to the list representing the
       *         Candidate Fifo's lower 32 bits
       */
      const std::list< unsigned int >* getCandidateFifo() const { return &m_candidateFiFo; }
      /**
       * This method returns a pointer to the CandidateFiFo higher bits.
       * @return Pointer to the list representing the
       *         Candidate Fifo's higher 9 bits
       */
      const std::list< unsigned int >* getCandidateFifoHighBits() const {
         return &m_candidateFiFoHighBits;
      }
      /**
       * This method reads the data from the input MibakDataStream and
       * splits it into an Event and a Candidate Stream in the format
       * used by the EventReader FPGA on the MIROD Board.
       */
      void readData( const MibakStreamEvent* theStream );
      /**
       * This method prints the EventFiFo and the Candidate FiFo to the
       * screen. The parameter determines if the printout will be in
       * binary, hexadecimal or human radable format. Consequently the
       * choices are "hex", "bin" or "human"
       * @param format Determines if the printout will be in
       *               binary or hexadecimal format. Choices are "hex", "bin", "human"
       */
      void printEvent( std::string format ) const;

   private:
      mutable MsgLogger m_logger;

      std::list< unsigned int > m_eventFiFo;
      std::list< unsigned int > m_candidateFiFo;
      std::list< unsigned int > m_candidateFiFoHighBits;

   }; // class MirodEventReader

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIRODEVENTREADER_H
