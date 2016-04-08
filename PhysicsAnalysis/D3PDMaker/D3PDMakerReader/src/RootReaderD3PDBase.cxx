/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RootReaderD3PDBase.cxx 595600 2014-05-06 12:18:26Z ssnyder $

// ROOT include(s):
#include <Rtypes.h>

// Boost include(s):
#include <boost/algorithm/string.hpp>

// Gaudi/Athena include(s):
#include "GaudiKernel/System.h"
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "RootReaderD3PDBase.h"

/// Private namespace for the local functions
#if 0
namespace {

   /// Function checking if a type is a primitive or not
   bool isPrimitive( const std::type_info& ti ) {

      if( ( ti == typeid( char* ) ) ||
          ( ti == typeid( Char_t ) ) ||
          ( ti == typeid( UChar_t ) ) ||
          ( ti == typeid( Short_t ) ) ||
          ( ti == typeid( UShort_t ) ) ||
          ( ti == typeid( Int_t ) ) ||
          ( ti == typeid( UInt_t ) ) ||
          ( ti == typeid( Float_t ) ) ||
          ( ti == typeid( Double_t ) ) ||
          ( ti == typeid( Long64_t ) ) ||
          ( ti == typeid( ULong64_t ) ) ||
          ( ti == typeid( Bool_t ) ) ) {

         return true;
      }

      return false;
   }

   /// Function returning the full ROOT typename of a primitive type
   const char* rootPrimitiveType( const std::type_info& ti ) {

      if( ti == typeid( char* ) ) {
         return "char*";
      } else if( ti == typeid( Char_t ) ) {
         return "Char_t";
      } else if( ti == typeid( UChar_t ) ) {
         return "UChar_t";
      } else if( ti == typeid( Short_t ) ) {
         return "Short_t";
      } else if( ti == typeid( UShort_t ) ) {
         return "UShort_t";
      } else if( ti == typeid( Int_t ) ) {
         return "Int_t";
      } else if( ti == typeid( UInt_t ) ) {
         return "UInt_t";
      } else if( ti == typeid( Float_t ) ) {
         return "Float_t";
      } else if( ti == typeid( Double_t ) ) {
         return "Double_t";
      } else if( ti == typeid( Long64_t ) ) {
         return "Long64_t";
      } else if( ti == typeid( ULong64_t ) ) {
         return "ULong64_t";
      } else if( ti == typeid( Bool_t ) ) {
         return "Bool_t";
      }

      return "\0";
   }

} // namespace
#endif

namespace D3PD {

   RootReaderD3PDBase::RootReaderD3PDBase()
      : m_metadata() {

   }

   StatusCode RootReaderD3PDBase::addVariable( const std::string& name,
                                               const std::type_info& ti,
                                               void*& ptr,
                                               const std::string& docstring,
                                               const void* defval ) {

      CHECK( m_metadata.addVariable( name, ti, ptr, docstring, defval ) );
      return StatusCode::SUCCESS;
   }

   StatusCode RootReaderD3PDBase::addDimensionedVariable( const std::string& name,
                                                          const std::type_info& ti,
                                                          void*& ptr,
                                                          const std::string& dim,
                                                          const std::string& docstring,
                                                          const void* defval ) {

      CHECK( m_metadata.addDimensionedVariable( name, ti, ptr, dim, docstring,
                                                defval ) );
      return StatusCode::SUCCESS;
   }

   StatusCode RootReaderD3PDBase::addMetadata( const std::string&,
                                               const void*,
                                               const std::type_info& ) {

      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "RootReaderD3PDBase" )
         << "addMetadata(...) not implemented";
      return StatusCode::FAILURE;
   }

   StatusCode RootReaderD3PDBase::capture() {

      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "RootReaderD3PDBase" )
         << "capture() not implemented";
      return StatusCode::FAILURE;
   }

   StatusCode RootReaderD3PDBase::clear() {

      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "RootReaderD3PDBase" )
         << "clear() not implemented";
      return StatusCode::FAILURE;
   }

   StatusCode RootReaderD3PDBase::redim( const Dim_t* ) {

      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "RootReaderD3PDBase" )
         << "redim(...) not implemented";
      return StatusCode::FAILURE;
   }

   void RootReaderD3PDBase::setIsContainer( bool isContainer ) {

      m_metadata.setContainer( isContainer );
      return;
   }

   bool RootReaderD3PDBase::isContainer() const {

      return m_metadata.container();
   }

   void RootReaderD3PDBase::setPrefix( const std::string& prefix ) {

      m_metadata.setPrefix( prefix );
      return;
   }

   const std::string& RootReaderD3PDBase::prefix() const {

      return m_metadata.prefix();
   }

   /**
    * This function is used internally to decide if specific STL headers are needed
    * for the generated code or not. The code just checks if the header name appears
    * in the variable types somewhere, and if it does, it adds the requested header.
    *
    * @param out Output stream where the include statement has to be written
    * @param name Name of the STL header that the code should process
    */
   void RootReaderD3PDBase::addSTLHeader( std::ostream& out, const char* name ) const {

      // Check if the STL name appears in the variable type names:
      bool header_needed = false;
      std::set< D3PD::ObjectMetadata::Variable >::const_iterator itr =
         m_metadata.variables().begin();
      std::set< D3PD::ObjectMetadata::Variable >::const_iterator end =
         m_metadata.variables().end();
      for( ; itr != end; ++itr ) {
         if( itr->type().find( name ) != std::string::npos ) {
            header_needed = true;
            break;
         }
      }

      // If it does, let's include it:
      if( header_needed ) {
         out << "#include <" << name << ">" << std::endl;
      }

      return;
   }

} // namespace D3PD
