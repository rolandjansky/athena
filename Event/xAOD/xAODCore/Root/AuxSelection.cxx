/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxSelection.cxx 653257 2015-03-11 11:26:15Z krasznaa $

// System include(s):
#include <iostream>
#include <map>

// EDM include(s):
#include "AthContainers/AuxTypeRegistry.h"

// Local include(s):
#include "xAODCore/AuxSelection.h"
#include "CxxUtils/checker_macros.h"
#include <mutex>

namespace {

   /// Helper variable to only print warning about missing variables once,
   /// with a mutex.
   std::set< std::string > mentionedVariableNames ATLAS_THREAD_SAFE;
   std::mutex mentionedMutex;

} // private namespace

namespace xAOD {

   AuxSelection::AuxSelection()
      : m_names()
   {
   }

   /// Sets which variables should be selected from a store object.
   /// The formalism is the following:
   ///   - An empty set, or a set containing "*" will select all the dynamic
   ///     attributes passed to the object.
   ///   - A single "-" attribute will not select any of the dynamic attributes.
   ///   - A set of variables (without "-" as the first character of the
   ///     variable names) will select just the variables listed.
   ///   - A set of variable names, each prefixed by "-", will select all
   ///     variables but the ones listed.
   ///
   /// @param attributes The attributes from ItemList or TEvent
   ///
   void AuxSelection::selectAux( const std::set< std::string >& attributes ) {

      m_names = attributes;
      return;
   }

   /// This function takes care of interpreting the options passed to
   /// ItemList in Athena, and TEvent in ROOT. It filters the list of variables
   /// passed to it, and returns the IDs of just the variables that should
   /// be written out.
   ///
   /// @param fullset The variables to be filtered based on the rules received
   /// @returns The list of variables to be written out
   ///
   SG::auxid_set_t
   AuxSelection::getSelectedAuxIDs( const SG::auxid_set_t& fullset ) const {

      // Check for the simplest case... all variables selected:
      if( m_names.empty() || ( m_names.find( "*" ) != m_names.end() ) ) {
         return fullset;
      }

      // Start from an empty list:
      SG::auxid_set_t auxids;

      // Check if everything should be disregarded:
      if( m_names.find( "-" ) != m_names.end() ) {
         return auxids;
      }

      // Check that the user only put positive or negative selections on the
      // list. They can't be mixed.
      bool sub = false, add = false;
      std::set< std::string >::const_iterator name_itr = m_names.begin();
      std::set< std::string >::const_iterator name_end = m_names.end();
      for( ; name_itr != name_end; ++name_itr ) {
         if( ( *name_itr )[ 0 ] == '-' ) {
            sub = true;
         } else {
            add = true;
         }
      }
      if( sub && add ) {
         std::cerr << "xAOD::AuxSelection ERROR Mixing + and - options for "
                   << "dynamic attributes" << std::endl;
         return fullset;  //better safe than sorry
      }

      if( add ) {
         // Start with an empty list, and loop over the selected names:
         name_itr = m_names.begin();
         name_end = m_names.end();
         for( ; name_itr != name_end; ++name_itr ) {
            // Get the ID of this name:
            const SG::auxid_t auxid =
               SG::AuxTypeRegistry::instance().findAuxID( *name_itr );
            if( auxid != SG::null_auxid ) {
               // Add this variable if it exists:
               if( fullset.test( auxid ) ) {
                  auxids.insert( auxid );
               }
            } else {
               // Check if a warning should be printed at this time or not:
               std::lock_guard<std::mutex> lock (mentionedMutex);
               if( ::mentionedVariableNames.insert( *name_itr ).second ) {
                  // Apparently we didn't complain about this name yet...
                  std::cerr << "xAOD::AuxSelection WARNING Selected dynamic "
                            << "Aux atribute \"" << *name_itr
                            << "\" not found in the registry" << std::endl;
               }
            }
         }
      } else {
         // Start from the full list:
         auxids = fullset;
         // ...and check which variables should be removed:
         for (SG::auxid_t id : fullset) {
            // Construct the name that we need to look for:
            const std::string attrname =
               "-" + SG::AuxTypeRegistry::instance().getName( id );
            // Check if it is in the list to be removed:
            if( m_names.find( attrname ) != m_names.end() ) {
               auxids.erase( id );
            }
         }
      }

      // Return the list of variables to be written out:
      return auxids;
   }

} // namespace xAOD
