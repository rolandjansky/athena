/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


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
      static const std::vector< std::string > primitives = {
         // Normal C++ type names:
         "bool",
         "char",
         "unsigned char",
         "short",
         "unsigned short",
         "int",
         "unsigned int",
         "long",
         "unsigned long",
         "long long",
         "unsigned long long",
         "float",
         "double",
         // ROOT type names:
         "Bool_t",
         "Char_t",
         "UChar_t",
         "Short_t",
         "UShort_t",
         "Int_t",
         "UInt_t",
         "Long_t",
         "ULong_t",
         "Long64_t",
         "ULong64_t",
         "Float_t",
         "Double_t",
      };

      return ( std::find( primitives.begin(), primitives.end(), type ) ==
               primitives.end() ? false : true );
   }

} // namespace D3PD
