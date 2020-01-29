/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <iostream>
#include <map>

// EDM include(s):
#include "AthContainers/AuxTypeRegistry.h"

// Local include(s):
#include "xAODCore/AuxCompression.h"

namespace xAOD {

  AuxCompression::AuxCompression()
    : m_names{},
      m_nbits{}
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
  /// @param attributes The attributes from CompressionList. By convention the first element holds the
  /// high compression and the second element holds the low compression lists
  ///
  void AuxCompression::setCompressedAuxIDs( const std::vector< std::set< std::string > >& attributes ) {

     m_names = attributes;
     return;
  }

  /// This function takes care of interpreting the options passed to
  /// CompressionList in Athena. It filters the list of variables
  /// passed to it, and returns the IDs of just the variables that should
  /// be compressed.
  ///
  /// @param fullset The variables to be compressed based on the rules received
  /// @param highComp Whether to retrieve the high or the low compression list
  /// @returns The list of variables to be compressed
  ///
  SG::auxid_set_t
  AuxCompression::getCompressedAuxIDs( const SG::auxid_set_t& fullset, const bool& highComp ) const {

    // Find the relevant index
    const bool idx = highComp ? AuxCompression::High : AuxCompression::Low;

    // Start from an empty list
    SG::auxid_set_t auxids;

    // Check the simplest case, nothing to be compressed
    if( m_names.size() < AuxCompression::NTotal || m_names[ idx ].empty() || ( m_names[ idx ].find("-") != m_names[ idx ].end() ) ) {
      return auxids;
    }

    // Check that the user only put positive or negative selections on the
    // list. They can't be mixed.
    bool sub = false, add = false;
    std::set< std::string >::const_iterator name_itr = m_names[ idx ].begin();
    std::set< std::string >::const_iterator name_end = m_names[ idx ].end();
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
       return auxids;  // Ill-defined input, not compressing anything just in case
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

    // Check if all floats are to be compressed, if so return the full float list at this point
    if( m_names[ idx ].find("*") != m_names[ idx ].end() ) {
      return fauxids;
    }

    // Here comes the parsing either + or - as in AuxSelection that we follow closely
    if( add ) {
      // Build the list of variables to be compressed starting from the empty list
      name_itr = m_names[ idx ].begin();
      name_end = m_names[ idx ].end();
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
      for (SG::auxid_t auxid : fauxids) {
         // Construct the name of this ID
         const std::string attrname = "-" + SG::AuxTypeRegistry::instance().getName( auxid );
         // Check if it is in the list to be removed
         if( m_names[ idx ].find( attrname ) != m_names[ idx ].end() ) {
            auxids.erase( auxid );
         }
      }
    }

    // Return the list of variables to be compressed
    return auxids;
  }

  /// Set the number of bits to be used in the float compression
  /// By definition first element stores the high compression
  /// while the second element stores the low compression configuration
  ///
  /// params nbits The vector holding the mantissa bits
  ///
  void AuxCompression::setCompressionBits( const std::vector< unsigned int >& nbits ) {

    m_nbits = nbits;
    return;
  }

  /// Get the number of bits to be used in the float compression
  /// See above for the convention
  ///
  /// params highComp Either high or low compression
  ///
  unsigned int
  AuxCompression::getCompressionBits( const bool& highComp ) const {

    // Find the relevant index
    const bool idx = highComp ? AuxCompression::High : AuxCompression::Low;

    // Return the number of mantissa bits
    return m_nbits[ idx ];
  }

} // namespace xAOD
