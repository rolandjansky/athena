/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Framework include(s):
#include "PathResolver/PathResolver.h"

// Local include(s):
#include "GoodRunsLists/GoodRunsListSelectionTool.h"
#include "GoodRunsLists/TGoodRunsListReader.h"
#include "GoodRunsLists/TMsgLogger.h"

GoodRunsListSelectionTool::GoodRunsListSelectionTool( const std::string& name ) 
   : asg::AsgTool( name ) {

   declareProperty( "GoodRunsListVec", m_goodrunslistVec );
   declareProperty( "BlackRunsListVec", m_blackrunslistVec );

   declareProperty( "BoolOperation", m_boolop = 0 );
   declareProperty( "PassThrough", m_passthrough = true );
   declareProperty( "RejectBlackRunsInEventSelector", m_rejectanybrl = false );
}

StatusCode GoodRunsListSelectionTool::initialize() {

   // Tell the user what's happening:
   ATH_MSG_DEBUG( "Initialising tool" );

   // Set the output level of the underlying code:
   const Root::TMsgLevel level =
         static_cast< Root::TMsgLevel >( msg().level() );
   Root::TMsgLogger::SetMinLevel( level );

   // Reset the pass-through mode setting:
   if( m_goodrunslistVec.size() || m_blackrunslistVec.size() ) {
      m_passthrough = false;
   }

   // Warn about pass-through mode:
   if( m_passthrough ) {
      ATH_MSG_WARNING( "Set to pass-through mode." );
   }

   // Read in the XML files:
   ATH_CHECK( readXMLs( m_grlcollection, m_goodrunslistVec ) );
   ATH_CHECK( readXMLs( m_brlcollection, m_blackrunslistVec ) );

   // Return gracefully:
   return StatusCode::SUCCESS;
}

bool GoodRunsListSelectionTool::
passRunLB( const std::vector< std::string >& grlnameVec,
           const std::vector< std::string >& brlnameVec ) const {

   // Retrieve the xAOD::EventInfo object:
   const xAOD::EventInfo* ei = 0;
   if( ! evtStore()->retrieve( ei, "EventInfo" ).isSuccess() ) {
      ATH_MSG_WARNING( "Couldn't access xAOD::EventInfo object. Using "
                       "pass-through mode..." );
      return true;
   }

   // MC events should not be filtered by this tool:
   if( ei->eventType( xAOD::EventInfo::IS_SIMULATION ) ) {
      ATH_MSG_VERBOSE( "MC event is accepted by the tool" );
      return true;
   }

   // Use this object:
   return passRunLB( *ei, grlnameVec, brlnameVec );
}

bool GoodRunsListSelectionTool::
passRunLB( const xAOD::EventInfo& event,
           const std::vector< std::string >& grlnameVec,
           const std::vector< std::string >& brlnameVec ) const {

   return passRunLB( event.runNumber(), event.lumiBlock(),
                     grlnameVec, brlnameVec );
}


bool GoodRunsListSelectionTool::
passRunLB( int runNumber, int lumiBlockNr,
           const std::vector< std::string >& grlnameVec,
           const std::vector< std::string >& brlnameVec ) const {

   ATH_MSG_VERBOSE( "passRunLB()" );

   // pass through
   if( m_passthrough ) {
      ATH_MSG_VERBOSE( "passRunLB() :: Pass through mode." );
      return true;
   }

   // decision based on merged blackrunslist
   if( m_rejectanybrl &&
       m_brlcollection.HasRunLumiBlock( runNumber, lumiBlockNr ) ) {
      ATH_MSG_VERBOSE( "passRunLB() :: Event rejected by (_any_ of) merged "
                       "black runs list." );
      return false;
   }

   // decision based on specific blackrunlists
   if( ( ! m_rejectanybrl ) && brlnameVec.size() ) {
      for( const std::string& brlname : brlnameVec ) {
         auto brl_itr = m_brlcollection.find( brlname );
         if( brl_itr == m_brlcollection.end() ) {
            continue;
         }
         if( brl_itr->HasRunLumiBlock( runNumber, lumiBlockNr ) ) {
            ATH_MSG_VERBOSE( "passRunLB() :: Event rejected by specific ("
                             << brlname << ") black runs list." );
            return false;
         }
      }
   }

   // decision based on specific goodrunlists
   for( const std::string& grlname : grlnameVec ) {
      auto grl_itr = m_grlcollection.find( grlname );
      if( grl_itr == m_grlcollection.end() ) {
         continue;
      }
      if( grl_itr->HasRunLumiBlock( runNumber, lumiBlockNr ) ) {
         ATH_MSG_VERBOSE( "passRunLB() :: Event accepted by specific ("
                          << grlname << ") good runs list." );
         return true;
      }
   }

   // If we got this far, the decision needs to be made based on the combined
   // GRL:
   return m_grlcollection.HasRunLumiBlock( runNumber, lumiBlockNr );
}

StatusCode
GoodRunsListSelectionTool::readXMLs( Root::TGRLCollection& grl,
                                     const std::vector< std::string >& files ) {

   // If there are no files specified, then let's just clean the GRL object,
   // and return:
   if( files.empty() ) {
      grl.clear();
      return StatusCode::SUCCESS;
   }

   // The reader object used:
   Root::TGoodRunsListReader reader;

   // Set up the files:
   for( const std::string& fname : files ) {

      // Find the file:
      std::string fileName = PathResolverFindXMLFile( fname );
      if( fileName == "" ) {
	//try with CalibArea
	fileName = PathResolverFindCalibFile( fname );
	if(fileName=="") {
         ATH_MSG_FATAL( "Couldn't find file: " << fname );
         return StatusCode::FAILURE;
	}
      }
      ATH_MSG_DEBUG( "Reading file: " << fileName << " (" << fname << ")" );

      // Add it to the reader:
      reader.AddXMLFile( fileName );
   }

   // (Try to) Interpret all the XML files:
   if( ! reader.Interpret() ) {
      ATH_MSG_ERROR( "There was an error parsing the GRL XML file(s)" );
      return StatusCode::FAILURE;
   }

   // Merge the GRLs into one:
   const Root::BoolOperation op =
         static_cast< Root::BoolOperation >( m_boolop );
   grl = reader.GetMergedGRLCollection( op );

   // Return gracefully:
   return StatusCode::SUCCESS;
}

#ifndef XAOD_STANDALONE
bool GoodRunsListSelectionTool::eventPassesFilter() const {
  const xAOD::EventInfo* ei = 0;
  if( evtStore()->retrieve( ei , "EventInfo" ).isFailure() ) {
    ATH_MSG_ERROR("Unable to retrieve EventInfo, returning false");
    return false;
  }
  return passRunLB( *ei );
}
#endif
