/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CodeGenerator_v2.cxx 530778 2012-12-17 12:56:33Z krasznaa $

// System include(s):
#include <ctime>
#include <locale>
#include <sys/stat.h>

// STL include(s):
#include <fstream>
#include <sstream>
#include <algorithm>

// Boost include(s):
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

// Gaudi/Athena include(s):
#include "GaudiKernel/System.h"
#include "AthenaKernel/errorcheck.h"

// D3PD include(s):
#include "D3PDMakerUtils/ObjectMetadata.h"

// Local include(s):
#include "CodeGenerator_v2.h"
#include "CodeGenerator_v2_constants.h"
#include "isPrimitive.h"

/// A little header for all the generated source files
static const char* CODE_COMMENT =
   "// Dear emacs, this is -*- c++ -*-\n"
   "// -------------------------------------------------------------\n"
   "//             Code produced by D3PDMakerReader\n"
   "//\n"
   "//  author: Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>\n"
   "// -------------------------------------------------------------";

/// A convenience macro used in the event class generation
#define FOR_ALL_EVENT_VARIABLES(EXP) {                                  \
      std::set< ObjectMetadata >::const_iterator meta_itr =             \
         metadata.begin();                                              \
      std::set< ObjectMetadata >::const_iterator meta_end =             \
         metadata.end();                                                \
      std::set< std::string > varnames;                                 \
      for( ; meta_itr != meta_end; ++meta_itr ) {                       \
         const std::string varname =                                    \
            eventVariableName( meta_itr->prefix(),                      \
                               meta_itr->name(),                        \
                               varnames );                              \
         varnames.insert( varname );                                    \
         EXP;                                                           \
      }                                                                 \
   } while( 0 )

namespace {

   /// Function extracting the template argument of a vector type
   /**
    * This function is used to "extract" the template argument of an std::vector
    * type declaration. So for instance it makes from "std::vector<int>" the
    * string "int". It tries to be as smart as possible, so you can give it
    * types with fairly complicated template parameters, but it may not be
    * without fault...
    *
    * @param type The original vector type name
    * @param ok A flag that is set to <code>false</code> if the extraction
    *           wasn't successful
    * @returns The template parameter of the vector type given as the first
    *          parameter
    */
   std::string vectorTemplateArgument( const std::string& type,
                                       bool& ok ) {

      //
      // Check that the type declaration begins with "std::vector<" or
      // "vector<":
      //
      ok = true;
      std::string result = type;
      if( result.find( "std::vector<" ) == 0 ) {
         result.replace( 0, 12, "" );
      } else if( result.find( "vector<" ) == 0 ) {
         result.replace( 0, 7, "" );
      } else {
         ok = false;
         return "";
      }

      //
      // Now go through the characters one-by-one, and try to find the closing
      // bracket of the template argument. Remember that the template argument
      // can itself be a templated type (vector of a vector for instance), so
      // the code has to remember how many brackets were opened and closed.
      //
      int brackets = 0;
      for( size_t i = 0; i < result.size(); ++i ) {

         switch( result[ i ] ) {
         case ',':
            if( ! brackets ) {
               return result.substr( 0, i );
            }
            break;
         case '<':
            ++brackets;
            break;
         case '>':
            if( ! brackets ) {
               // This is for the case when the vector's template argument is
               // itself a template type:
               if( result[ i - 1 ] == ' ' ) {
                  return result.substr( 0, i - 1 );
               } else {
                  return result.substr( 0, i );
               }
            }
            --brackets;
            break;
         default:
            break;
         }
      }

      ok = false;
      return "";
   }

   /// Function adding STL headers to the generated source files
   /**
    * This function is used to add, as needed, STL header includes to the
    * generated files.
    *
    * @param out Stream into which the code is written
    * @param name Name of the STL type (and header file)
    * @param metadata Object describing the generated variables
    */
   void addSTLHeader( std::ostream& out, const char* name,
                      const D3PD::ObjectMetadata& metadata ) {

      // Check if the STL name appears in the variable type names:
      bool header_needed = false;
      std::set< D3PD::ObjectMetadata::Variable >::const_iterator itr =
         metadata.variables().begin();
      std::set< D3PD::ObjectMetadata::Variable >::const_iterator end =
         metadata.variables().end();
      for( ; itr != end; ++itr ) {
         if( itr->type().find( name ) != std::string::npos ) {
            header_needed = true;
            break;
         }
      }

      // If it does, let's include it:
      if( header_needed ) {
         out << "#include <" << name << ">" << std::endl;
         // This statement is here to make sure that ROOT's type names such as
         // "vector<float>" will be compilable as well:
         out << "using std::" << name << ";" << std::endl;
      }

      return;
   }

   /// Function constructing a C++ variable name from the branch name
   /**
    * Some branches in D3PDs tend to have strange names. This function makes
    * sure that these names are transformed into something that C++ can
    * accept as variable names.
    *
    * @param varname The name of the branch
    * @returns A variable name that can be used in a C++ source
    */
   std::string variableName( const std::string& varname ) {

      std::string result = varname;

      // Exchange the non C++-compilant characters:
      std::string::size_type i = 0;
      while( ( i = result.find( ":" ) ) != std::string::npos ) {
         result[ i ] = '_';
      }
      while( ( i = result.find( " " ) ) != std::string::npos ) {
         result[ i ] = '_';
      }

      // If the first character is a number, preceed it with the
      // letter 'x':
      if( isdigit( result[ 0 ] ) ) {
         result = "x" + result;
      }

      return result;
   }

   /// Function creating a unique variabel name for the event class
   /**
    * This function is used to create a unique variable name for one
    * D3PDReader object in the main "event class".
    *
    * If the object has a prefix, we use that. But the function has to
    * check whether the created variable name exists already or not.
    * (We have multiple objects with the prefix "trig_" unfortunately.)
    *
    * If the object has no prefix (like in the case of EventInfoD3PDObject),
    * the object name is used as the basis for the variable name.
    *
    * @param prefix The prefix used by the object in question
    * @param objectName The name of the object in question
    * @param varnames List of already declared variable names
    * @returns A unique variable name for the new object
    */
   std::string eventVariableName( const std::string& prefix,
                                  const std::string& objectName,
                                  const std::set< std::string >& varnames ) {

      // The resulting string:
      std::string result;

      // If we have a prefix, use that as the variable name:
      if( prefix.size() ) {
         result = prefix;
         // Remove a possible trailing underscore from the name:
         if( result[ result.size() - 1 ] == '_' ) {
            result = result.substr( 0, prefix.size() - 1 );
         }
         // Return this variable name if it's not been used yet:
         if( varnames.find( result ) == varnames.end() ) {
            return result;
         }
         // Try to extend the variable name until it becomes unique:
         for( int i = 0; ; ++i ) {
            std::ostringstream name;
            name << result << "_" << static_cast< char >( 'a' + i );
            if( varnames.find( name.str() ) == varnames.end() ) {
               return name.str();
            }
         }
      }

      // If we don't have a prefix, let's use the object name:
      result = objectName;
      // Remove a posible "D3PDObject" from the name:
      if( result.find( "D3PDObject" ) == result.size() - 10 ) {
         result = result.substr( 0, result.size() - 10 );
      }
      // Turn the string into all lower case:
      boost::algorithm::to_lower( result );

      return result;
   }

} // private namespace

namespace D3PD {

   namespace Version2 {

      /**
       * This function can be used to create the D3PDReader::D3PDObjectBase
       * class's source files. The created class is used as a base to all the
       * main D3PDReader classes.
       *
       * @param dir Directory name where the source file should be put
       * @returns A StatusCode indicating whether the operation was successful
       */
      StatusCode writeD3PDObjectBase( const std::string& dir ) {

         // Construct the file names as they will be needed in a few places:
         const std::string headerName = dir + "/" + D3PDOBJECTBASE_HEADER_NAME;

         // Only create the header if it doesn't exist:
         struct stat fileInfo;
         if( stat( headerName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO,
                                         "Version2::writeD3PDObjectBase" )
               << "Generating file: " << headerName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream header( headerName.c_str() );

            // Write the header file:
            header << D3PDOBJECTBASE_HEADER << std::endl;
            header.close();
         }

         return StatusCode::SUCCESS;
      }

      /**
       * This function can be used to create the D3PDReader::VarHandle class's
       * source files. The created class is used throughout the D3PDReader code
       * to access the branches of the TTrees.
       *
       * @param dir Directory name where the source files should be put
       * @returns A StatusCode indicating whether the operation was successful
       */
      StatusCode writeVarHandle( const std::string& dir ) {

         // Construct the file names as they will be needed in a few places:
         const std::string headerName = dir + "/" + VARHANDLE_HEADER_NAME;
         const std::string implName   = dir + "/" + VARHANDLE_IMPL_NAME;
         const std::string sourceName = dir + "/" + VARHANDLE_CXX_NAME;

         // Only create the header if it doesn't exist:
         struct stat fileInfo;
         if( stat( headerName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO,
                                         "Version2::writeVarHandle" )
               << "Generating file: " << headerName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream header( headerName.c_str() );

            // Write the header file:
            header << VARHANDLE_HEADER << std::endl;
            header.close();
         }

         // Only create the implementation if it doesn't exist:
         if( stat( implName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO, "Version2::writeVarHandle" )
               << "Generating file: " << implName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream impl( implName.c_str() );

            // Write the header file:
            impl << VARHANDLE_IMPL << std::endl;
            impl.close();
         }

         // Only create the source if it doesn't exist:
         if( stat( sourceName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO, "Version2::writeVarHandle" )
               << "Generating file: " << sourceName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream source( sourceName.c_str() );

            // Write the header file:
            source << VARHANDLE_CXX << std::endl;
            source.close();
         }

         return StatusCode::SUCCESS;
      }

      /**
       * This function can be used to create the D3PDReader::VarProxy class's
       * source files. The created class is used in the D3PDReader code to
       * construct classes representing one element in a container.
       *
       * @param dir Directory name where the source files should be put
       * @returns A StatusCode indicating whether the operation was successful
       */
      StatusCode writeVarProxy( const std::string& dir ) {

         // Construct the file names as they will be needed in a few places:
         const std::string headerName = dir + "/" + VARPROXY_HEADER_NAME;
         const std::string implName   = dir + "/" + VARPROXY_IMPL_NAME;

         // Only create the header if it doesn't exist:
         struct stat fileInfo;
         if( stat( headerName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO, "Version2::writeVarProxy" )
               << "Generating file: " << headerName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream header( headerName.c_str() );

            // Write the header file:
            header << VARPROXY_HEADER << std::endl;
            header.close();
         }

         // Only create the implementation if it doesn't exist:
         if( stat( implName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO, "Version2::writeVarProxy" )
               << "Generating file: " << implName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream impl( implName.c_str() );

            // Write the header file:
            impl << VARPROXY_IMPL << std::endl;
            impl.close();
         }

         return StatusCode::SUCCESS;
      }

      /**
       * This function can be used to create the D3PDReader::UserD3PDObject
       * class's source files. The created class is used throughout the
       * D3PDReader code to create/access user variables.
       *
       * @param dir Directory name where the source files should be put
       * @returns A StatusCode indicating whether the operation was successful
       */
      StatusCode writeUserD3PDObject( const std::string& dir ) {

         // Construct the file names as they will be needed in a few places:
         const std::string headerName = dir + "/" + USERD3PDOBJECT_HEADER_NAME;
         const std::string implName   = dir + "/" + USERD3PDOBJECT_IMPL_NAME;
         const std::string sourceName = dir + "/" + USERD3PDOBJECT_CXX_NAME;

         // Only create the header if it doesn't exist:
         struct stat fileInfo;
         if( stat( headerName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO,
                                         "Version2::writeUserD3PDObject" )
               << "Generating file: " << headerName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream header( headerName.c_str() );

            // Write the header file:
            header << USERD3PDOBJECT_HEADER << std::endl;
            header.close();
         }

         // Only create the implementation if it doesn't exist:
         if( stat( implName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO,
                                         "Version2::writeUserD3PDObject" )
               << "Generating file: " << implName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream impl( implName.c_str() );

            // Write the header file:
            impl << USERD3PDOBJECT_IMPL << std::endl;
            impl.close();
         }

         // Only create the source if it doesn't exist:
         if( stat( sourceName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO,
                                         "Version2::writeUserD3PDObject" )
               << "Generating file: " << sourceName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream source( sourceName.c_str() );

            // Write the header file:
            source << USERD3PDOBJECT_CXX << std::endl;
            source.close();
         }

         return StatusCode::SUCCESS;
      }

      /**
       * This function can be used to create the D3PDReader::D3PDReadStats
       * class's source files. The created class is used in the D3PDReader code
       * to keep track of the performance of an analysis.
       *
       * @param dir Directory name where the source files should be put
       * @returns A StatusCode indicating whether the operation was successful
       */
      StatusCode writeD3PDReadStats( const std::string& dir ) {

         // Construct the file names as they will be needed in a few places:
         const std::string headerName = dir + "/" + D3PDREADSTATS_HEADER_NAME;
         const std::string cxxName    = dir + "/" + D3PDREADSTATS_CXX_NAME;

         // Only create the header if it doesn't exist:
         struct stat fileInfo;
         if( stat( headerName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO,
                                         "Version2::writeD3PDReadStats" )
               << "Generating file: " << headerName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream header( headerName.c_str() );

            // Write the header file:
            header << D3PDREADSTATS_HEADER << std::endl;
            header.close();
         }

         // Only create the implementation if it doesn't exist:
         if( stat( cxxName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO,
                                         "Version2::writeD3PDReadStats" )
               << "Generating file: " << cxxName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream impl( cxxName.c_str() );

            // Write the header file:
            impl << D3PDREADSTATS_CXX << std::endl;
            impl.close();
         }

         return StatusCode::SUCCESS;
      }

      /**
       * This function can be used to create the D3PDReader::D3PDPerfStats
       * class's source files. The created class is collecting global
       * performance statistics from ROOT during a D3PD analysis.
       *
       * @param dir Directory name where the source files should be put
       * @returns A StatusCode indicating whether the operation was successful
       */
      StatusCode writeD3PDPerfStats( const std::string& dir ) {

         // Construct the file names as they will be needed in a few places:
         const std::string headerName = dir + "/" + D3PDPERFSTATS_HEADER_NAME;
         const std::string cxxName    = dir + "/" + D3PDPERFSTATS_CXX_NAME;

         // Only create the header if it doesn't exist:
         struct stat fileInfo;
         if( stat( headerName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO,
                                         "Version2::writeD3PDPerfStats" )
               << "Generating file: " << headerName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream header( headerName.c_str() );

            // Write the header file:
            header << D3PDPERFSTATS_HEADER << std::endl;
            header.close();
         }

         // Only create the implementation if it doesn't exist:
         if( stat( cxxName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO,
                                         "Version2::writeD3PDPerfStats" )
               << "Generating file: " << cxxName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream impl( cxxName.c_str() );

            // Write the header file:
            impl << D3PDPERFSTATS_CXX << std::endl;
            impl.close();
         }

         return StatusCode::SUCCESS;
      }

      /**
       * This function can be used to create source files containing some
       * utility functions.
       *
       * @param dir Directory name where the source files should be put
       * @returns A StatusCode indicating whether the operation was successful
       */
      StatusCode writeUtils( const std::string& dir ) {

         // Construct the file names as they will be needed in a few places:
         const std::string headerName = dir + "/" + UTILS_HEADER_NAME;
         const std::string cxxName    = dir + "/" + UTILS_CXX_NAME;

         // Only create the header if it doesn't exist:
         struct stat fileInfo;
         if( stat( headerName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO, "Version2::writeUtils" )
               << "Generating file: " << headerName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream header( headerName.c_str() );

            // Write the header file:
            header << UTILS_HEADER << std::endl;
            header.close();
         }

         // Only create the implementation if it doesn't exist:
         if( stat( cxxName.c_str(), &fileInfo ) ) {

            // Let everyone know what we're doing:
            REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO, "Version2::writeUtils" )
               << "Generating file: " << cxxName;

            // Open the header file (overwriting a possibly existing file):
            std::ofstream impl( cxxName.c_str() );

            // Write the header file:
            impl << UTILS_CXX << std::endl;
            impl.close();
         }

         return StatusCode::SUCCESS;
      }

      /**
       * This function is used to create the header of the class describing a
       * set of D3PD variables. When the source code is generated in an Athena
       * job, the classes follow the organization of the D3PDMaker python
       * configuration. But in standalone code generation mode any kind of
       * variables can be grouped together.
       *
       * @param classname The name of the class to be generated
       * @param dir The directory where the header file should be put
       * @param prefix The common prefix of the variable names
       * @param vars A vector of the variable descriptions
       * @param isContainer A flag specifying whether an element proxy class
       *                    should be generated
       * @returns A StatusCode indicating whether the operation was successful
       */
      StatusCode writeHeader( const std::string& classname,
                              const std::string& dir,
                              const ObjectMetadata& metadata ) {

         // Construct the file name:
         const std::string fileName = dir + "/" + classname + ".h";

         // If the file already exists, let's not overwrite it:
         struct stat fileInfo;
         if( ! stat( fileName.c_str(), &fileInfo ) ) return StatusCode::SUCCESS;

         // Let everyone know what we're doing:
         REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO, "Version2::writeHeader" )
            << "Generating file: " << fileName;

         // Open the header file (overwriting a possibly existing file):
         std::ofstream header( fileName.c_str() );

         // Write a header for the file:
         header << CODE_COMMENT << std::endl;
         header << "#ifndef D3PDREADER_" << classname << "_H" << std::endl;
         header << "#define D3PDREADER_" << classname << "_H" << std::endl
                << std::endl;

         // The STL map is always needed:
         header << "// STL include(s):" << std::endl;
         header << "#include <map>" << std::endl;

         // Include some other STL headers if they're needed:
         addSTLHeader( header, "vector", metadata );
         addSTLHeader( header, "string", metadata );
         header << std::endl;

         // ROOT include(s):
         header << "// ROOT include(s):" << std::endl;
         header << "#include <TObject.h>" << std::endl;
         header << "#include <TString.h>" << std::endl << std::endl;

         // Local include(s):
         header << "#include \"" << D3PDOBJECTBASE_HEADER_NAME << "\""
                << std::endl;
         header << "#include \"" << D3PDREADSTATS_HEADER_NAME << "\""
                << std::endl;
         header << "#include \"" << VARHANDLE_HEADER_NAME << "\"" << std::endl;
         if( metadata.container() ) {
            header << "#include \"" << VARPROXY_HEADER_NAME << "\""
                   << std::endl;
         }
         header << "#include \"" << USERD3PDOBJECT_HEADER_NAME << "\""
                << std::endl;
         header << std::endl;

         // Forward declaration(s):
         header << "class TTree;" << std::endl << std::endl;

         // Declare the namespace:
         header << "namespace D3PDReader {" << std::endl << std::endl;

         //
         // Declare the proxy class:
         //
         if( metadata.container() ) {

            // Forward declare the reader class:
            header << "   // Forward declaration(s):" << std::endl;
            header << "   class " << classname << ";" << std::endl << std::endl;

            // Declare the proxy class:
            header << "   /**" << std::endl;
            header << "    * Code generated by CodeGenerator_v2 on:"
                   << std::endl;
            header << "    *  host = " << System::hostName() << std::endl;
            header << "    *  OS   = " << System::osName() << " / "
                   << System::osVersion() << std::endl;
            header << "    *  user = " << System::accountName() << std::endl;
            const time_t rawtime = time( NULL );
            header << "    *  time = " << ctime( &rawtime );
            header << "    */" << std::endl;
            header << "   class " << classname << "Element : "
                   << "public UserD3PDObjectElement {" << std::endl
                   << std::endl;
            header << "      friend class " << classname << ";" << std::endl
                   << std::endl;
            header << "   protected:" << std::endl;

            // Create the constructor:
            header << "      /// Constructor only visible to " << classname
                   << std::endl;
            header << "      " << classname << "Element( size_t index, const "
                   << classname << "& parent );" << std::endl << std::endl;

            // Create the copy constructor:
            header << "   public:" << std::endl;
            header << "      /// Copy constructor" << std::endl;
            header << "      " << classname << "Element( const " << classname
                   << "Element& parent );" << std::endl << std::endl;

            // Create the generic accessor functions:
            header << "      /// Parent object of this proxy object"
                   << std::endl;
            header << "      const " << classname << "* GetParent() const;"
                   << std::endl;
            header << "      /// Index of the object inside its container"
                   << std::endl;
            header << "      size_t GetIndex() const;" << std::endl
                   << std::endl;

            // Create the member variable(s):
            std::map< std::string, int > variableCounter;
            std::set< ObjectMetadata::Variable >::const_iterator itr =
               metadata.variables().begin();
            std::set< ObjectMetadata::Variable >::const_iterator end =
               metadata.variables().end();
            for( ; itr != end; ++itr ) {
               // Ignore the container size variable:
               if( ( itr->name() == "n" ) || ( itr->name() == "N" ) ) continue;

               bool ok = true;
               const std::string type =
                  vectorTemplateArgument( itr->type(), ok );
               if( ! ok ) {
                  REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR,
                                               "Version2::writeHeader" )
                     << "Unexpected variable type encountered for "
                     << "container dumper: " << itr->type();
                  REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR,
                                               "Version2::writeHeader" )
                     << "Variable name: " << itr->name();
                  return StatusCode::FAILURE;
               }
               if( itr->doc() != "" ) {
                  header << "      /// " << itr->doc() << std::endl; 
               }
               if( variableCounter.find( itr->name() ) ==
                   variableCounter.end() ) {
                  header << "      VarProxy< " << type << " > "
                         << variableName( itr->name() ) << ";" << std::endl;
                  variableCounter[ itr->name() ] = 1;
               } else {
                  header << "      VarProxy< " << type << " > "
                         << variableName( itr->name() )
                         << variableCounter[ itr->name() ] << ";" << std::endl;
                  variableCounter[ itr->name() ]++;
               }
            }
            header << std::endl;

            // Create the private variable(s):
            header << "   private:" << std::endl;
            header << "      /// Reference to the parent of this object"
                   << std::endl;
            header << "      const " << classname << "* fParent;" << std::endl;
            header << "      /// The index of this object inside the parent "
                   << "container" << std::endl;
            header << "      size_t fIndex;" << std::endl << std::endl;

            // Close the class definition:
            header << "      ClassDef( " << classname << "Element, 0 )"
                   << std::endl
                   << std::endl;
            header << "   }; // class " << classname << "Element" << std::endl
                   << std::endl;
         }

         //
         // Declare the reader class itself:
         //
         header << "   /**" << std::endl;
         header << "    * Code generated by CodeGenerator_v2 on:" << std::endl;
         header << "    *  host = " << System::hostName() << std::endl;
         header << "    *  OS   = " << System::osName() << " / "
                << System::osVersion() << std::endl;
         header << "    *  user = " << System::accountName() << std::endl;
         const time_t rawtime = time( NULL );
         header << "    *  time = " << ctime( &rawtime );
         header << "    */" << std::endl;
         header << "   class " << classname << " : "
                << "public UserD3PDObject {" << std::endl << std::endl;
         header << "   public:" << std::endl;

         //
         // Declare the constructor(s)/destructor:
         //
         header << "      /// Constructor used when reading from a TTree"
                << std::endl;
         header << "      " << classname
                << "( const ::Long64_t& master, const char* prefix = \""
                << metadata.prefix() << "\" );" << std::endl;
         header << "      /// Constructor when the object is only used for "
                << "writing data out" << std::endl;
         header << "      " << classname << "( const char* prefix = \""
                << metadata.prefix() << "\" );" << std::endl;
         if( metadata.container() ) {
            header << "      /// Destructor" << std::endl;
            header << "      ~" << classname << "();" << std::endl;
         }
         header << std::endl;

         //
         // Declare some functions:
         //
         header << "      /// Get the currently configured prefix value"
                << std::endl;
         header << "      virtual const char* GetPrefix() const;" << std::endl;
         header << "      /// Set the prefix for the variables" << std::endl;
         header << "      virtual void SetPrefix( const char* prefix );"
                << std::endl << std::endl;

         header << "      /// Connect the object to an input TTree"
                << std::endl;
         header << "      virtual void ReadFrom( ::TTree* tree );" << std::endl;
         header << "      /// Connect the object to an output TTree"
                << std::endl;
         header << "      virtual void WriteTo( ::TTree* tree );" << std::endl
                << std::endl;

         header << "      /// Turn (selected) branches either on or off"
                << std::endl;
         header << "      virtual void SetActive( ::Bool_t active = kTRUE,"
                << std::endl;
         header << "                              const ::TString& pattern = "
                << "\".*\" );" << std::endl;
         header << "      /// Read in all the variables that we need to write "
                << "out as well" << std::endl;
         header << "      virtual void ReadAllActive();" << std::endl
                << std::endl;

         header << "      /// Get the D3PD reading statistics" << std::endl;
         header << "      virtual D3PDReadStats GetStatistics() const;"
                << std::endl << std::endl;

         header << "      /// Set the contents of the object according to "
                << "another object" << std::endl;
         header << "      " << classname << "& Set( const " << classname
                << "& parent );" << std::endl << std::endl;

         if( metadata.container() ) {
            header << "      /// Clear the container. Useful when writing new "
                   << "branches." << std::endl;
            header << "      void Clear( Option_t* option = \"\" );"
                   << std::endl;
            header << "      /// Add one element to an output collection"
                   << std::endl;
            header << "      " << classname << "& Add( const " << classname
                   << "Element& el );" << std::endl << std::endl;
            header << "      /// Access a proxy class describing one element of"
                   << " the container" << std::endl;
            header << "      " << classname << "Element& operator[]( size_t "
                   << "index );" << std::endl;
            header << "      /// Access a proxy class describing one element of"
                   << " the container (constant version)" << std::endl;
            header << "      const " << classname << "Element& operator[]( "
                   << "size_t index ) const;" << std::endl;
            header << "      /// Add one element to an output collection"
                   << std::endl;
            header << "      " << classname << "& operator+=( const "
                   << classname << "Element& el );" << std::endl << std::endl;
         }

         //
         // Declare each variable responsible for reading a branch:
         //
         std::map< std::string, int > variableCounter;
         std::set< ObjectMetadata::Variable >::const_iterator itr =
            metadata.variables().begin();
         std::set< ObjectMetadata::Variable >::const_iterator end =
            metadata.variables().end();
         for( ; itr != end; ++itr ) {
            if( itr->doc() != "" ) {
               header << "      /// " << itr->doc() << std::endl; 
            }
            if( variableCounter.find( itr->name() ) == variableCounter.end() ) {
               header << "      VarHandle< " << itr->type()
                      << ( itr->primitive() ? "" : "*" ) << " > "
                      << variableName( itr->name() ) << ";" << std::endl;
               variableCounter[ itr->name() ] = 1;
            } else {
               header << "      VarHandle< " << itr->type()
                      << ( itr->primitive() ? "" : "*" ) << " > "
                      << variableName( itr->name() )
                      << variableCounter[ itr->name() ] << ";" << std::endl;
               variableCounter[ itr->name() ]++;
            }
         }

         // Declare the private functions/members:
         header << std::endl << "   private:" << std::endl;
         header << "      /// Function used internally to access the variables"
                << std::endl;
         header << "      VarHandleBase* GetVarHandle( const char* name );"
                << std::endl;
         header << "      /// Function setting up all the VarHandle members"
                << std::endl;
         header << "      void SetVarHandles( const ::Long64_t* master );"
                << std::endl << std::endl;

         // Declare the additional member variable(s):
         if( metadata.container() ) {
            header << "      mutable std::vector< " << classname << "Element* >"
                   << " fProxies; ///< Internal list of proxy objects"
                   << std::endl;
         }
         header << "      std::map< ::TString, VarHandleBase* > fHandles; "
                << "///< Book-keeper of the VarHandle members" << std::endl;
         header << "      const ::Bool_t fFromInput; "
                << "///< Flag specifying if object is used for D3PD reading"
                << std::endl;
         header << "      ::TString fPrefix; ///< Prefix to the branch names"
                << std::endl << std::endl;

         // Close the class definition:
         header << "      ClassDef( " << classname << ", 0 )" << std::endl
                << std::endl;
         header << "   }; // class " << classname << std::endl << std::endl;
         header << "} // namespace D3PDReader" << std::endl << std::endl;
         header << "#endif // D3PDREADER_" << classname << "_H" << std::endl;

         header.close();

         return StatusCode::SUCCESS;
      }

      /**
       * This function is used to generate the source file of a D3PDReader
       * class.
       *
       * @param classname The name of the class to be generated
       * @param dir The directory where the header file should be put
       * @param vars A vector of the variable descriptions
       * @param isContainer A flag specifying whether an element proxy class
       *                    should be generated
       * @returns A StatusCode indicating whether the operation was successful
       */
      StatusCode writeSource( const std::string& classname,
                              const std::string& dir,
                              const ObjectMetadata& metadata ) {

         // Construct the file name:
         const std::string fileName = dir + "/" + classname + ".cxx";

         // If the file already exists, let's not overwrite it:
         struct stat fileInfo;
         if( ! stat( fileName.c_str(), &fileInfo ) ) return StatusCode::SUCCESS;

         // Let everyone know what we're doing:
         REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO, "Version2::writeSource" )
            << "Generating file: " << fileName;

         // Open the header file (overwriting a possibly existing file):
         std::ofstream source( fileName.c_str() );

         // Add the common header to the file:
         source << CODE_COMMENT << std::endl << std::endl;

         if( metadata.container() ) {
            source << "#include <limits>" << std::endl;
            source << "#include <stdexcept>" << std::endl << std::endl;
         }

         // Extra ROOT include(s):
         source << "#include <TPRegexp.h>" << std::endl;
         source << "#include <TList.h>" << std::endl;
         source << "#include <TDataMember.h>" << std::endl << std::endl;

         // Include the class's header:
         source << "#include \"" << classname << ".h\"" << std::endl
                << std::endl;

         if( metadata.container() ) {
            source << "ClassImp( D3PDReader::" << classname << "Element )"
                   << std::endl;
         }
         source << "ClassImp( D3PDReader::" << classname << " )" << std::endl
                << std::endl;

         source << "namespace D3PDReader {" << std::endl << std::endl;

         if( metadata.container() ) {
            //
            // Produce the constructor of the proxy class:
            //
            const std::string spacer( ( 2 * classname.size() ) + 21, ' ' );
            source << "   /**" << std::endl;
            source << "    * This constructor can be used to create new proxy "
                   << "objects from scratch." << std::endl;
            source << "    * Since the constructor of such an object is quite "
                   << "complicated and" << std::endl;
            source << "    * error prone, it is only visible to the parent "
                   << "class " << classname << "." << std::endl;
            source << "    */" << std::endl;
            source << "   " << classname << "Element::" << classname
                   << "Element( size_t index, const " << classname
                   << "& parent )" << std::endl;
            source << "      : UserD3PDObjectElement( index, parent )";
            std::map< std::string, int > variableCounter;
            std::set< ObjectMetadata::Variable >::const_iterator itr =
               metadata.variables().begin();
            std::set< ObjectMetadata::Variable >::const_iterator end =
               metadata.variables().end();
            for( ; itr != end; ++itr ) {
               // Ignore the container size variable:
               if( ( itr->name() == "n" ) || ( itr->name() == "N" ) ) continue;

               source << "," << std::endl;
               if( variableCounter.find( itr->name() ) ==
                   variableCounter.end() ) {
                  source << "        " << variableName( itr->name() )
                         << "( parent." << variableName( itr->name() )
                         << ", index )";
                  variableCounter[ itr->name() ] = 1;
               } else {
                  source << "        " << variableName( itr->name() )
                         << variableCounter[ itr->name() ]
                         << "( parent." << variableName( itr->name() )
                         << variableCounter[ itr->name() ] << ", index )";
                  variableCounter[ itr->name() ]++;
               }
            }
            source << "," << std::endl;
            source << "        fParent( &parent ), fIndex( index ) {"
                   << std::endl << std::endl;
            source << "   }" << std::endl << std::endl;

            //
            // Produce the copy-constructor of the proxy class:
            //
            source << "   /**" << std::endl;
            source << "    * This constructor is useful for creating copies of "
                   << "proxy objects." << std::endl;
            source << "    * Such objects are fairly cheap to copy, so the user"
                   << " is allowed to have" << std::endl;
            source << "    * his/her containers of them inside an analysis "
                   << "code. (To select and" << std::endl;
            source << "    * sort objects according to some criteria for "
                   << "instance.)" << std::endl;
            source << "    *" << std::endl;
            source << "    * @param parent The proxy object that should be "
                   << "copied" << std::endl;
            source << "    */" << std::endl;
            source << "   " << classname << "Element::" << classname
                   << "Element( const " << classname << "Element& parent )"
                   << std::endl;
            source << "      : UserD3PDObjectElement( parent )";
            variableCounter.clear();
            itr = metadata.variables().begin();
            end = metadata.variables().end();
            for( ; itr != end; ++itr ) {
               // Ignore the container size variable:
               if( ( itr->name() == "n" ) || ( itr->name() == "N" ) ) continue;

               source << "," << std::endl;
               if( variableCounter.find( itr->name() ) ==
                   variableCounter.end() ) {
                  source << "        " << variableName( itr->name() )
                         << "( parent." << variableName( itr->name() )
                         << " )";
                  variableCounter[ itr->name() ] = 1;
               } else {
                  source << "        " << variableName( itr->name() )
                         << variableCounter[ itr->name() ]
                         << "( parent." << variableName( itr->name() )
                         << variableCounter[ itr->name() ] << " )";
                  variableCounter[ itr->name() ]++;
               }
            }
            source << "," << std::endl;
            source << "        fParent( parent.fParent ), "
                   << "fIndex( parent.fIndex ) {" << std::endl << std::endl;
            source << "   }" << std::endl << std::endl;

            //
            // Produce the GetParent() function of the proxy class:
            //
            source << "   /**" << std::endl;
            source << "    * This function can be used to access the parent "
                   << "object of this" << std::endl;
            source << "    * proxy object. It can come in handy when optimizing"
                   << " an analysis code." << std::endl;
            source << "    *" << std::endl;
            source << "    * @returns A reference to this object's parent"
                   << std::endl;
            source << "    */" << std::endl;
            source << "   const " << classname << "* " << classname
                   << "Element::GetParent() const {" << std::endl << std::endl;
            source << "      return fParent;" << std::endl;
            source << "   }" << std::endl << std::endl;

            //
            // Produce the GetIndex() function of the proxy class:
            //
            source << "   /**" << std::endl;
            source << "    * This function can be used to access the index of "
                   << "this proxy object" << std::endl;
            source << "    * inside the parent container. It can come in handy "
                   << "when optimizing an" << std::endl;
            source << "    * analysis code." << std::endl;
            source << "    *" << std::endl;
            source << "    * @returns The index of the proxy in the object's "
                   << "parent" << std::endl;
            source << "    */" << std::endl;
            source << "   size_t " << classname << "Element::GetIndex() const {"
                   << std::endl << std::endl;
            source << "      return fIndex;" << std::endl;
            source << "   }" << std::endl << std::endl;
         }

         //
         // Produce the (input) constructor:
         //
         source << "   /**" << std::endl;
         source << "    * This constructor should be used when the object will "
                << "be used to read" << std::endl;
         source << "    * variables from an existing ntuple. The object will "
                << "also be able to" << std::endl;
         source << "    * output variables, but it will also need to read them "
                << "from somewhere." << std::endl;
         source << "    *" << std::endl;
         source << "    * @param master Reference to the variable holding the "
                << "current " << "event number" << std::endl;
         source << "    * @param prefix Prefix of the variables in the D3PD"
                << std::endl;
         source << "    */" << std::endl;
         source << "   " << classname << "::" << classname
                << "( const ::Long64_t& master, const char* prefix )"
                << std::endl;
         source << "      : UserD3PDObject( master, prefix ),"
                << std::endl;
         source << "        fHandles()," << std::endl;
         source << "        fFromInput( kTRUE )," << std::endl;
         source << "        fPrefix( prefix ) {" << std::endl << std::endl;
         source << "      SetVarHandles( &master );" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the (output) constructor:
         //
         source << "   /**" << std::endl;
         source << "    * This constructor can be used when the object will "
                << "only have to output" << std::endl;
         source << "    * (and temporarily store) new information into an "
                << "output ntuple. For" << std::endl;
         source << "    * instance when one wants to create a selected/modified"
                << " list of information." << std::endl;
         source << "    *" << std::endl;
         source << "    * @param prefix Prefix of the variables in the D3PD"
                << std::endl;
         source << "    */" << std::endl;
         source << "   " << classname << "::" << classname
                << "( const char* prefix )" << std::endl;
         source << "      : UserD3PDObject( prefix ),"
                << std::endl;
         source << "        fHandles()," << std::endl;
         source << "        fFromInput( kFALSE )," << std::endl;
         source << "        fPrefix( prefix ) {" << std::endl << std::endl;
         source << "      SetVarHandles( 0 );" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the destructor:
         //
         if( metadata.container() ) {
            source << "   /**" << std::endl;
            source << "    * The destructor needs to delete all the allocated "
                   << "objects." << std::endl;
            source << "    */" << std::endl;
            source << "   " << classname << "::~" << classname << "() {"
                   << std::endl
                   << std::endl;
            source << "      for( size_t i = 0; i < fProxies.size(); ++i ) {"
                   << std::endl;
            source << "         delete fProxies[ i ];" << std::endl;
            source << "      }" << std::endl;
            source << "   }" << std::endl << std::endl;
         }

         //
         // Produce the prefix handling function(s):
         //
         source << "   /**" << std::endl;
         source << "    * @returns The branch name prefix used by the object"
                << std::endl;
         source << "    */" << std::endl;
         source << "   const char* " << classname << "::GetPrefix() const {"
                << std::endl << std::endl;
         source << "      return fPrefix;" << std::endl;
         source << "   }" << std::endl << std::endl;

         source << "   /**" << std::endl;
         source << "    * @param prefix The prefix that should be used for the "
                << "variables" << std::endl;
         source << "    */" << std::endl;
         source << "   void " << classname << "::SetPrefix( const char* prefix "
                << ") {" << std::endl << std::endl;
         source << "      // Call the base class's function:" << std::endl;
         source << "      UserD3PDObject::SetPrefix( prefix );" << std::endl
                << std::endl;
         source << "      // Remember the prefix:" << std::endl;
         source << "      fPrefix = prefix;" << std::endl << std::endl;
         source << "      // Set all the variable names:" << std::endl;
         source << "      std::map< TString, VarHandleBase* >::const_iterator "
                << "itr = fHandles.begin();" << std::endl;
         source << "      std::map< TString, VarHandleBase* >::const_iterator "
                << "end = fHandles.end();" << std::endl;
         source << "      for( ; itr != end; ++itr ) {" << std::endl;
         source << "         itr->second->SetName( ::TString( prefix ) + "
                << "itr->first );" << std::endl;
         source << "      }" << std::endl << std::endl;
         source << "      return;" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the ReadFrom(...) function:
         //
         source << "   /**" << std::endl;
         source << "    * This function should be called every time a new TFile"
                << " is opened" << std::endl;
         source << "    * by your analysis code." << std::endl;
         source << "    *" << std::endl;
         source << "    * @param tree Pointer to the TTree with the variables"
                << std::endl;
         source << "    */" << std::endl;
         source << "   void " << classname << "::ReadFrom( TTree* tree ) {"
                << std::endl << std::endl;
         source << "      // Check if the object will be able to read from the "
                << "TTree:" << std::endl;
         source << "      if( ! fFromInput ) {" << std::endl;
         source << "         Error( \"ReadFrom\", \"The object was not created "
                << "with the correct\" );" << std::endl;
         source << "         Error( \"ReadFrom\", \"constructor to read data "
                << "from a D3PD!\" );" << std::endl;
         source << "         return;" << std::endl;
         source << "      }" << std::endl << std::endl;
         source << "      // Call the base class's function:" << std::endl;
         source << "      UserD3PDObject::ReadFrom( tree );" << std::endl
                << std::endl;
         source << "      // Call ReadFrom(...) on all the variables:"
                << std::endl;
         source << "      std::map< TString, VarHandleBase* >::const_iterator "
                << "itr = fHandles.begin();" << std::endl;
         source << "      std::map< TString, VarHandleBase* >::const_iterator "
                << "end = fHandles.end();" << std::endl;
         source << "      for( ; itr != end; ++itr ) {" << std::endl;
         source << "         itr->second->ReadFrom( tree );" << std::endl;
         source << "      }" << std::endl << std::endl;
         source << "      return;" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the WriteTo(...) function:
         //
         source << "   /**" << std::endl;
         source << "    * This function can be called to connect the active "
                << "variables of the object" << std::endl;
         source << "    * to an output TTree. It can be called multiple times, "
                << "then the variables" << std::endl;
         source << "    * will be written to multiple TTrees." << std::endl;
         source << "    *" << std::endl;
         source << "    * @param tree Pointer to the TTree where the variables "
                << "should be written" << std::endl;
         source << "    */" << std::endl;
         source << "   void " << classname << "::WriteTo( TTree* tree ) {"
                << std::endl << std::endl;
         source << "      // Call the base class's function:" << std::endl;
         source << "      UserD3PDObject::WriteTo( tree );" << std::endl
                << std::endl;
         source << "      // Call WriteTo(...) on all the variables:"
                << std::endl;
         source << "      std::map< TString, VarHandleBase* >::const_iterator "
                << "itr = fHandles.begin();" << std::endl;
         source << "      std::map< TString, VarHandleBase* >::const_iterator "
                << "end = fHandles.end();" << std::endl;
         source << "      for( ; itr != end; ++itr ) {" << std::endl;
         source << "         itr->second->WriteTo( tree );" << std::endl;
         source << "      }" << std::endl << std::endl;
         source << "      return;" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the SetActive(...) function:
         //
         source << "   /**" << std::endl;
         source << "    * This is a convenience function for turning the "
                << "branches active or" << std::endl;
         source << "    * inactive conveniently. If the parameter is set to "
                << "<code>kTRUE</code>" << std::endl;
         source << "    * then the branches available from the input which "
                << "match the given" << std::endl;
         source << "    * pattern are turned active."
                << std::endl;
         source << "    * When it's set to <code>kFALSE</code> then all the "
                << "variables matching" << std::endl;
         source << "    * the pattern are turned inactive." << std::endl;
         source << "    *" << std::endl;
         source << "    * @param active Flag behaving as explained above"
                << std::endl;
         source << "    * @param pattern Regular expression specifying which "
                << "branches to modify" << std::endl;
         source << "    */" << std::endl;
         source << "   void " << classname << "::SetActive( ::Bool_t active, "
                << "const ::TString& pattern ) {" << std::endl
                << std::endl;
         source << "      // Call the base class's function:" << std::endl;
         source << "      UserD3PDObject::SetActive( active, pattern );"
                << std::endl << std::endl;
         source << "      ::TPRegexp re( pattern );" << std::endl << std::endl;
         source << "      std::map< TString, VarHandleBase* >::const_iterator "
                << "itr = fHandles.begin();" << std::endl;
         source << "      std::map< TString, VarHandleBase* >::const_iterator "
                << "end = fHandles.end();" << std::endl;
         source << "      for( ; itr != end; ++itr ) {" << std::endl;
         source << "         if( ! re.Match( fPrefix + itr->first ) ) continue;"
                << std::endl;
         source << "         if( active ) {" << std::endl;
         source << "            if( itr->second->IsAvailable() ) "
                << "itr->second->SetActive( active );" << std::endl;
         source << "         } else {" << std::endl;
         source << "            itr->second->SetActive( active );" << std::endl;
         source << "         }" << std::endl;
         source << "      }" << std::endl << std::endl;
         source << "      return;" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the ReadAllActive(...) function:
         //
         source << "   /**" << std::endl;
         source << "    * This function can be used to read in all the branches"
                << " from the input" << std::endl;
         source << "    * TTree which are set active for writing out. This can "
                << "simplify writing" << std::endl;
         source << "    * event selector codes immensely. Remember to set the "
                << "desired variable" << std::endl;
         source << "    * active before calling this function." << std::endl;
         source << "    */" << std::endl;
         source << "   void " << classname << "::ReadAllActive() {" << std::endl
                << std::endl;
         source << "      // Check if it makes sense to call this function:"
                << std::endl;
         source << "      if( ! fFromInput ) {" << std::endl;
         source << "         static ::Bool_t wPrinted = kFALSE;" << std::endl;
         source << "         if( ! wPrinted ) {" << std::endl;
         source << "            Warning( \"ReadAllActive\", "
                << "\"Function only meaningful when used on objects\" );"
                << std::endl;
         source << "            Warning( \"ReadAllActive\", "
                << "\"which are used to read information from a D3PD\" );"
                << std::endl;
         source << "            wPrinted = kTRUE;" << std::endl;
         source << "         }" << std::endl;
         source << "      }" << std::endl << std::endl;
         source << "      // Call the base class's function:" << std::endl;
         source << "      UserD3PDObject::ReadAllActive();" << std::endl
                << std::endl;
         source << "      // Read in the current entry for each active "
                << "variable:" << std::endl;
         source << "      std::map< TString, VarHandleBase* >::const_iterator "
                << "itr = fHandles.begin();" << std::endl;
         source << "      std::map< TString, VarHandleBase* >::const_iterator "
                << "end = fHandles.end();" << std::endl;
         source << "      for( ; itr != end; ++itr ) {" << std::endl;
         source << "         if( ! itr->second->IsActive() ) continue;"
                << std::endl;
         source << "         itr->second->ReadCurrentEntry();" << std::endl;
         source << "      }" << std::endl << std::endl;
         source << "      return;" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the GetStatistics() function:
         //
         source << "   /**" << std::endl;
         source << "    * This function can be used to get information about "
                << "the access" << std::endl;
         source << "    * pattern/statistics of the job. It should be called "
                << "at the end of" << std::endl;
         source << "    * an analysis job to get the information about the "
                << "performance of the" << std::endl;
         source << "    * analysis." << std::endl;
         source << "    *" << std::endl;
         source << "    * @returns An object describing the D3PD access "
                << "statistics" << std::endl;
         source << "    */" << std::endl;
         source << "   D3PDReadStats " << classname
                << "::GetStatistics() const {" << std::endl << std::endl;
         source << "      // The result object:" << std::endl;
         source << "      D3PDReadStats result = "
                << "UserD3PDObject::GetStatistics();" << std::endl << std::endl;
         source << "      // Add the statistics from each variable to the "
                << "result:" << std::endl;
         source << "      std::map< ::TString, VarHandleBase* >::const_iterator"
                << " itr = fHandles.begin();" << std::endl;
         source << "      std::map< ::TString, VarHandleBase* >::const_iterator"
                << " end = fHandles.end();" << std::endl;
         source << "      for( ; itr != end; ++itr ) {" << std::endl;
         source << "         result.AddVariable( itr->second->GetStatistics() );"
                << std::endl;
         source << "      }" << std::endl << std::endl;
         source << "      return result;" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the Set function:
         //
         source << "   /**" << std::endl;
         source << "    * This function can be used to copy the contents of the"
                << " entire object" << std::endl;
         source << "    * for a given event. This can be useful for instance "
                << "when the user" << std::endl;
         source << "    * wants to copy all information to an output file, and "
                << "modify it a bit," << std::endl;
         source << "    * and only then write it out." << std::endl;
         source << "    *" << std::endl;
         source << "    * @param parent The object to copy the information from"
                << std::endl;
         source << "    * @returns This same object, for convenience reasons"
                << std::endl;
         source << "    */" << std::endl;
         source << "   " << classname << "& " << classname << "::Set( const "
                << classname << "& parent ) {" << std::endl << std::endl;
         source << "      // Check if this function can be used on the object:"
                << std::endl;
         source << "      if( fFromInput ) {" << std::endl;
         source << "         Error( \"Set\", "
                << "\"Objects used for reading a D3PD can't be modified!\" );"
                << std::endl;
         source << "         return *this;" << std::endl;
         source << "      }" << std::endl << std::endl;
         source << "      // Call the base class's function:" << std::endl;
         source << "      UserD3PDObject::Set( parent );" << std::endl
                << std::endl;
         std::map< std::string, int > variableCounter;
         std::set< ObjectMetadata::Variable >::const_iterator itr =
            metadata.variables().begin();
         std::set< ObjectMetadata::Variable >::const_iterator end =
            metadata.variables().end();
         for( ; itr != end; ++itr ) {
            std::string postfix = "";
            if( variableCounter.find( itr->name() ) == variableCounter.end() ) {
               variableCounter[ itr->name() ] = 1;
            } else {
               postfix =
                  boost::lexical_cast< std::string >( variableCounter[ itr->name() ] );
               variableCounter[ itr->name() ]++;
            }
            source << "      if( parent." << variableName( itr->name() )
                   << postfix << ".IsAvailable() && "
                   << variableName( itr->name() ) << postfix
                   << ".IsActive() ) {" << std::endl;
            if( itr->primitive() ) {
               source << "         " << variableName( itr->name() ) << postfix
                      << "() = parent." << variableName( itr->name() )
                      << postfix << "();" << std::endl;
            } else {
               source << "         *( " << variableName( itr->name() )
                      << postfix << "() ) = *( parent."
                      << variableName( itr->name() ) << postfix
                      << "() );" << std::endl;
            }
            source << "      } else {" << std::endl;
            if( itr->primitive() ) {
               source << "         " << variableName( itr->name() )
                      << postfix << "() = 0;" << std::endl;
            } else {
               source << "         " << variableName( itr->name() )
                      << postfix << "()->clear();" << std::endl;
            }
            source << "      }" << std::endl;
         }
         source << std::endl << "      return *this;" << std::endl;
         source << "   }" << std::endl << std::endl;

         if( metadata.container() ) {
            //
            // Produce the Clear function:
            //
            source << "   /**" << std::endl;
            source << "    * This function makes it easier to clear out the "
                   << "object completely." << std::endl;
            source << "    * It cleares all the vector variables, and sets the "
                   << "element number" << std::endl;
            source << "    * variable to 0. Very useful when performing object "
                   << "selection." << std::endl;
            source << "    * The option argument is not used at the moment for "
                   << "anything." << std::endl;
            source << "    * It's only there because the <code>Clear</code> "
                   << "function defined in" << std::endl;
            source << "    * TObject has this parameter as well." << std::endl;
            source << "    *" << std::endl;
            source << "    * @param option Ignored at the moment" << std::endl;
            source << "    */" << std::endl;
            source << "   void " << classname << "::Clear( Option_t* opt ) {"
                   << std::endl << std::endl;
            source << "      // Check if this function can be used on the "
                   << "object:" << std::endl;
            source << "      if( fFromInput ) {" << std::endl;
            source << "         Error( \"Clear\", "
                   << "\"Objects used for reading a D3PD can't be cleared!\" );"
                   << std::endl;
            source << "         return;" << std::endl;
            source << "      }" << std::endl << std::endl;
            source << "      // Call the base class's function:" << std::endl;
            source << "      UserD3PDObject::Clear( opt );" << std::endl
                   << std::endl;
            source << "      // Clear each variable:" << std::endl;
            source << "      std::map< ::TString, VarHandleBase* >::const_iterator"
                   << " itr = fHandles.begin();" << std::endl;
            source << "      std::map< ::TString, VarHandleBase* >::const_iterator"
                   << " end = fHandles.end();" << std::endl;
            source << "      for( ; itr != end; ++itr ) {" << std::endl;
            source << "         itr->second->Clear();" << std::endl;
            source << "      }" << std::endl << std::endl;
            source << "      return;" << std::endl;
            source << "   }" << std::endl << std::endl;

            //
            // Produce the Add function:
            //
            source << "   /**" << std::endl;
            source << "    * This function can be used to easily add an "
                   << "'element' describing one" << std::endl;
            source << "    * object to an output collection. Comes in very "
                   << "handy when performing" << std::endl;
            source << "    * object selection." << std::endl;
            source << "    *" << std::endl;
            source << "    * Note that variables which are not available from "
                   << "the input, will be" << std::endl;
            source << "    * filled with dummy values." << std::endl;
            source << "    *" << std::endl;
            source << "    * @param el The 'element' that should be added to "
                   << "the collection" << std::endl;
            source << "    */" << std::endl;
            source << "   " << classname << "& " << classname << "::Add( const "
                   << classname << "Element& el ) {" << std::endl << std::endl;
            source << "      // Check if this function can be used on the "
                   << "object:" << std::endl;
            source << "      if( fFromInput ) {" << std::endl;
            source << "         Error( \"Add\", \"Objects used for reading a "
                   << "D3PD can't be modified!\" );" << std::endl;
            source << "         return *this;" << std::endl;
            source << "      }" << std::endl << std::endl;
            source << "      // Call the base class's function:" << std::endl;
            source << "      UserD3PDObject::Add( el );" << std::endl
                   << std::endl;
            variableCounter.clear();
            itr = metadata.variables().begin();
            end = metadata.variables().end();
            for( ; itr != end; ++itr ) {
               // Treat the size variable in a special way:
               if( itr->name() == "n" ) {
                  source << "      ++( n() );" << std::endl;
                  continue;
               }
               if( itr->name() == "N" ) {
                  source << "      ++( N() );" << std::endl;
                  continue;
               }

               // Construct a possible post-fix for the variable name:
               std::string postfix = "";
               if( variableCounter.find( itr->name() ) ==
                   variableCounter.end() ) {
                  variableCounter[ itr->name() ] = 1;
               } else {
                  postfix =
                     boost::lexical_cast< std::string >( variableCounter[ itr->name() ] );
                  variableCounter[ itr->name() ]++;
               }

               source << "      if( el." << variableName( itr->name() )
                      << postfix << ".IsAvailable() && "
                      << variableName( itr->name() ) << postfix
                      << ".IsActive() ) {" << std::endl;
               source << "         " << variableName( itr->name() ) << postfix
                      << "()->push_back( el." << variableName( itr->name() )
                      << postfix << "() );" << std::endl;
               source << "      } else {" << std::endl;

               // Extract the type of the variable we're dealing with:
               bool ok = true;
               const std::string type =
                  vectorTemplateArgument( itr->type(), ok );
               if( ! ok ) {
                  REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR,
                                               "Version2::writeSource" )
                     << "Unexpected variable type encountered for "
                     << "container dumper: " << itr->type();
                  return StatusCode::FAILURE;
               }

               // Primitive and complex types have to be treated differently if
               // they don't exist in the input:
               source << "         " << variableName( itr->name() )
                      << postfix << "()->push_back( ";
               if( isPrimitive( type ) ) {
                  source << "std::numeric_limits< " << type << " >::min()";
               } else {
                  source << type << "()";
               }
               source << " );" << std::endl;
               source << "      }" << std::endl;
            }
            source << "      return *this;" << std::endl;
            source << "   }" << std::endl << std::endl;

            //
            // Try to find the size variable's name:
            //
            static const int NSIZEVARS = 2;
            static const char* SIZEVARS[ NSIZEVARS ] = { "n", "N" };
            std::string sizeVariable = "";
            ObjectMetadata::Variable var;
            for( int i = 0; i < NSIZEVARS; ++i ) {
               var.setName( SIZEVARS[ i ] );
               if( metadata.variables().find( var ) !=
                   metadata.variables().end() ) {
                  sizeVariable = var.name() + "()";
                  break;
               }
            }

            //
            // Produce the [] operators:
            //
            source << "   /**" << std::endl;
            source << "    * This operator can be used to get access to one "
                   << "element in the" << std::endl;
            source << "    * collection. This element can then be passed around"
                   << " between parts" << std::endl;
            source << "    * of the analysis code easily." << std::endl;
            source << "    *" << std::endl;
            source << "    * This version is useful when modifying the variable"
                   << " contents through" << std::endl;
            source << "    * the proxy objects." << std::endl;
            source << "    *" << std::endl;
            source << "    * @param index Index of the element inside the "
                   << "collection" << std::endl;
            source << "    */" << std::endl;
            source << "   " << classname << "Element& " << classname
                   << "::operator[]( size_t index ) {" << std::endl
                   << std::endl;
            source << "      // Check whether the index makes sense:"
                   << std::endl;
            source << "      if( index >= static_cast< size_t >( "
                   << ( sizeVariable == "" ? "1000000" : sizeVariable )
                   << " ) ) {" << std::endl;
            source << "         Fatal( \"operator[]\", \"Proxy with index %i"
                   << " requested\", static_cast< int >( index ) );"
                   << std::endl;
            source << "         // The previous should've stopped the code "
                   << "already," << std::endl;
            source << "         // but let's go for sure..." << std::endl;
            source << "         throw std::runtime_error( \"Too large proxy "
                   << "index\" );" << std::endl;
            source << "      }" << std::endl << std::endl;
            source << "      // Make sure that the proxy exists:" << std::endl;
            source << "      while( fProxies.size() <= index ) {" << std::endl;
            source << "         fProxies.push_back( new " << classname
                   << "Element( fProxies.size(), *this ) );" << std::endl;
            source << "      }" << std::endl;
            source << "      return *fProxies[ index ];" << std::endl;
            source << "   }" << std::endl << std::endl;

            source << "   /**" << std::endl;
            source << "    * This operator can be used to get access to one "
                   << "element in the" << std::endl;
            source << "    * collection. This element can then be passed around"
                   << " between parts" << std::endl;
            source << "    * of the analysis code easily." << std::endl;
            source << "    *" << std::endl;
            source << "    * This version is useful when only reading the "
                   << "variables." << std::endl;
            source << "    *" << std::endl;
            source << "    * @param index Index of the element inside the "
                   << "collection" << std::endl;
            source << "    */" << std::endl;
            source << "   const " << classname << "Element& " << classname
                   << "::operator[]( size_t index ) const {" << std::endl
                   << std::endl;
            source << "      // Check whether the index makes sense:"
                   << std::endl;
            source << "      if( index >= static_cast< size_t >( "
                   << ( sizeVariable == "" ? "1000000" : sizeVariable )
                   << " ) ) {" << std::endl;
            source << "         Fatal( \"operator[]\", \"Proxy with index %i"
                   << " requested\", static_cast< int >( index ) );"
                   << std::endl;
            source << "         // The previous should've stopped the code "
                   << "already," << std::endl;
            source << "         // but let's go for sure..." << std::endl;
            source << "         throw std::runtime_error( \"Too large proxy "
                   << "index\" );" << std::endl;
            source << "      }" << std::endl << std::endl;
            source << "      // Make sure that the proxy exists:" << std::endl;
            source << "      while( fProxies.size() <= index ) {" << std::endl;
            source << "         fProxies.push_back( new " << classname
                   << "Element( fProxies.size(), *this ) );" << std::endl;
            source << "      }" << std::endl;
            source << "      return *fProxies[ index ];" << std::endl;
            source << "   }" << std::endl << std::endl;

            //
            // Produce the += operator:
            //
            source << "   /**" << std::endl;
            source << "    * A convenience operator for adding an 'element' to "
                   << "this collection." << std::endl;
            source << "    *" << std::endl;
            source << "    * @see Add" << std::endl;
            source << "    * @param el The 'element' that should be added to "
                   << "the collection" << std::endl;
            source << "    */" << std::endl;
            source << "   " << classname << "& " << classname
                   << "::operator+=( const " << classname << "Element& el ) {"
                   << std::endl << std::endl;
            source << "      return this->Add( el );" << std::endl;
            source << "   }" << std::endl << std::endl;
         }

         //
         // Produce the GetVarHandle(...) function:
         //
         source << "   /**" << std::endl;
         source << "    * This function is used internally to access VarHandle "
                << "members" << std::endl;
         source << "    * by name. This is necessary to push some setup from "
                << "compile time" << std::endl;
         source << "    * to run time. It may sound weird, but it makes a lot "
                << "of sense for large" << std::endl;
         source << "    * classes." << std::endl;
         source << "    *" << std::endl;
         source << "    * @param name The name of the C++ variable (not of the "
                << "branch)" << std::endl;
         source << "    * @returns A pointer to the VarHandle object"
                << std::endl;
         source << "    */" << std::endl;
         source << "   VarHandleBase* " << classname
                << "::GetVarHandle( const char* name ) {" << std::endl
                << std::endl;
         variableCounter.clear();
         itr = metadata.variables().begin();
         end = metadata.variables().end();
         for( ; itr != end; ++itr ) {
            // Construct a possible post-fix for the variable name:
            std::string postfix = "";
            if( variableCounter.find( itr->name() ) ==
                variableCounter.end() ) {
               variableCounter[ itr->name() ] = 1;
            } else {
               postfix =
                  boost::lexical_cast< std::string >( variableCounter[ itr->name() ] );
               variableCounter[ itr->name() ]++;
            }
            if( itr == metadata.variables().begin() ) {
               source << "      if( ! ::strcmp( name, \""
                      << variableName( itr->name() ) << postfix << "\" ) ) {"
                      << std::endl;
            } else {
               source << "      else if( ! ::strcmp( name, \""
                      << variableName( itr->name() ) << postfix << "\" ) ) {"
                      << std::endl;
            }
            source << "         return &" << variableName( itr->name() )
                   << postfix << ";" << std::endl;
            source << "      }" << std::endl;
         }
         source << std::endl << "      Error( \"GetVarHandle\", \"Variable \\\"%s"
                << "\\\" unknown\", name );" << std::endl;
         source << "      return 0;" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the SetVarHandles(...) function:
         //
         source << "   /**" << std::endl;
         source << "    * This function is used internally to set up all the "
                << "VarHandle members" << std::endl;
         source << "    * of the class. It speeds up compilation *a lot* to do "
                << "this at run-time" << std::endl;
         source << "    * like this, instead of putting a lot of lines of code "
                << "operating on" << std::endl;
         source << "    * the std::map member." << std::endl;
         source << "    *" << std::endl;
         source << "    * @param master Pointer to the master index, or a null "
                << "pointer" << std::endl;
         source << "    */" << std::endl;
         source << "   void " << classname << "::SetVarHandles( const "
                << "::Long64_t* master ) {" << std::endl << std::endl;
         source << "      // Create a list of variable-branch name pairs:"
                << std::endl;
         source << "      static const Int_t NVARNAMES = "
                << metadata.variables().size() << ";" << std::endl;
         source << "      static const char* VARNAMES[ NVARNAMES ][ 2 ] = {"
                << std::endl;
         variableCounter.clear();
         itr = metadata.variables().begin();
         end = metadata.variables().end();
         for( ; itr != end; ++itr ) {
            // Construct a possible post-fix for the variable name:
            std::string postfix = "";
            if( variableCounter.find( itr->name() ) ==
                variableCounter.end() ) {
               variableCounter[ itr->name() ] = 1;
            } else {
               postfix =
                  boost::lexical_cast< std::string >( variableCounter[ itr->name() ] );
               variableCounter[ itr->name() ]++;
            }
            // Add the names:
            source << "         { \"" << variableName( itr->name() ) << postfix
                   << "\", \"" << variableName( itr->name() ) << "\" }";
            if( ++itr != end ) {
               source << ",";
            }
            --itr;
            source << std::endl;
         }
         source << "      };" << std::endl << std::endl;
         source << "      // Set up the fHandles map using this list:"
                << std::endl;
         source << "      for( Int_t i = 0; i < NVARNAMES; ++i ) {"
                << std::endl;
         source << "         VarHandleBase* vh = "
                << "GetVarHandle( VARNAMES[ i ][ 0 ] );" << std::endl;
         source << "         vh->SetName( fPrefix + VARNAMES[ i ][ 1 ] );"
                << std::endl;
         source << "         vh->SetMaster( master );" << std::endl;
         source << "         fHandles[ VARNAMES[ i ][ 0 ] ] = vh;" << std::endl;
         source << "      }" << std::endl << std::endl;

         /*
         source << "      // Access the class's dictionary:" << std::endl;
         source << "      ::TClass* cl = IsA();" << std::endl;
         source << "      // Get a list of all members:" << std::endl;
         source << "      ::TList* members = cl->GetListOfDataMembers();"
                << std::endl;
         source << "      // Set up all the VarHandle members:" << std::endl;
         source << "      for( ::Int_t i = 0; i < members->GetSize(); ++i ) {"
                << std::endl;
         source << "         // Check that it's a VarHandle:" << std::endl;
         source << "         ::TDataMember* m = dynamic_cast< ::TDataMember* >("
                << " members->At( i ) );" << std::endl;
         source << "         if( ! m ) {" << std::endl;
         source << "            Fatal( \"SetVarHandles\", \"Found a wrong "
                << "data member\" );" << std::endl;
         source << "            continue;" << std::endl;
         source << "         }" << std::endl;
         source << "         const ::TString type = m->GetTypeName();"
                << std::endl;
         source << "         if( ! type.BeginsWith( \"D3PDReader::VarHandle<\" "
                << ") ) {" << std::endl;
         source << "            continue;" << std::endl;
         source << "         }" << std::endl;
         source << "         // If it is, let's set it up:" << std::endl;
         source << "         VarHandleBase* vh = GetVarHandle( m->GetName() );"
                << std::endl;
         source << "         fHandles[ m->GetName() ] = vh;" << std::endl;
         source << "      }" << std::endl << std::endl;
         source << "      // Now configure all variables in a second loop:"
                << std::endl;
         source << "      std::map< TString, VarHandleBase* >::const_iterator "
                << "itr = fHandles.begin();" << std::endl;
         source << "      std::map< TString, VarHandleBase* >::const_iterator "
                << "end = fHandles.end();" << std::endl;
         source << "      for( ; itr != end; ++itr ) {" << std::endl;
         source << "         itr->second->SetName( fPrefix + itr->first );"
                << std::endl;
         source << "         itr->second->SetMaster( master );" << std::endl;
         source << "      }" << std::endl << std::endl;
         */
         source << "      return;" << std::endl;
         source << "   }" << std::endl << std::endl;

         source << "} // namespace D3PDReader" << std::endl;

         source.close();

         return StatusCode::SUCCESS;
      }

      StatusCode writeEventHeader( const std::string& classname, const std::string& dir,
                                   const std::set< ObjectMetadata >& metadata ) {

         // Construct the file name:
         const std::string fileName = dir + "/" + classname + ".h";

         // If the file already exists, let's not overwrite it:
         struct stat fileInfo;
         if( ! stat( fileName.c_str(), &fileInfo ) ) return StatusCode::SUCCESS;

         // Let everyone know what we're doing:
         REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO, "Version2::writeEventHeader" )
            << "Generating file: " << fileName;

         // Open the header file (overwriting a possibly existing file):
         std::ofstream header( fileName.c_str() );

         // Write a header for the file:
         header << CODE_COMMENT << std::endl;
         header << "#ifndef D3PDREADER_" << classname << "_H" << std::endl;
         header << "#define D3PDREADER_" << classname << "_H" << std::endl << std::endl;

         //
         // Construct the includes:
         //
         header << "// Local include(s):" << std::endl;
         header << "#include \"D3PDObjectBase.h\"" << std::endl;
         std::set< std::string > d3pd_reader_names;
         std::set< ObjectMetadata >::const_iterator meta_itr = metadata.begin();
         std::set< ObjectMetadata >::const_iterator meta_end = metadata.end();
         for( ; meta_itr != meta_end; ++meta_itr ) {
            // Check if we already included this class:
            std::pair< std::set< std::string >::iterator, bool > ret =
               d3pd_reader_names.insert( meta_itr->name() );
            if( ! ret.second ) continue;
            // If not, then:
            header << "#include \"" << meta_itr->name() << ".h\"" << std::endl;
         }
         header << std::endl;

         header << "namespace D3PDReader {" << std::endl << std::endl;

         //
         // Declare the event class:
         //
         header << "   /**" << std::endl;
         header << "    * Code generated by CodeGenerator_v2 on:" << std::endl;
         header << "    *  host = " << System::hostName() << std::endl;
         header << "    *  OS   = " << System::osName() << " / " << System::osVersion()
                << std::endl;
         header << "    *  user = " << System::accountName() << std::endl;
         time_t rawtime = time( NULL );
         header << "    *  time = " << ctime( &rawtime );
         header << "    */" << std::endl;
         header << "   class " << classname << " : public D3PDObjectBase {" << std::endl
                << std::endl;
         header << "   public:" << std::endl;

         //
         // Declare the constructors:
         //
         header << "      /// Default constructor, to be used when reading a D3PD" << std::endl;
         header << "      " << classname << "();" << std::endl;
         header << "      /// Constructor for only writing data" << std::endl;
         header << "      " << classname << "( Bool_t onlyForWriting );" << std::endl
                << std::endl;

         //
         // Declare some functions:
         //
         header << "      /// Get the currently configured prefix value" << std::endl;
         header << "      virtual const char* GetPrefix() const;" << std::endl;
         header << "      /// Set the prefix for the variables" << std::endl;
         header << "      virtual void SetPrefix( const char* prefix );" << std::endl
                << std::endl;

         header << "      /// Connect the object to an input TTree" << std::endl;
         header << "      virtual void ReadFrom( ::TTree* tree );" << std::endl;
         header << "      /// Connect the object to an output TTree" << std::endl;
         header << "      virtual void WriteTo( ::TTree* tree );" << std::endl << std::endl;

         header << "      /// Turn (selected) branches either on or off" << std::endl;
         header << "      virtual void SetActive( ::Bool_t active = kTRUE," << std::endl;
         header << "                              const ::TString& pattern = \".*\" );"
                << std::endl;
         header << "      /// Read in all the variables that we need to write out as well"
                << std::endl;
         header << "      virtual void ReadAllActive();" << std::endl << std::endl;
         header << "      /// Get the D3PD reading statistics" << std::endl;
         header << "      virtual D3PDReadStats GetStatistics() const;" << std::endl
                << std::endl;

         header << "      /// Switch the reader object to a new event" << std::endl;
         header << "      void GetEntry( ::Long64_t entry );" << std::endl << std::endl;

         //
         // Declare each object:
         //
         header << "      //" << std::endl;
         header << "      // All the components of the D3PD:" << std::endl;
         header << "      //" << std::endl;
         FOR_ALL_EVENT_VARIABLES( header << "      " << meta_itr->name() << " " << varname
                                  << ";" << std::endl );

         // Declare the additional member variable(s):
         header << std::endl << "   private:" << std::endl;
         header << "      Long64_t fEntry; ///< Variable storing the current entry number"
                << std::endl;
         header << "      Bool_t fFromInput; "
                << "///< Flag specifying if object is used for D3PD reading"
                << std::endl << std::endl;

         // Close the class definition:
         header << "      ClassDef( " << classname << ", 0 )" << std::endl << std::endl;
         header << "   }; // class " << classname << std::endl << std::endl;
         header << "} // namespace D3PDReader" << std::endl << std::endl;
         header << "#endif // D3PDREADER_" << classname << "_H" << std::endl;

         header.close();

         return StatusCode::SUCCESS;
      }

      StatusCode writeEventSource( const std::string& classname, const std::string& dir,
                                   const std::set< ObjectMetadata >& metadata ) {

         // Construct the file name:
         const std::string fileName = dir + "/" + classname + ".cxx";

         // If the file already exists, let's not overwrite it:
         struct stat fileInfo;
         if( ! stat( fileName.c_str(), &fileInfo ) ) return StatusCode::SUCCESS;

         // Let everyone know what we're doing:
         REPORT_MESSAGE_WITH_CONTEXT( MSG::INFO, "Version2::writeSource" )
            << "Generating file: " << fileName;

         // Open the header file (overwriting a possibly existing file):
         std::ofstream source( fileName.c_str() );

         // Add the common header to the file:
         source << CODE_COMMENT << std::endl << std::endl;

         // Include the class's header:
         source << "#include \"" << classname << ".h\"" << std::endl;
         source << "#include \"D3PDPerfStats.h\"" << std::endl << std::endl;

         source << "ClassImp( D3PDReader::" << classname << " )" << std::endl << std::endl;

         source << "namespace D3PDReader {" << std::endl << std::endl;

         //
         // Produce the (input) constructor:
         //
         source << "   " << classname << "::" << classname << "()" << std::endl
                << "      : D3PDObjectBase()," << std::endl;
         FOR_ALL_EVENT_VARIABLES( source << "        " << varname << "( fEntry, \""
                                  << meta_itr->prefix() << "\" )," << std::endl );
         source << "        fEntry( 0 )," << std::endl;
         source << "        fFromInput( kTRUE ) {" << std::endl << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the (output) constructor:
         //
         source << "   " << classname << "::" << classname << "( Bool_t )" << std::endl
                << "      : D3PDObjectBase()," << std::endl;
         FOR_ALL_EVENT_VARIABLES( source << "        " << varname << "( \""
                                  << meta_itr->prefix() << "\" )," << std::endl );
         source << "        fEntry( 0 )," << std::endl;
         source << "        fFromInput( kFALSE ) {" << std::endl << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the prefix handling functions:
         //
         source << "   const char* " << classname << "::GetPrefix() const {" << std::endl
                << std::endl;
         source << "      Warning( \"GetPrefix\", \"A prefix is not defined for this object\" );"
                << std::endl;
         source << "      return \"\";" << std::endl;
         source << "   }" << std::endl << std::endl;

         source << "   void " << classname << "::SetPrefix( const char* ) {" << std::endl
                << std::endl;
         source << "      Warning( \"SetPrefix\", \"A prefix is not defined for this object\" );"
                << std::endl;
         source << "      return;" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the ReadFrom(...) function:
         //
         source << "   void " << classname << "::ReadFrom( ::TTree* tree ) {" << std::endl
                << std::endl;
         source << "      if( ! fFromInput ) {" << std::endl;
         source << "         Error( \"ReadFrom\", \"Object can't read a D3PD. Use a different "
                << "constructor!\" );" << std::endl;
         source << "         return;" << std::endl;
         source << "      }" << std::endl << std::endl;
         FOR_ALL_EVENT_VARIABLES( source << "      " << varname << ".ReadFrom( tree );"
                                  << std::endl );
         source << std::endl << "      return;" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the WriteTo(...) function:
         //
         source << "   void " << classname << "::WriteTo( ::TTree* tree ) {" << std::endl
                << std::endl;
         FOR_ALL_EVENT_VARIABLES( source << "      " << varname << ".WriteTo( tree );"
                                  << std::endl );
         source << std::endl << "      return;" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the SetActive(...) function:
         //
         source << "   void " << classname << "::SetActive( ::Bool_t active, "
                << "const ::TString& pattern ) {" << std::endl << std::endl;
         FOR_ALL_EVENT_VARIABLES( source << "      " << varname << ".SetActive( "
                                  << "active, pattern );" << std::endl );
         source << std::endl << "      return;" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the ReadAllActive() function:
         //
         source << "   void " << classname << "::ReadAllActive() {" << std::endl
                << std::endl;
         FOR_ALL_EVENT_VARIABLES( source << "      " << varname << ".ReadAllActive();"
                                  << std::endl );
         source << std::endl << "      return;" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the GetStatistics() function:
         //
         source << "   D3PDReadStats " << classname << "::GetStatistics() const {"
                << std::endl << std::endl;
         source << "      // The result object:" << std::endl;
         source << "      D3PDReadStats result( D3PDPerfStats::Instance()->GetStats() );"
                << std::endl << std::endl;
         source << "      // Collect the statistics from all constituents:" << std::endl;
         FOR_ALL_EVENT_VARIABLES( source << "      result += " << varname
                                  << ".GetStatistics();" << std::endl );
         source << std::endl << "      return result;" << std::endl;
         source << "   }" << std::endl << std::endl;

         //
         // Produce the GetEntry(...) function:
         //
         source << "   void " << classname << "::GetEntry( ::Long64_t entry ) {" << std::endl
                << std::endl;
         source << "      fEntry = entry;" << std::endl;
         source << "      return;" << std::endl;
         source << "   }" << std::endl << std::endl;

         source << "} // namespace D3PDReader" << std::endl;
         source.close();

         return StatusCode::SUCCESS;
      }

   } // namespace Version2

} // namespace D3PD
