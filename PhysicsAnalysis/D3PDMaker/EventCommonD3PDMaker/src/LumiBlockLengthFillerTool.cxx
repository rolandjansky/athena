/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LumiBlockLengthFillerTool.cxx 542661 2013-03-27 00:35:13Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// DB include(s):
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// Local include(s):
#include "LumiBlockLengthFillerTool.h"

namespace D3PD {

   LumiBlockLengthFillerTool::
   LumiBlockLengthFillerTool( const std::string& type, const std::string& name,
                              const IInterface* parent )
      : BlockFillerTool< void >( type, name, parent ),
        m_lbLength( 0 ),
        m_lbLengthTransient( -1.0 ) {

      declareProperty( "FolderName", m_folderName = "/TRIGGER/LUMI/LBLB",
                       "Name of the COOL folder to read the info from" );
   }

   StatusCode LumiBlockLengthFillerTool::initialize() {

      // Reset the stored value:
      m_lbLengthTransient = -1.0;

      // Register the callback to the appropriate folder:
      const DataHandle< AthenaAttributeList > aptr;
      CHECK( detStore()->regFcn( &LumiBlockLengthFillerTool::update, this,
                                 aptr, m_folderName, true ) );

      // Tell the user what we'll do:
      ATH_MSG_INFO( "Will read the luminosity block length info from folder: "
                    << m_folderName );

      return StatusCode::SUCCESS;
   }

   StatusCode LumiBlockLengthFillerTool::book() {

      // Book the only variable of the tool:
      CHECK( addVariable( "lbLength", m_lbLength,
                          "Length of the current luminosity block in "
                          "seconds" ) );

      return StatusCode::SUCCESS;
   }

   StatusCode LumiBlockLengthFillerTool::fill() {

      // Fill the variable with the stored value:
      *m_lbLength = m_lbLengthTransient;

      return StatusCode::SUCCESS;
   }

   /**
    * This function simply follows the structure described for the folder on:
    *
    * https://twiki.cern.ch/twiki/bin/viewauth/Atlas/CoolOnlineData#Folder_TRIGGER_LUMI_LBLB
    *
    * The luminosity block info is stored as a start and end time in the number
    * of nanoseconds elapsed since 1970. This is turned into a time difference
    * expressed in seconds.
    */
   StatusCode
   LumiBlockLengthFillerTool::update( IOVSVC_CALLBACK_ARGS ) {

      // Let the user know what we're doing:
      ATH_MSG_DEBUG( "Updating the luminosity block length info" );

      // Read the contents of the folder:
      const DataHandle< AthenaAttributeList > lblb;
      CHECK( detStore()->retrieve( lblb, m_folderName ) );

      // Read the contents:
      const unsigned long long start =
         ( *lblb )[ "StartTime" ].data< unsigned long long >();
      const unsigned long long end =
         ( *lblb )[ "EndTime" ].data< unsigned long long >();
      if( msgLvl( MSG::VERBOSE ) ) {
         REPORT_MESSAGE( MSG::VERBOSE ) << "StartTime = " << start;
         REPORT_MESSAGE( MSG::VERBOSE ) << "EndTime   = " << end;
      }

      // Turn the start and end times into a time length in seconds.
      // Notice that we can't use the m_lbLength variable here, as it may
      // not have been set up the first time when this function is called.
      m_lbLengthTransient =
         static_cast< float >( static_cast< double >( end - start ) * 1e-9 );

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
