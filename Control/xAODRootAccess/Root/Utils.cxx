/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Utils.cxx 613032 2014-08-22 10:50:40Z krasznaa $

// STL include(s):
#include <functional>
#include <regex>

// ROOT include(s):
#include <TError.h>

// EDM include(s):
#include "AthContainers/tools/error.h"

// Local include(s):
#include "xAODRootAccess/tools/Utils.h"
#include "xAODRootAccess/tools/Message.h"

namespace {

   /// This function is used by xAOD::Utils::getTypeName to trim out the default
   /// template parameters of vector (std::vector, DataVector) types out of a
   /// type name.
   ///
   /// @param name The type name to be trimmed
   /// @param vectorName The vector type name (std::vector or DataVector
   ///                   really...)
   ///
   void removeDefaultTemplateParameters( std::string& name,
                                         const std::string& vectorName ) {

      size_t vecPos = 0;
      while( ( vecPos = name.find( vectorName + "<", vecPos ) ) !=
             std::string::npos ) {

         // Update the position to point at the end of the search string:
         vecPos += ( vectorName.size() + 1 );

         // Look for "the comma" that we should remove everything
         // after:
         int open = 0;
         size_t commaPos = std::string::npos;
         for( size_t pos = vecPos; pos < name.size(); ++pos ) {
            switch( name[ pos ] ) {
            case '<':
               ++open;
               break;
            case '>':
               --open;
               break;
            case ',':
               if( ! open ) {
                  commaPos = pos;
               }
               break;
            default:
               break;
            }
            if( commaPos != std::string::npos ) {
               break;
            }
         }

         // If we didn't find a comma, then let's give up:
         if( commaPos == std::string::npos ) {
            continue;
         }

         // If we did, then let's look for the closing '>' of the vector:
         open = 0;
         size_t closingPos = std::string::npos;
         for( size_t pos = commaPos; pos < name.size(); ++pos ) {
            switch( name[ pos ] ) {
            case '<':
               ++open;
               break;
            case '>':
               if( ! open ) {
                  closingPos = pos;
                  break;
               }
               --open;
               break;
            default:
               break;
            }
            if( closingPos != std::string::npos ) {
               break;
            }
         }

         // It's bad if we didn't find the closing '>'...
         if( closingPos == std::string::npos ) {
            ::Error( "removeDefaultTemplateParameters",
                     XAOD_MESSAGE( "Couldn't find closing '>' in name %s" ),
                     name.c_str() );
            return;
         }

         // Remove the identified part of the string:
         name.erase( commaPos, closingPos - commaPos );
      }

      return;
   }

} // private namespace

namespace xAOD {

   namespace Utils {

      /// This function provides a hashed version of the key (branch) names
      /// used in the xAOD file, similar to how Athena saves hashed numbers
      /// in persistent ElementLinks.
      ///
      /// Since Athena uses the top 2 bits in an ElementLink's 32-bit hashed key
      /// to save additional state information about the link, the top 2 bits
      /// of the hash are always set to 0.
      ///
      /// @param name The name that we want to create a unique hash for
      /// @return A more or less unique hash for the string
      ///
      uint32_t hash( const std::string& name ) {

         // The helper object:
         static std::hash< std::string > helper;
         // Let the helper do the work:
         return ( static_cast< uint32_t >( helper( name ) ) & 0x3fffffff );
      }

      /// This function is used to figure out what to name dynamic auxiliary
      /// branches coming from a container called <code>key</code>. It needs
      /// to work in sync wiht how it is done in the offline code, so will
      /// eventually probably end up in another package.
      ///
      /// @param key The key/branch name of the auxiliary container
      /// @returns The prefix of the dynamic variables created based on the
      ///          contents of the object
      ///
      std::string dynBranchPrefix( const std::string& key ) {

         // Make a copy of the key that we can modify:
         std::string result = key;

         // If it ends in a dot, then let's exchange that dot for
         // "Dyn.":
         if( result[ result.size() - 1 ] == '.' ) {
            result.replace( result.size() - 1, 1, "Dyn." );
         }
         // If it doesn't end in a dot, then let's just add "Dyn." at
         // the end, and be done with it:
         else {
            result += "Dyn.";
         }

         return result;
      }

      /// This function is used when reading a primitive branch from an
      /// input file without the user explicitly asking for it.
      ///
      /// @param type The type taken from ROOT
      /// @returns The typeid of the primitive type in question
      ///
      const std::type_info& getTypeInfo( EDataType type ) {

         switch( type ) {

         case kChar_t:
         case kchar:
            return typeid( Char_t );
         case kUChar_t:
            return typeid( UChar_t );
         case kShort_t:
            return typeid( Short_t );
         case kUShort_t:
            return typeid( UShort_t );
         case kInt_t:
         case kCounter:
            return typeid( Int_t );
         case kUInt_t:
         case kBits:
         case kDataTypeAliasUnsigned_t:
            return typeid( UInt_t );
         case kLong_t:
            return typeid( Long_t );
         case kULong_t:
            return typeid( ULong_t );
         case kFloat_t:
            return typeid( Float_t );
         case kDouble_t:
            return typeid( Double_t );
         case kDouble32_t:
            return typeid( Double32_t );
         case kBool_t:
            return typeid( Bool_t );
         case kLong64_t:
            return typeid( Long64_t );
         case kULong64_t:
            return typeid( ULong64_t );
         case kFloat16_t:
            return typeid( Float16_t );
         case kCharStar:
            return typeid( char* );
         case kVoid_t:
            return typeid( void );
         default:
            ::Error( "xAOD::Utils::getTypeInfo",
                     XAOD_MESSAGE( "Unknown data type (%i) received" ),
                     static_cast< int >( type ) );
            return typeid( void );
         }
      }

      /// This function is used internally in the code when creating primitive
      /// dynamic auxiliary branches. I just took the code from SFrame to be
      /// honest...
      ///
      /// @param typeidType The type name coming from typeid(...).name()
      /// @returns The character describing this type for TTree::Branch(...)
      ///
      char rootType( char typeidType ) {

         // Do the hard-coded translation:
         switch( typeidType ) {

         case 'c':
            return 'B';
            break;
         case 'h':
            return 'b';
            break;
         case 's':
            return 'S';
            break;
         case 't':
            return 's';
            break;
         case 'i':
            return 'I';
            break;
         case 'j':
            return 'i';
            break;
         case 'f':
            return 'F';
            break;
         case 'd':
            return 'D';
            break;
         case 'x':
            return 'L';
            break;
         case 'y':
         case 'm': // Not sure how platform-independent this one is...
            return 'l';
            break;
         case 'b':
            return 'O';
            break;
         }

         // If we didn't find this type:
         Error( "xAOD::Utils::rootType",
                XAOD_MESSAGE( "Received an unknown type: %c" ), typeidType );
         return '\0';
      }

      /// This function is necessary in order to create type names that ROOT
      /// can understand. For instance, a type name like
      /// <code>std::vector<ElementLink<DataVector<xAOD::IParticle, DataModel_detail::NoBase> >, std::allocator<ElementLink<DataVector<xAOD::IParticle, DataModel_detail::NoBase> > > ></code>
      /// as seen by the compiler, needs to become
      /// <code>std::vector<ElementLink<DataVector<xAOD::IParticle> > ></code>
      /// for ROOT to find a dictionary for it.
      ///
      /// This function executes the string manipulation necessary to do this.
      ///
      /// @param ti The type for which we want to find a string type name
      /// @returns The type name in a format that ROOT understands
      ///
      std::string getTypeName( const std::type_info& ti ) {

         // Get the "raw" type name:
         std::string result = AthContainers_detail::typeinfoName( ti );

         // If there are no "tricky" classes in the name, then we're done
         // already:
         if( ( result.find( "DataVector<" ) == std::string::npos ) &&
             ( result.find( "std::vector<" ) == std::string::npos ) &&
             ( result.find( "std::__1::vector<") == std::string::npos ) ) {
            return result;
         }

         // Deal with the vector types:
         removeDefaultTemplateParameters( result, "DataVector" );
         removeDefaultTemplateParameters( result, "std::vector" );
         removeDefaultTemplateParameters( result, "std::__1::vector" );

         // Make sure that no ">>" parts are left in the type name:
         size_t pos = 0;
         while( ( pos = result.find( ">>" ) ) != std::string::npos ) {
            result.replace( pos, 2, "> >" );
         }

         // Remove all "__1" namespaces from the type name:
         while( ( pos = result.find( "__1::" ) ) != std::string::npos ) {
            result.replace( pos, 5, "" );
         }

         // Return the massaged name:
         return result;
      }

      std::string getFirstBranchMatch( TTree * tree,
                                        const std::string& pre ) {
         const TObjArray * pBranches = tree->GetListOfBranches();
         const std::regex pattern( ".*" + pre + ".*" );

         for( int i = 0; i < pBranches->GetLast() ; ++i ) {
            const std::string name = pBranches->At(i)->GetName();

            if( std::regex_match( name, pattern ) )
               return name;

         }

         return pre;
      }

   } // namespace Utils

} // namespace xAOD
