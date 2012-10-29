/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: D3PDBunchCrossingTool.cxx 502485 2012-05-25 13:02:53Z krasznaa $

// ROOT include(s):
#include <TROOT.h>
#include <TSeqCollection.h>
#include <TFile.h>
#include <TTree.h>

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/System.h"
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "D3PDBunchCrossingTool.h"
#include "TrigBunchCrossingTool/MsgWriter.h"

namespace Trig {

   D3PDBunchCrossingTool::D3PDBunchCrossingTool( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IBunchCrossingTool >( this );

      // Declare the properties of the tool:
      declareProperty( "MaxBunchSpacing", m_maxBunchSpacing = 150,
                       "Maximum bunch spacing in the trains in nanoseconds" );

      declareProperty( "FrontLength", m_frontLength = 300,
                       "Length of the front part of a train in nanoseconds" );
      declareProperty( "TailLength", m_tailLength = 300,
                       "Length of the tail part of a train in nanoseconds" );

      declareProperty( "ConfigID",
                       m_configID = SG::RVar< unsigned int >( "bunch_configID" ),
                       "Variable in the D3PD specifying the event's bunch "
                       "configuration identifier" );
   }

   StatusCode D3PDBunchCrossingTool::initialize() {

      // Configure the underlying logger:
      MsgWriter::instance()->setMinType( msg().level() );
      MsgWriter::instance()->setSource( name() );

      // Report about the initialization:
      ATH_MSG_INFO( "Initializing D3PDBunchCrossingTool - package version: "
                    << PACKAGE_VERSION );
      ATH_MSG_INFO( "  Maximal bunch spacing: " << m_maxBunchSpacing << " ns" );
      ATH_MSG_INFO( "  Length of train front: " << m_frontLength << " ns" );
      ATH_MSG_INFO( "  Length of train tail : " << m_tailLength << " ns" );

      // Configure the base class:
      setMaxBunchSpacing( m_maxBunchSpacing );
      setFrontLength( m_frontLength );
      setTailLength( m_tailLength );

      //
      // Set up the incidents that the tool should listen to:
      //
      ServiceHandle< IIncidentSvc > incidentSvc( "IncidentSvc", name() );
      CHECK( incidentSvc.retrieve() );
      incidentSvc->addListener( this, IncidentType::BeginEvent );

      //
      // Get the input file list. This is a bit ugly, but we need to get
      // the names of the input files and the name of the main TTree from
      // the event selector service. Later on there may be a less convoluted
      // access to the metadata trees, but right now there isn't.
      //
      m_files.clear();
      m_tupleName = "";
      ServiceHandle< IJobOptionsSvc > jobOSvc( "JobOptionsSvc", name() );
      CHECK( jobOSvc.retrieve() );
      const std::vector< const Property* >* evSelProp =
         jobOSvc->getProperties( "EventSelector" );
      if( ! evSelProp ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Didn't find the input file list";
         return StatusCode::FAILURE;
      }
      std::vector< const Property* >::const_iterator itr = evSelProp->begin();
      std::vector< const Property* >::const_iterator end = evSelProp->end();
      for( ; itr != end; ++itr ) {

         // Look for the "InputCollections" property:
         if( ( *itr )->name() == "InputCollections" ) {

            // Translate it to a list of file names:
            StringArrayProperty files;
            if( ! files.assign( **itr ) ) {
               REPORT_MESSAGE( MSG::FATAL )
                  << "The EventSelector.InputCollections property is of "
                  << "the wrong type";
               return StatusCode::FAILURE;
            }
            m_files = files.value();

         }
         // Also look for the "TupleName" property:
         else if( ( *itr )->name() == "TupleName" ) {

            // Translate to a simple string:
            StringProperty tupleName;
            if( ! tupleName.assign( **itr ) ) {
               REPORT_MESSAGE( MSG::FATAL )
                  << "The EventSelector.TupleName property is of "
                  << "the wrong type";
               return StatusCode::FAILURE;
            }
            m_tupleName = tupleName.value();

         }
      }
      if( ( ! m_files.size() ) || ( m_tupleName == "" ) ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "Couldn't access the needed jobOption properties";
         return StatusCode::FAILURE;
      }

      m_currentID = 0;

      return StatusCode::SUCCESS;
   }

   void D3PDBunchCrossingTool::handle( const Incident& inc ) {

      // Some debug information:
      ATH_MSG_DEBUG( "Incident received: type = " << inc.type()
                     << ", source = " << inc.source() );

      // Return right away if the configuration hasn't changed:
      if( *m_configID == m_currentID ) {
         return;
      }

      // Check if the configuration is available:
      if( ! isAvailable( *m_configID ) ) {

         // If not, let's try to access the input file, and read in the
         // configuration from the metadata tree.

         // Check if one of the input files is open at the moment.
         // Notice that we need to iterate through the file list
         // backwards. This is because ROOT/Athena doesn't close the first
         // file after it opened the second one, and so on. So, the current
         // file is always the last one from the list that's already open.
         TSeqCollection* files = gROOT->GetListOfFiles();
         std::vector< std::string >::const_reverse_iterator file_itr =
            m_files.rbegin();
         std::vector< std::string >::const_reverse_iterator file_end =
            m_files.rend();
         TFile* file = 0;
         for( ; file_itr != file_end; ++file_itr ) {
            file =
               dynamic_cast< TFile* >( files->FindObject( file_itr->c_str() ) );
            if( ! file ) continue;
            ATH_MSG_DEBUG( "Found open file: " << file->GetName() );
            break;
         }
         if( ! file ) {
            REPORT_MESSAGE( MSG::ERROR )
               << "Couldn't find any of the input files being open!";
            return;
         }

         // Find the metadata tree:
         TTree* metaTree =
            dynamic_cast< TTree* >( file->Get( ( m_tupleName +
                                                 "Meta/BunchConfTree" ).c_str() ) );
         if( ! metaTree ) {
            REPORT_MESSAGE( MSG::ERROR )
               << "Couldn't access the trigger configuration metadata tree in file: "
               << file->GetName();
            return;
         }

         // Cache the configuration from the input file:
         if( ! cacheConfig( metaTree, false ) ) {
            REPORT_MESSAGE( MSG::ERROR )
               << "There was a problem with caching the bunch configuration";
            return;
         }
      }

      // Finally, let's try to load the configuration:
      if( ! loadConfig( *m_configID ) ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "Couldn't load configuration with ID: " << *m_configID;
         return;
      }
      m_currentID = *m_configID;

      return;
   }

} // namespace Trig
