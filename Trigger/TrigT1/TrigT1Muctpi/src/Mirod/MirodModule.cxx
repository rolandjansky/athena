/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MirodModule.cxx 439303 2011-05-25 15:20:17Z krasznaa $

// Local include(s):
#include "MirodModule.h"
#include "../Mibak/MibakStreamEvent.h"
#include "../Common/Configuration.h"

/*******************************************************************
 * $Date: 2011-05-25 17:20:17 +0200 (Wed, 25 May 2011) $
 *
 * Implementation of class MirodModule
 * @author  $Author: krasznaa $
 * @version $Revision: 439303 $
 ******************************************************************/

namespace LVL1MUCTPI {

   // constructor
   MirodModule::MirodModule()
      : m_logger( "MirodModule" ) {

      REPORT_VERBOSE_MSG( "constuctor called" );
   }

   void MirodModule::processData( const MibakStreamEvent* theStream ) {

      REPORT_VERBOSE_MSG( "Mirod module processing data" );

      m_mirodReader.readData( theStream );
      m_mirodReader.printEvent( "human" );
      m_mirodExtractor.processData( this->getMirodEventReader() );
      m_mirodExtractor.printEvent( "human" );
      m_mirodDaqProcessor.processData( this->getMirodExtractor() );
      m_mirodDaqProcessor.printEvent( "human" );
      m_mirodLvl2Processor.processData( this->getMirodExtractor() );
      m_mirodLvl2Processor.printEvent( "human" );

      return;
   }

   void MirodModule::setConfiguration( const Configuration& conf ) {

      REPORT_VERBOSE_MSG( "Configuring Extractor and LVL2 processor" );

      // configure Extractor
      m_mirodExtractor.setConfiguration( conf );

      // configure L2Processor
      m_mirodLvl2Processor.setConfiguration( conf );

      return;
   }

} // namespace LVL1MUCTPI
