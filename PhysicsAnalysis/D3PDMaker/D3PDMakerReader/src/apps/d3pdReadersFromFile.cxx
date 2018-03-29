/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: d3pdReadersFromFile.cxx 634769 2014-12-09 15:23:59Z mnowak $

// STL include(s):
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

// Boost include(s):
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

// ROOT include(s):
#include <TFile.h>
#include <TList.h>
#include <TKey.h>
#include <TDirectory.h>
#include <TString.h>
#include <TObjString.h>
#include <TError.h>

// Gaudi/Athena include(s):
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "AthenaKernel/errorcheck.h"

// D3PD include(s):
#include "D3PDMakerUtils/ObjectMetadata.h"

// Local include(s):
#include "../Variable.h"
#include "../CodeGenerator_v2.h"
#include "../RootObjectMetadata.h"

/// The program description that's printed with the available parameters
static const char* PROGRAM_DESC =
   "This application can be used to create all the D3PDReader classes\n"
   "from a D3PD file that stores the metadata of the D3PDObjects that\n"
   "were used to create it";

/// A greeting text that's printed when the application starts up
static const char* PROGRAM_GREETING =
   "*******************************************************************\n"
   "*                                                                 *\n"
   "*               D3PDReader standalone code generator              *\n"
   "*                                                                 *\n"
   "*******************************************************************";

/// Formatted printing for vector objects
/**
 * The code has to print a vector in at least one place. To make it easily
 * readable in the code, I like to use such an output operator. It can be
 * used for any kind of vector as long as the template type can also be
 * printed with the << operator.
 *
 * @param out An STL output stream
 * @param vec The vector that should be printed
 * @returns The same output stream that the operator received
 */
template< typename T >
std::ostream& operator<<( std::ostream& out, const std::vector< T >& vec ) {
   out << "[";
   typename std::vector< T >::const_iterator itr = vec.begin();
   typename std::vector< T >::const_iterator end = vec.end();
   for( ; itr != end; ++itr ) {
      out << *itr;
      if( ++itr != end ) {
         out << ", ";
      }
      --itr;
   }
   out << "]";
   return out;
}

/// Function collecting the objects from a specific directory in a file
StatusCode collectObjects( TDirectory* dir,
                           std::set< D3PD::ObjectMetadata >& objects );
/// Function mergint the objects of the same type
std::vector< D3PD::ObjectMetadata >
mergeObjects( const std::set< D3PD::ObjectMetadata >& objects );

/// A convenience declaration to save myself some typeing
namespace po = boost::program_options;

int main( int argc, char* argv[] ) {
   // Let's disable the ROOT warnings:
   gErrorIgnoreLevel = kError;

   // Force the Gaudi MessageSvc into existence. This gets rid of the
   // warnings from getMessageSvc().
   IMessageSvc* msvc = 0;
   if( Gaudi::svcLocator()->service( "MessageSvc", msvc, true ).isFailure() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
         << "Couldn't set up the Gaudi message service";
      return 255;
   }

   //
   // Construct the object describing all the command line parameters of the
   // application:
   //
   po::options_description desc( PROGRAM_DESC );
   desc.add_options()
      ( "help,h", "Give some help with the program usage" )
      ( "d3pd-files,f", po::value< std::vector< std::string > >()->multitoken(),
        "Input D3PD file(s)" )
      ( "event-class-name,n", po::value< std::string >()->default_value( "Event" ),
        "Name for the D3PDReader event class" )
      ( "output,o", po::value< std::string >()->default_value( "." ),
        "Output directory for the generated source files" )
      ( "verbosity,v", po::value< int >()->default_value( 3 ),
        "Verbosity level of the application. (1:VERBOSE, 2:DEBUG, 3:INFO, ...)" );

   //
   // Interpret the command line options provided by the user:
   //
   po::variables_map vm;
   try {
      po::store( po::parse_command_line( argc, argv, desc ), vm );
      po::notify( vm );
   } catch( const std::exception& ex ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
         << "There was a problem with interpreting the command line options.";
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
         << "Message: " << ex.what();
      return 255;
   }

   //
   // If the user asked for some help, let's print it:
   //
   if( vm.count( "help" ) ) {
      std::cout << desc << std::endl;
      return 0;
   }

   //
   // Set the verbosity level of the application:
   //
   msvc->setOutputLevel( vm[ "verbosity" ].as< int >() );

   //
   // Greet the user, and print all the parameters that the application
   // received:
   //
   std::cout << PROGRAM_GREETING << std::endl;

   // Extract the file name(s) from the command line arguments:
   if( ! vm.count( "d3pd-files" ) ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
         << "You have to specify at least one D3PD file!";
      return 255;
   }
   const std::vector< std::string > file_names =
      vm[ "d3pd-files" ].as< std::vector< std::string > >();
   std::cout << "  D3PD file(s): " << file_names << std::endl;

   // Get the event class name:
   const std::string event_name = vm[ "event-class-name" ].as< std::string >();
   std::cout << "  Event class name: " << event_name << std::endl;

   // Get the output directory:
   const std::string output = vm[ "output" ].as< std::string >();
   std::cout << "  Output directory: " << output << std::endl;

   //
   // A list of all the objects collected from all the files:
   //
   std::set< D3PD::ObjectMetadata > objects;

   //
   // Loop over the specified files:
   //
   std::vector< std::string >::const_iterator file_itr = file_names.begin();
   std::vector< std::string >::const_iterator file_end = file_names.end();
   for( ; file_itr != file_end; ++file_itr ) {

      // Try to open the D3PD file:
      REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO, "d3pdReadersFromFile" )
         << "Reading file: " << *file_itr;
      TFile* ifile = TFile::Open( file_itr->c_str(), "READ" );
      if( ( ! ifile ) || ifile->IsZombie() ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
            << "Failed to open D3PD file: " << *file_itr;
         return 255;
      }

      // Search for the metadata directories:
      std::vector< std::string > metaDirectories;
      const TList* keys = ifile->GetListOfKeys();
      for( Int_t i = 0; i < keys->GetSize(); ++i ) {
         const TKey* key = dynamic_cast< TKey* >( keys->At( i ) );
         if (!key) continue;
         std::string key_name = key->GetName();
         if( ( key_name.find( "Meta" ) != key_name.npos ) &&
             ( key->GetClassName() == std::string( "TDirectoryFile" ) ) ) {
            metaDirectories.push_back( key_name );
         }
      }

      // Collect the objects from each of these directories:
      std::vector< std::string >::const_iterator dir_itr = metaDirectories.begin();
      std::vector< std::string >::const_iterator dir_end = metaDirectories.end();
      for( ; dir_itr != dir_end; ++dir_itr ) {
         // Try to access the directory:
         TDirectory* dir = ifile->GetDirectory( dir_itr->c_str() );
         if( ! dir ) {
            REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
               << "Failed to access directory: " << *dir_itr;
            return 255;
         }
         // Collect the metadata from this directory:
         if( collectObjects( dir, objects ).isFailure() ) {
            REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
               << "Failed to collect variable definitions from metadata directory: "
               << *dir_itr;
            return 255;
         }
      }

      // Close the input file:
      ifile->Close();
      delete ifile;
   }

   //
   // Merge the objects of the same type:
   //
   std::vector< D3PD::ObjectMetadata > merged_objects = mergeObjects( objects );
   if( ! merged_objects.size() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
         << "Didn't find any metadata about the D3PD variables in the "
         << "specified file(s)!";
      return 255;
   }

   //
   // Generate the D3PDObjectBase class's source:
   //
   if( D3PD::Version2::writeD3PDObjectBase( output ).isFailure() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
         << "Couldn't create the D3PDObjectBase class source";
      return 255;
   }

   //
   // Generate the VarHandle class's source:
   //
   if( D3PD::Version2::writeVarHandle( output ).isFailure() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
         << "Couldn't create the VarHandle class source";
      return 255;
   }

   //
   // Generate the VarProxy class's source if needed:
   //
   if( D3PD::Version2::writeVarProxy( output ).isFailure() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
         << "Couldn't create the VarProxy class source";
      return 255;
   }

   //
   // Generate the UserD3PDObject class's source:
   //
   if( D3PD::Version2::writeUserD3PDObject( output ).isFailure() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
         << "Couldn't create the UserD3PDObject class source";
      return 255;
   }

   //
   // Generate the D3PDReadStats class's source:
   //
   if( D3PD::Version2::writeD3PDReadStats( output ).isFailure() ) {
       REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
          << "Couldn't create the D3PDReadStats class source";
       return 255;
   }

   //
   // Generate the D3PDPerfStats class's source:
   //
   if( D3PD::Version2::writeD3PDPerfStats( output ).isFailure() ) {
       REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
          << "Couldn't create the D3PDPerfStats class source";
       return 255;
   }

   //
   // Generate the Utils source:
   //
   if( D3PD::Version2::writeUtils( output ).isFailure() ) {
       REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
          << "Couldn't create the Utils source";
       return 255;
   }

   //
   // Generate the sources for each object:
   //
   std::vector< D3PD::ObjectMetadata >::const_iterator obj_itr = merged_objects.begin();
   std::vector< D3PD::ObjectMetadata >::const_iterator obj_end = merged_objects.end();
   for( ; obj_itr != obj_end; ++obj_itr ) {

      //
      // Generate the header of the D3PDReader class:
      //
      if( D3PD::Version2::writeHeader( obj_itr->name(), output,
                                       *obj_itr ).isFailure() ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
            << "Couldn't generate the D3PDReader class header";
         return 255;
      }

      //
      // Generate the source of the D3PDReader class:
      //
      if( D3PD::Version2::writeSource( obj_itr->name(), output,
                                       *obj_itr ).isFailure() ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
            << "Couldn't generate the D3PDReader class source";
         return 255;
      }
   }

   //
   // Generate the "event class":
   //
   if( D3PD::Version2::writeEventHeader( event_name, output,
                                         objects ).isFailure() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
         << "Couldn't generate the D3PDReader event class header";
      return 255;
   }
   if( D3PD::Version2::writeEventSource( event_name, output,
                                         objects ).isFailure() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
         << "Couldn't generate the D3PDReader event class source";
      return 255;
   }

   return 0;
}

/**
 * This function is used to collect the metadata about D3PDObjects from one particular
 * directory in one D3PD file.
 *
 * @param dir The directory where metadata should be searched for
 * @param objects The global list of object metadata
 * @returns <code>StatusCode::SUCCESS</code> if the operation was successful,
 *          <code>StatusCode::FAILURE</code> otherwise
 */
StatusCode collectObjects( TDirectory* dir,
                           std::set< D3PD::ObjectMetadata >& objects ) {

   // Find all the D3PDObject definitions in the directory:
   const TList* keys = dir->GetListOfKeys();
   for( Int_t i = 0; i < keys->GetSize(); ++i ) {

      // Look for D3PDObject definitions:
      const TKey* key = dynamic_cast< TKey* >( keys->At( i ) );
      if (!key) continue;
      if( key->GetClassName() != std::string( "TObjString" ) ) continue;

      // Check whether it looks like object metadata:
      if( ! D3PD::RootObjectMetadata::isObjectMetadata( key->GetName() ) ) {
         continue;
      }

      // The name of the D3PDObject:
      const std::string objName = D3PD::ObjectMetadata::objectName( key->GetName() );

      REPORT_MESSAGE_WITH_CONTEXT( MSG::DEBUG, "d3pdReadersFromFile" )
         << "Reading object '" << objName << "'";

      // Access the variable description:
      TObjString* ostring =
         dynamic_cast< TObjString* >( dir->Get( TString( key->GetName() ) + ";" +
                                                TString::Format( "%hi", key->GetCycle() ) ) );
      if( ! ostring ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
            << "Couldn't access object: " << key->GetName() << ";"
            << key->GetCycle();
         return StatusCode::FAILURE;
      }

      // Decode the metadata:
      D3PD::RootObjectMetadata metadata;
      metadata.setName( objName );
      if( metadata.read( ostring->GetString().Data() ).isFailure() ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
            << "Couldn't collect variables from object: " << key->GetName();
         return StatusCode::FAILURE;
      }

      // Fix the variable names if they need fixing:
      if( metadata.checkPrefixes().isFailure() ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReadersFromFile" )
            << "Couldn't fix prefixes in metadata object with name: "
            << metadata.name();
         return StatusCode::FAILURE;
      }

      // Check if such an object has already been added:
      std::pair< std::set< D3PD::ObjectMetadata >::iterator, bool > ret =
         objects.insert( metadata );
      // If this object already existed, merge the two:
      if( ! ret.second ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::DEBUG, "d3pdReadersFromFile" )
            << "Merging objects with name '" << objName << "'";
         D3PD::ObjectMetadata merged( *( ret.first ) );
         merged.merge( metadata );
         objects.erase( ret.first );
         objects.insert( merged );
      } else {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::DEBUG, "d3pdReadersFromFile" )
            << "Added object with name '" << objName << "'";
      }

   }

   return StatusCode::SUCCESS;
}

/**
 * This function is used to merge the objects of the same type on its input.
 * This practically means that objects that have the same "object name" get
 * merged, irrespective of their prefix.
 *
 * @param objects A set of independent D3PDObjects
 * @returns A merged list of D3PDObjects
 */
std::vector< D3PD::ObjectMetadata >
mergeObjects( const std::set< D3PD::ObjectMetadata >& objects ) {

   // Internal map for executing the merging:
   std::map< std::string, D3PD::ObjectMetadata > tmp_result;

   // Merge the objects using the temporary map:
   std::set< D3PD::ObjectMetadata >::const_iterator set_itr = objects.begin();
   std::set< D3PD::ObjectMetadata >::const_iterator set_end = objects.end();
   for( ; set_itr != set_end; ++set_itr ) {
      tmp_result[ set_itr->name() ].setName( set_itr->name() );
      tmp_result[ set_itr->name() ].setPrefix( set_itr->prefix() );
      tmp_result[ set_itr->name() ].setContainer( set_itr->container() );
      tmp_result[ set_itr->name() ].merge( *set_itr );
   }

   // Copy the objects from the temporary map into the result vector:
   std::vector< D3PD::ObjectMetadata > result;
   std::map< std::string, D3PD::ObjectMetadata >::const_iterator map_itr =
      tmp_result.begin();
   std::map< std::string, D3PD::ObjectMetadata >::const_iterator map_end =
      tmp_result.end();
   for( ; map_itr != map_end; ++map_itr ) {
      result.push_back( map_itr->second );
   }

   return result;
}
