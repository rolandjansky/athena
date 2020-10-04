// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Utils.h 601421 2014-06-12 11:44:08Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_UTILS_H
#define XAODROOTACCESS_TOOLS_UTILS_H

// System include(s):
#include <string>
#include <typeinfo>
extern "C" {
#   include <stdint.h>
}

// ROOT include(s):
#include <TDataType.h>
#include <TTree.h>

namespace xAOD {

   namespace Utils {

      /// Function creating a hash out of a "key name"
      uint32_t hash( const std::string& key );

      /// Get the dynamic auxiliary variable prefix based on a container name
      std::string dynBranchPrefix( const std::string& key );

      /// Get the type info of a primitive variable, as declared by ROOT
      const std::type_info& getTypeInfo( EDataType type );

      /// Get the character describing a given primitive type for ROOT
      char rootType( char typeidType );

      /// Get the type name as it is known to ROOT, based on std::type_info
      std::string getTypeName( const std::type_info& ti );

      /// Search for branches, returns search term on no result
      std::string getFirstBranchMatch( TTree* tree, const std::string& pre );

   } // namespace Utils

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_UTILS_H
