/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <iostream>

// EDM include(s):
#include "AthContainers/AuxTypeRegistry.h"

// Local include(s):
#include "xAODCore/AuxCompression.h"

namespace xAOD {

  AuxCompression::AuxCompression()
    : m_compression_map{}
  {
  }

  /// Set which variables should be compressed
  /// The formalism is the following:
  ///   - A set containing "*" will compress all float attributes passed to the object.
  ///   - An empty set, or a single "-" attribute will not compress
  ///     any of the float attributes.
  ///   - A set of variables (without "-" as the first character of the
  ///     variable names) will compress just the variables listed.
  ///   - A set of variable names, each prefixed by "-", will compress all
  ///     variables but the ones listed.
  ///
  /// @param attributes The attributes from CompressionList.
  ///                   By convention the key is the nmantissa and the value is the
  ///                   set of variables that should be compressed at that level.
  ///
  void AuxCompression::setCompressedAuxIDs( const std::map< unsigned int, std::set< std::string > >& attributes ) {

     m_compression_map = attributes;
     return;
  }

  /// This function takes care of interpreting the options passed to
  /// CompressionList in Athena. It filters the list of variables
  /// passed to it, and returns the IDs of just the variables that should
  /// be compressed.
  ///
  /// @param fullset The variables to be compressed based on the rules received
  /// @returns The list of variables to be compressed per compression level
  ///
  std::map< unsigned int, SG::auxid_set_t >
  AuxCompression::getCompressedAuxIDs( const SG::auxid_set_t& fullset ) const {

    // Create an empty result map
    std::map< unsigned int, SG::auxid_set_t > result;

    // Loop over the internal map set by setCompressedAuxIDs
    for(const auto& val : m_compression_map) {

      // Set helper variables
      const unsigned int nmantissa = val.first;
      const std::set< std::string > names = val.second;

      // Start from an empty list
      SG::auxid_set_t auxids;

      // Check the simplest case, nothing to be compressed
      if( names.empty() || ( names.find("-") != names.end() ) ) {
        continue;
      }

      // Check that the user only put positive or negative selections on the
      // list. They can't be mixed.
      bool sub = false, add = false;
      std::set< std::string >::const_iterator name_itr = names.begin();
      std::set< std::string >::const_iterator name_end = names.end();
      for( ; name_itr != name_end; ++name_itr ) {
         if( ( *name_itr )[ 0 ] == '-' ) {
            sub = true;
         } else {
            add = true;
         }
      }
      if( sub && add ) {
         // At this level we don't have a handle to the MsgStream,
         // otherwise it'd be preferred!
         std::cerr << "xAOD::AuxCompression ERROR Mixing + and - options for "
                   << "compression attributes" << std::endl;
         continue;  // Ill-defined input, not compressing anything just in case
      }

      // Loop over the full set, find all float and std::vector<float> variables, add to the list
      // This is our starting point as we currently compress only floats.
      // This way we don't mistakenly float compress a random type based on wrong user input.
      SG::auxid_set_t fauxids;

      for ( const SG::auxid_t& auxid : fullset ) {
        const std::string cType = SG::AuxTypeRegistry::instance().getTypeName( auxid );
        if( cType == "float" || cType == "std::vector<float>" ) {
          fauxids.insert( auxid );
        }
      }

      // Check if all floats are to be compressed
      if( names.find("*") != names.end() ) {
        auxids = fauxids;
      }
      // Here comes the parsing either + or - as in AuxSelection that we follow closely
      else if( add ) {
        // Build the list of variables to be compressed starting from the empty list
        name_itr = names.begin();
        name_end = names.end();
        for( ; name_itr != name_end; ++name_itr ) {
           // Get the ID of this name
           const SG::auxid_t auxid = SG::AuxTypeRegistry::instance().findAuxID( *name_itr );
           if( auxid != SG::null_auxid ) {
              // Add this variable if it exists
              if( fauxids.test( auxid ) ) {
                 auxids.insert( auxid );
              }
           }
        }
      } else {
        // Build the list of variables to be compressed starting from the full float list
        auxids = fauxids;
        // Loop over all float variables and remove if matching
        for ( SG::auxid_t auxid : fauxids ) {
           // Construct the name of this ID
           const std::string attrname = "-" + SG::AuxTypeRegistry::instance().getName( auxid );
           // Check if it is in the list to be removed
           if( names.find( attrname ) != names.end() ) {
              auxids.erase( auxid );
           }
        }
      }

      // Finally fill the result map
      result[ nmantissa ] = auxids;

    } // End of loop over internal map

    return result; // Return the result map
  }

} // namespace xAOD
