/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMissingETComponentFillerTool.cxx 502729 2012-05-27 21:54:40Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigMissingEtEvent/TrigMissingET.h"

// Local include(s):
#include "TrigMissingETComponentFillerTool.h"

namespace D3PD {

   TrigMissingETComponentFillerTool::TrigMissingETComponentFillerTool( const std::string& type,
                                                                       const std::string& name,
                                                                       const IInterface* parent )
      : BlockFillerTool< TrigMissingETContainer >( type, name, parent )
   {
     book().ignore(); // Avoid coverity warnings.
   }

   /**
    * Here the output variables are declared to the D3PDMaker framework.
    */
   StatusCode TrigMissingETComponentFillerTool::book() {

      CHECK( addVariable( "nameOfComponent", m_name ) );
      CHECK( addVariable( "MExComponent",    m_ex ) );
      CHECK( addVariable( "MEyComponent",    m_ey ) );
      CHECK( addVariable( "MEzComponent",    m_ez ) );
      CHECK( addVariable( "sumEtComponent",  m_sumEt ) );
      CHECK( addVariable( "sumEComponent",   m_sumE ) );
      CHECK( addVariable( "componentCalib0", m_calib0 ) );
      CHECK( addVariable( "componentCalib1", m_calib1 ) );
      CHECK( addVariable( "sumOfSigns",      m_sumOfSigns ) );
      CHECK( addVariable( "usedChannels",    m_usedChannels ) );
      CHECK( addVariable( "status",          m_status ) );

      return StatusCode::SUCCESS;
   }

   /**
    * Here the output variables are filled with the information coming from a
    * TrigMissingET object.
    */
   StatusCode TrigMissingETComponentFillerTool::fill( const TrigMissingETContainer& met ) {

      // Clear the vectors:
      m_name->clear();
      m_ex->clear();
      m_ey->clear();
      m_ez->clear();
      m_sumEt->clear();
      m_sumE->clear();
      m_calib0->clear();
      m_calib1->clear();
      m_sumOfSigns->clear();
      m_usedChannels->clear();
      m_status->clear();

      // If the container is empty, leave the dummy values in the variables:
      if( ! met.size() ) {
         REPORT_MESSAGE( MSG::DEBUG ) << "TrigMET container is empty!";
         return StatusCode::SUCCESS;
      }

      // Issue a warning if there are more than 1 objects in the container.
      // This should not happen if the HLT is working correctly.
      if( met.size() > 1 ) {
         REPORT_MESSAGE( MSG::WARNING ) << "TrigMET container has multiple objects!";
         REPORT_MESSAGE( MSG::WARNING ) << "Check with trigger experts!";
      }

      // Fill the variables with the correct values:
      for( unsigned int i = 0; i < met.front()->getNumOfComponents(); ++i ) {

         m_name->push_back( met.front()->getNameOfComponent( i ) );
         m_ex->push_back( met.front()->getExComponent( i ) );
         m_ey->push_back( met.front()->getEyComponent( i ) );
         m_ez->push_back( met.front()->getEzComponent( i ) );
         m_sumEt->push_back( met.front()->getSumEtComponent( i ) );
         m_sumE->push_back( met.front()->getSumEComponent( i ) );
         m_calib0->push_back( met.front()->getComponentCalib0( i ) );
         m_calib1->push_back( met.front()->getComponentCalib1( i ) );
         m_sumOfSigns->push_back( met.front()->getSumOfSigns( i ) );
         m_usedChannels->push_back( met.front()->getUsedChannels( i ) );
         m_status->push_back( met.front()->getStatus( i ) );

      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
