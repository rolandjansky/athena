/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMissingETBaseFillerTool.cxx 502729 2012-05-27 21:54:40Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigMissingEtEvent/TrigMissingET.h"

// Local include(s):
#include "TrigMissingETBaseFillerTool.h"

namespace D3PD {

   TrigMissingETBaseFillerTool::TrigMissingETBaseFillerTool( const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent )
      : BlockFillerTool< TrigMissingETContainer >( type, name, parent )
   {
     book().ignore(); // Avoid coverity warnings.
   }

   /**
    * Here the output variables are declared to the D3PDMaker framework.
    */
   StatusCode TrigMissingETBaseFillerTool::book() {

      CHECK( addVariable( "MEx",   m_ex ) );
      CHECK( addVariable( "MEy",   m_ey ) );
      CHECK( addVariable( "MEz",   m_ez ) );
      CHECK( addVariable( "sumEt", m_sumEt ) );
      CHECK( addVariable( "sumE",  m_sumE ) );

      CHECK( addVariable( "flag",  m_flag ) );

      return StatusCode::SUCCESS;
   }

   /**
    * Here the output variables are filled with the information coming from a
    * TrigMissingET object.
    */
   StatusCode TrigMissingETBaseFillerTool::fill( const TrigMissingETContainer& met ) {

      // Fill the variables with dummy values:
      *m_ex    = -99999999.9;
      *m_ey    = -99999999.9;
      *m_ez    = -99999999.9;
      *m_sumEt = -99999999.9;
      *m_sumE  = -99999999.9;

      *m_flag  = 0x80000000;

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
      *m_ex    = met.front()->ex();
      *m_ey    = met.front()->ey();
      *m_ez    = met.front()->ez();
      *m_sumEt = met.front()->sumEt();
      *m_sumE  = met.front()->sumE();

      *m_flag  = met.front()->getFlag();

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
