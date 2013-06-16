// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MirodDaqProcessor.h 439423 2011-05-25 21:22:40Z krasznaa $
#ifndef TRIGT1MUCTPI_MIRODDAQPROCESSOR_H
#define TRIGT1MUCTPI_MIRODDAQPROCESSOR_H

// STL include(s):
#include <list>
#include <string>

// Local include(s):
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class MirodExtractor;

   /**
    *  @short funtionality of the MIROD DAQ Processor
    *
    *         Class MirodDaqProcessor implements the functionality
    *         of the MIROD DAQ Processor, which reads the data
    *         provided by the Extractor and formats them to the
    *         format used for driving out data to the Read Out
    *         system. This is the final step of the processing in
    *         the MIROD for the DAQ branch of the data flow. The
    *         data is provided as pointer to a list of 0-31 bit wide
    *         unsigned int. The SLINK control bits are of course not
    *         necessary here (would by bit 32). Otherwise the data
    *         format is exactly as for the hardware SLINK
    *
    *    @see MirodModule
    *    @see MirodLvl2Processor
    * @author Thorsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 439423 $
    * $Date: 2011-05-25 23:22:40 +0200 (Wed, 25 May 2011) $
    */
   class MirodDaqProcessor {

   public:
      /**
       * The constructor of MirodDaqProcessor
       */
      MirodDaqProcessor();
      /**
       * Alternative constructor of MirodDaqProcessor which has
       * the purpose of creating an object with setting the Daq
       * out data to an existing list of unsigned ints - e.g.
       * using the output of the hardware - to then utilize the
       * analysis methods of this class
       */
      MirodDaqProcessor( std::list< unsigned int > daqOut );
      /**
       * access to the output result
       * return The Daq Output Data Output to the ROS
       */
      const std::list< unsigned int >& getDaqOutputData() const { return m_daqOutputData; }
      /**
       * This is the main action interface of the MirodDaqProcessor,
       * gets a link to the data output of the MirodExtractor. Calling
       * this method fills the STL list which holds the output of the
       * DaqProcessor in the format used in the hardware, e.g. a stream
       * of 32-bit words
       * @see MirodModule
       * @see MirodExtractor
       * @see MirodLvl2Processor
       * @param theExtractor Poniter to the data output of the
       *                     Mirod Extractor
       */
      void processData( const MirodExtractor* theExtractor );
      /**
       * This method prints the Event in DaqOutput format to the
       * screen. The parameter determines if the printout will be in
       * binary, hexadecimal or human readable format. Consequently
       * the choices are "hex", "bin" or "human"
       * @param format Determines if the printout will be in
       *               binary or hexadecimal format. Choices are "hex", "bin", "human"
       */
      void printEvent( std::string format ) const;
      /**
       * function to return a string representing the information
       * contained in one sector word of the Extractor format
       * in human readable form
       * @param sectorWord reference to a sector word represented by
       *                   32 - bit Extractor format
       * @return A string to be used for output the information of
       *         this sector
       */
      std::string printSector( const unsigned int& sectorWord ) const;

   private:
      mutable MsgLogger         m_logger;
      std::list< unsigned int > m_daqOutputData;

   }; // class MirodDaqProcessor

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIRODDAQPROCESSOR_H
