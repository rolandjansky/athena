/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: isPrimitive.cxx 462802 2011-10-12 16:06:24Z krasznaa $

// STL include(s):
#include <vector>
#include <algorithm>

// Local include(s):
#include "isPrimitive.h"

namespace D3PD {

   /**
    * This function is used in the code generator to determine from a type name if
    * it's a primitive type or not. Some parts of the generated code have to be
    * written differently for primitive and complex types.
    *
    * @param type The type name that we want to evaluate
    * @returns <code>true</code> if the type name is a known primitive type,
    *          <code>false</code> otherwise
    */
   bool isPrimitive( const std::string& type ) {

      //
      // Create an internal list of primitive type names. This
      // only has to be done once.
      //
      static std::vector< std::string > primitives;
      if( ! primitives.size() ) {
         // Normal C++ type names:
         primitives.push_back( "bool" );
         primitives.push_back( "char" );
         primitives.push_back( "unsigned char" );
         primitives.push_back( "short" );
         primitives.push_back( "unsigned short" );
         primitives.push_back( "int" );
         primitives.push_back( "unsigned int" );
         primitives.push_back( "long" );
         primitives.push_back( "unsigned long" );
         primitives.push_back( "long long" );
         primitives.push_back( "unsigned long long" );
         primitives.push_back( "float" );
         primitives.push_back( "double" );
         // ROOT type names:
         primitives.push_back( "Bool_t" );
         primitives.push_back( "Char_t" );
         primitives.push_back( "UChar_t" );
         primitives.push_back( "Short_t" );
         primitives.push_back( "UShort_t" );
         primitives.push_back( "Int_t" );
         primitives.push_back( "UInt_t" );
         primitives.push_back( "Long_t" );
         primitives.push_back( "ULong_t" );
         primitives.push_back( "Long64_t" );
         primitives.push_back( "ULong64_t" );
         primitives.push_back( "Float_t" );
         primitives.push_back( "Double_t" );
      }

      return ( std::find( primitives.begin(), primitives.end(), type ) ==
               primitives.end() ? false : true );
   }

} // namespace D3PD
