// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MirodExtractor.h 439423 2011-05-25 21:22:40Z krasznaa $
#ifndef TRIGT1MUCTPI_MIRODEXTRACTOR_H
#define TRIGT1MUCTPI_MIRODEXTRACTOR_H

// STL include(s):
#include <list>
#include <string>

// Local include(s):
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class Configuration;
   class MirodEventReader;

   /**
    *  @short This class implements the Mirod Extractor
    *
    *         This class implements the fuunctionality of the MIROD
    *         Extractor. Its receives the data from the MIROD
    *         Event Reader, which in turn has read them from the
    *         Lvl1 Mibak backplane. The data are handed over in form
    *         of pointers to the Event and Candidate Fifo's, which
    *         are implemented as STL lists. The result of the
    *         processing done in the Extractor is stored again in
    *         an STL list of unsigned int words, which in format
    *         corrspond to the Extractor format of the actual MIROD.
    *
    *         The Extractor compares each Muon candidates to
    *         programmable thresholds.
    *
    *    @see MiordModule
    *    @see MirodEventReader
    * @author Thorsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 439423 $
    * $Date: 2011-05-25 23:22:40 +0200 (Wed, 25 May 2011) $
    */
   class MirodExtractor {

   public:
      /**
       * The constructor of MirodExtractor
       */
      MirodExtractor();
      /**
       * Alternative constructor of MirodExtractor which has
       * the purpose of creating an object with setting the Extractor
       * out data to an existing list of unsigned ints - e.g.
       * using the output of the hardware - to then utilize the
       * analysis methods of this class
       */
      MirodExtractor( std::list<unsigned int> extractorOut );

      // set Configuration
      void setConfiguration( const Configuration &conf );

      /**
       * This is the main action interface of the MirodExtractor, which
       * gets links from the MirodEventReader and processes the data
       * the Extractor format which is used on the actual board and
       * is expected by the Lvl2Processor and the DaqProcessor. The
       * result is again stored in an STL list
       * @see MirodEventReader
       * @see MirodLvl2Processor
       * @see MirodDaqProcessor
       */
      void processData( const MirodEventReader* theReader );
      /**
       * This method takes one input word from the candidate buffer
       * (data word and high bits holding the sector ID) and
       * extracts the first candidate if any. The return is a candiate
       * word in Extractor format
       * @return First candidate in Extractor format
       * @param candidateFiFo candidate word bit 0-31
       * @param candidateFiFoHighBits candidate word high bits 32-40
       */
      unsigned int extractFirstCandidate( const unsigned int candidateFiFo,
                                          const unsigned int candidateFiFoHighBits );
      /**
       * This method takes one input word from the candidate buffer
       * (data word and high bits holding the sector ID) and
       * extracts the second candidate if any. The return is a candiate
       * word in Extractor format
       * @return Second candidate in Extractor format
       * @param candidateFiFo candidate word bit 0-31
       * @param candidateFiFoHighBits candidate word high bits 32-40
       */
      unsigned int extractSecondCandidate( const unsigned int candidateFiFo,
                                           const unsigned int candidateFiFoHighBits );
      /**
       * This method hands back a Pointer to the list holding the
       * output data of the Extractor
       * @return Pointer to the list of output data
       *         in Mirod Extractor format
       */
      const std::list< unsigned int >* getExtractorData() const { return & m_extractorData; }
      /**
       * This method prints the Event in Extractor format to the
       * screen. The parameter determines if the printout will be in
       * binary, hexadecimal or human readable format.
       * Consequently the choices are "hex", "bin" or "human"
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
      static std::string printSector( const unsigned int & sectorWord );

   private:
      mutable MsgLogger m_logger;

      std::list< unsigned int > m_extractorData;

      // threshold parameters
      unsigned int m_threshold1Candidate;
      unsigned int m_threshold2Candidate;

      /// suppression mode
      int m_suppressionMode;

   }; // class MirodExtractor

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIRODEXTRACTOR_H
