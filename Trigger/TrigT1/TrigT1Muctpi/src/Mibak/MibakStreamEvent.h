// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MibakStreamEvent.h 364083 2011-05-06 09:09:55Z krasznaa $
#ifndef TRIGT1MUCTPI_MIBAKSTREAMEVENT_H
#define TRIGT1MUCTPI_MIBAKSTREAMEVENT_H

// STL include(s):
#include <list>
#include <string>

// Local include(s):
#include "../Logging/MsgLogger.h"

// Forward declaration(s):
class MirodTestRamIn;

namespace LVL1MUCTPI {

   /**
    *  @short Class containing one event in MIBAK format
    *
    *         This class constitutes the interface in the software
    *         to the MIORD read out driver. It contains all
    *         information of the event in the format found on the
    *         the MIBAK. It is filled by method getMibakData() in
    *         class Lvl1Mibak, which takes as an argument whether
    *         or not there should be zero suppression.
    *
    *    @see Lvl1Mibak
    * @author Thorsten Wengler
    *
    * $Revision: 364083 $
    * $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $
    */
   class MibakStreamEvent {

   public:
      /**
       * since this is just a data container the data members of this
       * class are exceptionally public
       */
      std::list< unsigned int > highBits;
      std::list< unsigned int > dataWords;
      /**
       * The constructor of MibakStreamEvent
       */
      MibakStreamEvent();
      /**
       * This method prints the event to the screen. The parameter
       * determines if the printout will be in binary, hexadecimal
       * or human readable format. Consequently the choices are
       * "hex", "bin" or "human"
       * @param format Determines if the printout will be in
       *        binary or hexadecimal format. Choices are "hex", "bin", "human"
       */
      void printEvent( const std::string& format ) const;
      /**
       * Service which allows to set the data in this class directly, i.e.
       * feed the datastream into the system at this point. Since in normal
       * operation part of the data consists of pointers to previous
       * processing, another list is needed to store the data words in this
       * case.
       * @param inputHighBits list of the data words
       * @param inputDataWords list of the data words
       */
      void setEvent( const std::list< unsigned int >& inputHighBits,
                     const std::list< unsigned int >& inputDataWords );
      /**
       * method to read event from file. The format of the file is identical
       * to the one expected by the MIROD testram controller, since the
       * main use of this function is to read such files
       * @param fileName name of the file to read
       */
      void readFromFile( const std::string& fileName );
      /**
       * Get the test ram input for the Mirod in object format for
       * easier handling - some data copying out of lazyness, but
       * speed is unimportant for this use case.
       */
      MirodTestRamIn getMirodTestRamInput() const;
      /**
       * This method returns a pointer to the list MirodTestDataLow
       */
      const std::list< unsigned int >* getMirodTestDataLow() const { return &m_mirodTestDataLow; }
      /**
       * This method returns a pointer to the list MirodTestDataLow
       */
      const std::list< unsigned int >* getMirodTestDataHigh() const { return &m_mirodTestDataHigh; }
      /**
       * This method returns a pointer to the list MirodTestDataLow
       */
      const std::list< unsigned int >* getMirodTestControl() const { return &m_mirodTestControl; }
      /**
       * this method prints the data prepared in the prepareMiordTestRamInput
       * method to either the screen or two files
       * @param fileName either "screen" for terminal output or filename root,
       *                 which is used to for the two files root_data.dat and root_ctrl.dat
       */
      void printMirodTestRamInput( const std::string& fileName );

   private:
      mutable MsgLogger                  m_logger;
      //
      // The mirod test memory data members are declared mutable, as it should be allowed to
      // synchronize them to the "highBits" and "dataWords" data members even when only having
      // a constant reference to the object.
      //
      mutable std::list< unsigned int >  m_mirodTestDataLow;
      mutable std::list< unsigned int >  m_mirodTestDataHigh;
      mutable std::list< unsigned int >  m_mirodTestControl;

      /**
       * This method fills two lists in a format suitable for loading the
       * event into the MIROD testram. The hardware test library expects
       * the lower bits to be 0-23 , and the higher bits above that , so
       * the lists are constructed accordingly. Another list holds the
       * corresponding control words emulating the MIBAK controls in the
       * the testram
       */
      void prepareMirodTestRamInput() const;

   }; // class MibakStreamEvent

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MIBAKSTREAMEVENT_H
