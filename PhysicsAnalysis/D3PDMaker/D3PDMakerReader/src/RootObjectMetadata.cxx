/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RootObjectMetadata.cxx 472095 2011-12-02 10:27:01Z krasznaa $

// STL include(s):
#include <vector>
#include <algorithm>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "RootObjectMetadata.h"
#include "isPrimitive.h"

namespace D3PD {

   RootObjectMetadata::RootObjectMetadata()
      : ObjectMetadata() {

   }

   RootObjectMetadata::RootObjectMetadata( const RootObjectMetadata& parent )
      : ObjectMetadata( parent ) {

   }

   StatusCode RootObjectMetadata::addVariable( const std::string& name,
                                               const std::string& type,
                                               const std::string& docstring ) {

      // Check that the variable has the correct prefix:
      if( ( m_prefix != "" ) &&
          ( name.find( m_prefix ) != 0 ) ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "RootObjectMetadata" )
            << "Specified variable name (" << name << ") doesn't have the "
            << "expected prefix (" << m_prefix << ")";
         return StatusCode::RECOVERABLE;
      }

      // Create a new variable:
      Variable var;

      // Remove the prefix from the variable name:
      var.setName( name.substr( m_prefix.size(), name.npos ) );

      // Check if it's already amongst the existing variables:
      if( m_variables.find( var ) != m_variables.end() ) {
         return StatusCode::SUCCESS;
      }

      // Set the type of the variable:
      var.setType( type );

      // Set whether the variable is a primitive:
      var.setPrimitive( isPrimitive( type ) );

      // Set the documentation string for the variable:
      var.setDoc( docstring );

      // Remember the variable:
      m_variables.insert( var );

      return StatusCode::SUCCESS;
   }

   /**
    * By mistake some of the variable names in the D3PDs produced by
    * AtlasPhysics-17.0.5.5.2 did not get their prefix removed when they were
    * saved into the metadata. (Still need to investigate why.)
    *
    * This function remedies this by fixing the variable names manually.
    *
    * @returns <code>StatusCode::SUCCESS</code> if everything went fine
    */
   StatusCode RootObjectMetadata::checkPrefixes() {

      // For simplicity's sake I don't modify the original std::set,
      // but create a new one instead. This is slightly slower, but
      // much easier. In any case, performance is not a major concern
      // in this code...
      std::set< Variable > newVariables;

      // Loop over the variables:
      std::set< Variable >::const_iterator itr = m_variables.begin();
      std::set< Variable >::const_iterator end = m_variables.end();
      for( ; itr != end; ++itr ) {

         // Check if the variable name begins with the prefix (it shouldn't):
         if( ( m_prefix != "" ) &&
             ( itr->name().find( m_prefix ) == 0 ) ) {
            // Create a copy of the variable, and remove the prefix from its name:
            Variable var = *itr;
            var.setName( var.name().substr( m_prefix.size(), var.name().npos ) );
            // Insert it into the new container:
            newVariables.insert( var );
         } else {
            // Insert the variable as it is into the new container:
            newVariables.insert( *itr );
         }
      }

      // Swap the two containers. (This should be a fairly quick operation...)
      m_variables.swap( newVariables );

      return StatusCode::SUCCESS;
   }

   /**
    * Metadata objects are saved with a name like "Something_XXXXXX". This function
    * makes a very simple check whether the name given to it follows this format.
    *
    * @param name The name of a metadata object
    * @returns <code>true</code> if it appears to be an object metadata object
    */
   bool RootObjectMetadata::isObjectMetadata( const std::string& name ) {

      if( ( name.size() > ( RANDOM_NAME_POSTFIX_LENGTH + 1 ) ) &&
          ( name[ name.size() - ( RANDOM_NAME_POSTFIX_LENGTH + 1 ) ] == '_' ) ) {
         return true;
      } else {
         return false;
      }
   }

} // namespace D3PD
