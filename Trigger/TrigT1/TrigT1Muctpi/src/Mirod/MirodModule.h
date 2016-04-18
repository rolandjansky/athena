// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MirodModule.h 726107 2016-02-25 11:04:42Z wengler $
#ifndef TRIGT1MUCTPI_MIRODMODULE_H
#define TRIGT1MUCTPI_MIRODMODULE_H

// STL include(s):
#include <list>

// Local include(s):
#include "../Logging/MsgLogger.h"
#include "MirodEventReader.h"
#include "MirodExtractor.h"
#include "MirodDaqProcessor.h"
#include "MirodLvl2Processor.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class Configuration;
   class MibakStreamEvent;

   /**
    *  @short Contains all relevant entities of the MIROD board
    *
    *         The MirodModule class groups together all relevant
    *         entities for the Read Out Driver board of the
    *         MUCTPI (MIROD). It provides services to extract data
    *         at several stages of the processing in the board,
    *         corresponding to equivalent access point on the
    *         the actual hardware. Only one board exists in the
    *         system. As for the MioctModules, this Module is
    *         instantiated in the constructor of the Lvl1Mibak
    *         class
    *
    *    @see Lvl1Mibak
    *    @see MioctModule
    *    @see MictpModule
    *
    * @author Thorsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 726107 $
    * $Date: 2016-02-25 12:04:42 +0100 (Thu, 25 Feb 2016) $
    */
   class MirodModule {

   public:
      /**
       * The constructor of MirodModule
       */
      MirodModule();

      /// set Configuration
      void setConfiguration( const Configuration &conf );

      /**
       * Method to set the source of data for the input stream coming
       * from the MIBAK
       * @see MibakStreamEvent
       * @param theStream Pointer to the input data stream
       *                  object holding the data to be read by the MIROD
       */
      void processData( const MibakStreamEvent* theStream );
      /**
       * This method returns a pointer to the MirodEventReader
       * @return Pointer to the MirodEventReader
       *         which is part of the MirodModule
       */
      const MirodEventReader* getMirodEventReader() const { return &m_mirodReader; }
      /**
       * This method returns a pointer to the MirodExtractor
       * @return Pointer to the MirodExtractor
       *         which is part of the MirodModule
       */
      const MirodExtractor* getMirodExtractor() const { return &m_mirodExtractor; }
      /**
       * This method returns a pointer to the MirodLvl2Processor
       * @return Pointer to the MirodLvl2Processor
       *         which is part of the MirodModule
       */
      const MirodLvl2Processor* getMirodLvl2Processor() const { return &m_mirodLvl2Processor; }
      /**
       * This method returns a reference to the list of unsigned ints holding
       * the output of the Mirod to the RoIB (Level2), candidates only, wihtout header or trailer words
       * @return The LVL2 output candidates
       */
      const std::list< unsigned int >& getLvl2Candidates() const { return m_mirodLvl2Processor.getLvl2OutputCandidates(); }
      /**
       * This method returns a reference to the list of unsigned ints holding
       * the output of the Mirod to the RoIB (Level2)
       * @return The LVL2 output
       */
      const std::list< unsigned int >& getLvl2Output() const { return m_mirodLvl2Processor.getLvl2OutputData(); }
      /**
       * This method returns a pointer to the MirodDaqProcessor
       * @return Pointer to the MirodDaqProcessor
       *         which is part of the MirodModule
       */
      const MirodDaqProcessor* getMirodDaqProcessor() const { return &m_mirodDaqProcessor; }
      /**
       * This method reyrn a reference to the list of unsigned ints holding
       * the output of the Mirod to the ROB (DAQ)
       * @return The DAQ output
       */
      const std::list< unsigned int >& getDaqOutput() const { return m_mirodDaqProcessor.getDaqOutputData(); }

   private:
      mutable MsgLogger  m_logger;
      MirodEventReader   m_mirodReader;
      MirodExtractor     m_mirodExtractor;
      MirodDaqProcessor  m_mirodDaqProcessor;
      MirodLvl2Processor m_mirodLvl2Processor;

   }; // class MirodModule

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIRODMODULE_H
