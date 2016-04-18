// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuctpiSim.h 726107 2016-02-25 11:04:42Z wengler $
#ifndef TRIGT1MUCTPI_MUCTPISIM_H
#define TRIGT1MUCTPI_MUCTPISIM_H

// STL include(s):
#include <list>
#include <string>

// Gaudi/Athena include(s):
#include "GaudiKernel/StatusCode.h"

// Local include(s):
#include "../Logging/MsgLogger.h"
#include "../Mibak/Lvl1Mibak.h"
#include "../Mirod/MirodFiFoContent.h"
#include "../Mirod/MirodModule.h"
#include "../Mirod/MirodTestRamIn.h"
#include "../Mioct/StrategyName.h"
#include "../Mioct/MioctID.h"
#include "../Mictp/MictpModule.h"
#include "../Common/MioctL1TopoConverter.h"

namespace LVL1MUONIF {
  class Lvl1MuCTPIInput;
}
namespace LVL1MUCTPI {

   // Forward declaration(s):
   class Configuration;
   class EventReader;
   class SectorLogicSource;

   /*********************************************************************
    *
    *    $Date: 2016-02-25 12:04:42 +0100 (Thu, 25 Feb 2016) $
    *
    *    @short Top level class of the MUCTPI simulation
    *
    *           Top level class: The constructor sets up the simulation
    *           and defines the input source and output destination
    *           depending on its argument(s). The processData method
    *           performs the actual simulation in reading the input
    *           data, simulation the result and providing the output.
    *           Additional debugging and diagnostics interfaces
    *           provide access to all the modules of the MUCTPI
    *
    *      @see MirodModule
    *      @see MioctModule
    *      @see MictpModule
    *      @see Lvl1Mibak
    *      @see MibakStreamEvent
    *      @see MirodModule
    *   @author $Author: ssnyder $
    *  @version $Revision: 726107 $
    *
    *******************************************************************
    */
   class MuctpiSim {

   public:

      // Constructor
      /**
       * The constructor of MuctpiSim
       */
      MuctpiSim();

      // disable copy/assignment operator as not used/implemented
      MuctpiSim( const MuctpiSim & ) = delete;
      MuctpiSim & operator = ( const MuctpiSim & ) = delete;
     

      /// sets Configuration
      void setConfiguration( const Configuration& conf );

      /**
       * The processData method does the data processing in the Muctpi
       * It needs to be called before any of the output data is accessed,
       * which otherwise will be empty or still hold the last event.
       */
     void processData( const LVL1MUONIF::Lvl1MuCTPIInput* currentInput, int bcidOffset = 0 );
      /**
       * method to access the data send to the CTP.
       */
      unsigned int getCTPData()
      { return this->getMictpModule()->getMictpCTPWord(); }
      /**
       * method to access the candidates sent to the RoIB (Level 2 Trigger)
       * without the header or trailer words.
       */
      const std::list<unsigned int>& getRoIBCandidates()
      { return this->getMirodModule()->getLvl2Candidates(); }
      /**
       * method to access the data send to the RoIB (Level 2 Trigger).
       */
      const std::list<unsigned int>& getRoIBData()
      { return this->getMirodModule()->getLvl2Output(); }
      /**
       * method to access the data send to the DAQ system.
       */
      const std::list<unsigned int>& getDAQData()
      { return this->getMirodModule()->getDaqOutput(); }
      /**
       * setup the Muon to L1Topo converter
       */
      void setupL1TopoConverter(const std::string & inputfile){
	m_L1TopoConv.setupParser(inputfile);
      }
      /**
       * access the output for the L1Topo, and add the LVL2 style 32 bit word to it.
       */
      LVL1::MuCTPIL1Topo getL1TopoData();
      

      /// Is there at least one barrel candidate in the event?
      bool hasBarrelCandidate() const;
      /// Is there at least one endcap candidate in the event?
      bool hasEndcapCandidate() const;

      /**
       * method to access a MioctModule specified by its ID
       */
      const MioctModule* getMioct( MioctID id ) const
      { return m_mibak->getMioctModule( id ); }
      /**
       * method to access the MirodModule
       */
      const MirodModule* getMirodModule() { return m_mibak->getMirod(); }
      /**
       * method to access the MictpModule
       */
      const MictpModule* getMictpModule() { return m_mibak->getMictp(); }
      /**
       * method to access the data stream on the Mibak to the Mirod
       */
      const MibakStreamEvent* getMibakStreamData()
      { return m_mibak->getMibakStreamEvent(); }
      /**
       * get the input needed to load the MirodTestRam
       * @return object holding the input to the testram
       */
      MirodTestRamIn getMirodTestRamInput()
      { return this->getMibakStreamData()->getMirodTestRamInput(); }
      /**
       * method to get the content of the MirodFiFos as an object for
       * comparisons to the hardware
       */
      MirodFiFoContent getMirodFiFoContent();
      /**
       * print methods for the output of the MUCTPI. The parameter determines
       * if the output is in "bin"-binary, "hex"-hexadecimal, or
       * "human"-interpreted format
       * @param format - determines the output format (bin,hex,human)
       */
      void printCTPOut( std::string format );
      void printLvl2Out( std::string format );
      void printDAQOut( std::string format );

      /**
       * These methods are used to set up the simulation's overlap handling scheme.
       * Currently 2 strategies are supported: NO_OVERLAP and LUT_OVERLAP
       */
      void setOverlapStrategy( StrategyName strategy ) {
         m_mibak->setOverlapStrategy( strategy );
      }
      StatusCode initializeLUTOverlapStrategy( const std::string& lutXMLFile,
                                               bool flagMode, bool dumpLut,
					       const std::string& runPeriod = "RUN1" );

      /**
       * These methods are used to set up the simulation's multiplicity calculation
       * scheme. Currently 3 strategies are supported: INCLUSIVE, EXCLUSIVE, XML
       */
      void setMultiplicityStrategy( MultiplicityCalcType type ) {
         m_mibak->setMultiplicityStrategy( type );
      }
      void initializeXMLMultStrategy( const std::string& xmlFile ) {
         m_mibak->initializeXMLMultStrategy( xmlFile );
      }

      /**
       * The destructor of MuctpiSim
       */
      ~MuctpiSim();

   private:
      MsgLogger                    m_logger;
      EventID                      m_theEventID;
      EventReader*                 m_theReader;
      SectorLogicSource*           m_theSource;
      Lvl1Mibak*                   m_mibak;
      LVL1MUONIF::Lvl1MuCTPIInput* m_muctpiInput;
      MioctL1TopoConverter         m_L1TopoConv;

   }; // class MuctpiSim

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MUCTPISIM_H
