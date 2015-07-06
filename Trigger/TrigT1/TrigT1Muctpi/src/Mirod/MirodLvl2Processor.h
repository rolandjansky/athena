// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MirodLvl2Processor.h 515239 2012-08-28 11:39:45Z krasznaa $
#ifndef TRIGT1MUCTPI_MIRODLVL2PROCESSOR_H
#define TRIGT1MUCTPI_MIRODLVL2PROCESSOR_H

// STL include(s):
#include <string>
#include <list>
#include <vector>

// Local include(s):
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class MirodExtractor;
   class Configuration;

   /**
    *  @short implements the MIROD Lvl2 processor
    *
    *         This class implements the functionality of the MIROD
    *         Lvl2 processor. It receives its data from the
    *         MIROD extractor and chooses the ones that have passed
    *         the programmable thresholds for candiates in the
    *         Extractor. The candidates or sorted and the total
    *         number of candidates and the number of candidates per
    *         pt value is limited
    *
    *    @see MirodModule
    *    @see MirodDaqProcessor
    * @author Thorsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 515239 $
    * $Date: 2012-08-28 13:39:45 +0200 (Tue, 28 Aug 2012) $
    */
   class MirodLvl2Processor {

   public:
      /**
       * The default constructor of MirodLvl2Processor
       */
      MirodLvl2Processor();
      /**
       * Alternative constructor of MirodLvl2Processor which has
       * the purpose of creating an object with setting the Lvl2
       * out data to an existing std::list of unsigned ints - e.g.
       * using the output of the hardware - to then utilize the
       * analysis methods of this class
       */
      MirodLvl2Processor( std::list< unsigned int > lvl2Out );

      // set Configuration
      void setConfiguration( const Configuration& conf );

      /**
       * access to the output result
       * return The Lvl2 Output Data to the RoIB
       */
      const std::list< unsigned int >& getLvl2OutputData() const { return m_lvl2OutputData; }
      /**
       * This is the main action interface of the MirodLvl2Processor,
       * gets a link to the data output of the MirodExtractor. Calling
       * this method fills the STL list which holds the output of the
       * Lvl2Processor in the format used in the hardware, e.g. a
       * stream of 32-bit words
       * @see MirodModule
       * @see MirodExtractor
       * @see MirodLvl2Processor
       * @param theExtractorData Pointer to the data output of the
       *                         Mirod Extractor
       */
      void processData( const MirodExtractor* theExtractorData );
      /**
       * This method prints the Event in DaqOutput format to the
       * screen. The parameter determines if the printout will be in
       * binary, hexadecimal or human readable format. Consequently the
       * choices are "hex", "bin", or "human"
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
      mutable MsgLogger m_logger;

      std::list< unsigned int >  m_lvl2OutputData;

      /// Buckets used for sorting the candidates by pT
      std::vector< std::vector< unsigned int > > m_ptSorterBuckets;

      std::list< unsigned int >  m_ptCombinedFiFo;

      // configuration parameters
      unsigned int m_maxCandPerPtvalue;
      unsigned int m_maxCandSendToRoib;
      unsigned int m_candBcidOffset;

   }; // class MirodLvl2Processor

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIRODLVL2PROCESSOR_H
