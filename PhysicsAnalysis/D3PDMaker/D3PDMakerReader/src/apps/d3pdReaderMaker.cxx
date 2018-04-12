/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: d3pdReaderMaker.cxx 634769 2014-12-09 15:23:59Z mnowak $

// STL include(s):
#include <iostream>
#include <vector>
#include <string>

// Boost include(s):
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

// ROOT include(s):
#include <TFile.h>
#include <TTree.h>
#include <TList.h>
#include <TKey.h>
#include <TBranch.h>
#include <TBranchElement.h>
#include <TLeaf.h>
#include <TError.h>

// Gaudi/Athena include(s):
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "../CodeGenerator_v2.h"
#include "../RootObjectMetadata.h"

/// The program description that's printed with the available parameters
static const char* PROGRAM_DESC =
   "This application can be used to generate D3PDReader classes\n"
   "based on an existing D3PD file and some additional command\n"
   "line options";

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

/// This function is used to extract the variable descriptions from the D3PD file
StatusCode extractVariables( const std::string& file_name, const std::string& tree_name,
                             const std::vector< std::string >& patterns,
                             D3PD::RootObjectMetadata& metadata );
StatusCode extractVariables( const std::string& file_name, const std::string& tree_name,
                             const std::string& pattern,
                             D3PD::RootObjectMetadata& metadata );

/// A convenience declaration to save myself some typeing
namespace po = boost::program_options;

int main( int argc, char* argv[] ) {
   // Let's disable the ROOT warnings:
   gErrorIgnoreLevel = kError;

   // Force the Gaudi MessageSvc into existence. This gets rid of the
   // warnings from getMessageSvc().
   IMessageSvc* msvc = 0;
   if( Gaudi::svcLocator()->service( "MessageSvc", msvc, true ).isFailure() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
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
      ( "classname,n", po::value< std::string >()->default_value( "D3PDReader" ),
        "Generated class name" )
      ( "d3pd-files,f", po::value< std::vector< std::string > >()->multitoken(),
        "Input D3PD file(s)" )
      ( "tree,t", po::value< std::string >()->default_value( "" ), "Name of the D3PD tree" )
      ( "variables,v", po::value< std::vector< std::string > >()->multitoken(),
        "Variable names to consider (regular expression(s))" )
      ( "prefixes,p", po::value< std::vector< std::string > >()->multitoken(),
        "Variable name prefix(es)" )
      ( "container,c", "Option specifying that a proxy class should be created" )
      ( "output,o", po::value< std::string >()->default_value( "." ),
        "Output directory for the generated source files" );

   //
   // Interpret the command line options provided by the user:
   //
   po::variables_map vm;
   try {
      po::store( po::parse_command_line( argc, argv, desc ), vm );
      po::notify( vm );
   } catch( const std::exception& ex ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
         << "There was a problem with interpreting the command line options.";
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
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
   // Greet the user, and print all the parameters that the application
   // received:
   //
   std::cout << PROGRAM_GREETING << std::endl;
   std::cout << "Program will be executed with the options:" << std::endl;

   // Get the name of the class to be generated:
   std::string class_name = vm[ "classname" ].as< std::string >();
   std::cout << "  Generating class with name: \"D3PDReader::" << class_name << "\""
             << std::endl;

   // Extract the file name(s) from the command line arguments:
   if( ! vm.count( "d3pd-files" ) ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
         << "You have to specify at least one D3PD file!";
      return 255;
   }
   const std::vector< std::string > file_names =
      vm[ "d3pd-files" ].as< std::vector< std::string > >();
   std::cout << "  D3PD file(s): " << file_names << std::endl;

   // Get the name of the D3PD TTree:
   const std::string d3pd_tree_name = vm[ "tree" ].as< std::string >();
   if( d3pd_tree_name != "" ) {
      std::cout << "  Using D3PD tree: " << d3pd_tree_name << std::endl;
   } else {
      std::cout << "  Trying to find D3PD tree automatically" << std::endl;
   }

   // Get the variable name patterns:
   std::vector< std::string > var_names;
   if( vm.count( "variables" ) ) {
      var_names = vm[ "variables" ].as< std::vector< std::string > >();
      std::cout << "  Only considering variables with names: " << var_names << std::endl;
   } else {
      std::cout << "  Considering all variables from the input file" << std::endl;
   }

   // Get the common prefix of the variable names:
   if( ( ( vm.count( "prefixes" ) != vm.count( "variables" ) ) &&
         ( vm.count( "prefixes" ) != 1 ) ) ||
       ( vm.count( "prefixes" ) == 0 ) ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
         << "You have to either specify just one prefix, or the same number as "
         << "how many variable regular expressions you gave";
      return 255;
   }
   const std::vector< std::string > prefixes =
      vm[ "prefixes" ].as< std::vector< std::string > >();
   std::cout << "  Common variable prefix(es): " << prefixes << std::endl;

   // Get the parameter describing whether proxy classes should be generated:
   const bool is_container = vm.count( "container" );
   std::cout << "  Proxy class will " << ( is_container ? "" : "NOT " ) << "be created"
             << std::endl;

   // Get the output directory:
   const std::string output = vm[ "output" ].as< std::string >();
   std::cout << "  Output directory: " << output << std::endl;

   //
   // Create the metadata object(s):
   //
   std::map< std::string, D3PD::RootObjectMetadata > metadata;
   std::vector< std::string >::const_iterator prefix_itr = prefixes.begin();
   std::vector< std::string >::const_iterator prefix_end = prefixes.end();
   for( ; prefix_itr != prefix_end; ++prefix_itr ) {
      metadata[ *prefix_itr ].setName( class_name );
      metadata[ *prefix_itr ].setPrefix( *prefix_itr );
      metadata[ *prefix_itr ].setContainer( is_container );
   }

   //
   // Iterate over the files:
   //
   std::vector< std::string >::const_iterator file_itr = file_names.begin();
   std::vector< std::string >::const_iterator file_end = file_names.end();
   for( ; file_itr != file_end; ++file_itr ) {

      if( prefixes.size() > 1 ) {

         for( size_t i = 0; i < prefixes.size(); ++i ) {
            if( extractVariables( *file_itr, d3pd_tree_name, var_names[ i ],
                                  metadata[ prefixes[ i ] ] ).isFailure() ) {
               REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
                  << "Couldn't extract the variable descriptions from file: "
                  << *file_itr;
               return 255;
            }
         }

      } else {

         if( extractVariables( *file_itr, d3pd_tree_name, var_names,
                               metadata[ prefixes[ 0 ] ] ).isFailure() ) {
            REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
               << "Couldn't extract the variable descriptions from file: "
               << *file_itr;
            return 255;
         }

      }

   }

   //
   // Merge the information extracted:
   //
   D3PD::ObjectMetadata summed_meta;
   summed_meta.setName( class_name );
   summed_meta.setPrefix( prefixes[ 0 ] );
   summed_meta.setContainer( is_container );
   prefix_itr = prefixes.begin();
   prefix_end = prefixes.end();
   for( ; prefix_itr != prefix_end; ++prefix_itr ) {
      summed_meta += metadata[ *prefix_itr ];
   }

   //
   // Generate the D3PDObjectBase class's source:
   //
   if( D3PD::Version2::writeD3PDObjectBase( output ).isFailure() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
         << "Couldn't create the D3PDObjectBase class source";
      return 255;
   }

   //
   // Generate the VarHandle class's source:
   //
   if( D3PD::Version2::writeVarHandle( output ).isFailure() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
         << "Couldn't create the VarHandle class source";
      return 255;
   }

   //
   // Generate the VarProxy class's source if needed:
   //
   if( is_container ) {
      if( D3PD::Version2::writeVarProxy( output ).isFailure() ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
            << "Couldn't create the VarProxy class source";
         return 255;
      }
   }

   //
   // Generate the VarHandle class's source:
   //
   if( D3PD::Version2::writeUserD3PDObject( output ).isFailure() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
         << "Couldn't create the UserD3PDObject class source";
      return 255;
   }

   //
   // Generate the D3PDReadStats class's source:
   //
   if( D3PD::Version2::writeD3PDReadStats( output ).isFailure() ) {
       REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
          << "Couldn't create the D3PDReadStats class source";
       return 255;
   }

   //
   // Generate the D3PDPerfStats class's source:
   //
   if( D3PD::Version2::writeD3PDPerfStats( output ).isFailure() ) {
       REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
          << "Couldn't create the D3PDPerfStats class source";
       return 255;
   }

   //
   // Generate the Utils source:
   //
   if( D3PD::Version2::writeUtils( output ).isFailure() ) {
       REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
          << "Couldn't create the Utils source";
       return 255;
   }

   //
   // Generate the header of the D3PDReader class:
   //
   if( D3PD::Version2::writeHeader( class_name, output, summed_meta ).isFailure() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
         << "Couldn't generate the D3PDReader class header";
      return 255;
   }

   //
   // Generate the source of the D3PDReader class:
   //
   if( D3PD::Version2::writeSource( class_name, output, summed_meta ).isFailure() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker" )
         << "Couldn't generate the D3PDReader class source";
      return 255;
   }

   return 0;
}

/**
 * This function is used to parse the variables in the specified D3PD file, and collect
 * the types and names of them. This information is then used to generate the D3PDReader
 * sources.
 *
 * @param file_name Name of the input D3PD file
 * @param tree_name Name of the D3PD TTree in the file
 * @param patterns A vector of regular expressions to select the variables with
 * @param vars The vector which is filled with the extracted information
 * @returns A StatusCode indicating whether the operation was successful
 */
StatusCode extractVariables( const std::string& file_name, const std::string& tree_name,
                             const std::vector< std::string >& patterns,
                             D3PD::RootObjectMetadata& metadata ) {

   //
   // Try to open the D3PD file:
   //
   TFile* file = TFile::Open( file_name.c_str(), "READ" );
   if( ( ! file ) || file->IsZombie() ) {
      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker::extractVariables" )
         << "Failed to open D3PD file: " << file_name;
      return StatusCode::FAILURE;
   }

   //
   // Access the D3PD tree:
   //
   TTree* tree = 0;
   if( tree_name != "" ) {
      //
      // If the user specified a tree name, then try to access that tree:
      //
      tree = dynamic_cast< TTree* >( file->Get( tree_name.c_str() ) );
      if( ! tree ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker::extractVariables" )
            << "Couldn't find tree \"" << tree_name << "\" in file: " << file_name;
         return StatusCode::FAILURE;
      }
   } else {
      //
      // If the user didn't specify a tree name then search for all the TTree-s
      // in the file which are not called "CollectionTree", and select the first
      // one.
      //
      std::string name = "";
      const TList* keys = file->GetListOfKeys();
      for( Int_t i = 0; i < keys->GetSize(); ++i ) {
         const TKey* key = dynamic_cast< TKey* >( keys->At( i ) );
         if (!key) continue;
         std::string key_name = key->GetName();
         if( ( key_name.find( "CollectionTree" ) == key_name.npos ) &&
             ( key->GetClassName() == std::string( "TTree" ) ) ) {
            name = key_name;
            std::cout << "Assuming that the D3PD tree is called: " << name
                      << std::endl;
            break;
         }
      }
      if( name == "" ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker::extractVariables" )
            << "Couldn't find the name for the D3PD tree!";
         return StatusCode::FAILURE;
      }
      tree = dynamic_cast< TTree* >( file->Get( name.c_str() ) );
      if( ! tree ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker::extractVariables" )
            << "Couldn't find tree \"" << tree_name << "\" in file: "
            << file_name;
         return StatusCode::FAILURE;
      }
   }

   //
   // Look at all the branches in the tree:
   //
   TObjArray* branches = tree->GetListOfBranches();
   for( Int_t i = 0; i < branches->GetSize(); ++i ) {

      //
      // Only consider the branches that match one of the specified patterns:
      //
      bool pass = false;
      std::vector< std::string >::const_iterator itr = patterns.begin();
      std::vector< std::string >::const_iterator end = patterns.end();
      for( ; itr != end; ++itr ) {
         if( boost::regex_search( branches->At( i )->GetName(),
                                  boost::basic_regex< char >( itr->c_str() ) ) ) {
            pass = true;
            break;
         }
      }
      if( ! pass ) continue;

      //
      // Extract the type of the variable. This has to be done a little differently for
      // primitive types and objects (STL containers).
      //
      const std::string branch_type( branches->At( i )->IsA()->GetName() );
      if( branch_type == "TBranch" ) {

         //
         // If it's a branch describing a primitive type, then the name of the type
         // has to be extracted from the single leaf of the branch:
         //
         TBranch* branch = dynamic_cast< TBranch* >( branches->At( i ) );
         if( ! branch ) {
            REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker::extractVariables" )
               << "Could not cast object into a TBranch!";
            return StatusCode::FAILURE;
         }
         TObjArray* leaves = branch->GetListOfLeaves();
         bool leaf_found = false;
         for( Int_t j = 0; j < leaves->GetSize(); ++j ) {
            TLeaf* leaf = dynamic_cast< TLeaf* >( leaves->At( 0 ) );
            if( leaf ) {
               if( metadata.addVariable( branch->GetName(),
                                         leaf->GetTypeName(),
                                         branch->GetTitle() ).isFailure() ) {
                  REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker::extractVariables" )
                     << "Failed adding variable: " << branch->GetName();
                  return StatusCode::FAILURE;
               }
               leaf_found = true;
               break;
            }
         }
         if( ! leaf_found ) {
            REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker::extractVariables" )
               << "Couldn't find the TLeaf describing a primitive type!";
            return StatusCode::FAILURE;
         }

      } else if( branch_type == "TBranchElement" ) {

         //
         // If it's a branch describing an object, then the situation is a bit simpler.
         // In this case we can ask the TBranchElement directly for the class name.
         // Remember however that this is a "simplified" name of the class, like
         // "vector<float>".
         //
         TBranchElement* branch = dynamic_cast< TBranchElement* >( branches->At( i ) );
         if( ! branch ) {
            REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker::extractVariables" )
               << "Could not cast object into a TBranchElement!";
            return StatusCode::FAILURE;
         }
         if( metadata.addVariable( branch->GetName(),
                                   branch->GetClassName(),
                                   branch->GetTitle() ).isFailure() ) {
            REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "d3pdReaderMaker::extractVariables" )
               << "Failed adding variable: " << branch->GetName();
            return StatusCode::FAILURE;
         }
      }
   }

   // Clean up after ourselves:
   delete file;
   return StatusCode::SUCCESS;
}

StatusCode extractVariables( const std::string& file_name, const std::string& tree_name,
                             const std::string& pattern,
                             D3PD::RootObjectMetadata& metadata ) {

   std::vector< std::string > patterns; patterns.push_back( pattern );
   return extractVariables( file_name, tree_name, patterns, metadata );
}
