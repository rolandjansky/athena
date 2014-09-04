// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: copyAux.h 615003 2014-09-04 08:42:14Z krasznaa $
#ifndef XAODEGAMMAATHENAPOOL_COPYAUX_H
#define XAODEGAMMAATHENAPOOL_COPYAUX_H

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

namespace {

   /// This function is used to copy the contents of one auxiliary store
   /// into another one. To be put into some central location later on.
   ///
   /// @param orig Pointer to the original auxiliary store object
   /// @param copy Pointer to the auxiliary store to fill
   /// @param csize Size of the container connected to this auxiliary store
   ///
   void copyAux( const SG::IConstAuxStore* orig,
                 SG::IAuxStore* copy,
                 const std::size_t csize ) {

      // If the size of the container is 0, let's not do anything:
      if( ! csize ) {
         return;
      }

      // Access the auxiliary type registry:
      SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

      // The auxiliary IDs that the original container has:
      SG::auxid_set_t auxids = orig->getAuxIDs();

      // Make sure that the container is of the right size:
      copy->resize( csize );

      // Loop over all the variables of the original container:
      for( SG::auxid_t auxid : auxids ) {
         // Create the target variable:
         void* dst = copy->getData( auxid, csize, csize );
         // Access the source variable:
         const void* src = orig->getData( auxid );
         // A sanity check:
         if( ( ! dst ) || ( ! src ) ) {
            REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "copyAux" )
               << "Couldn't copy auxiliary ID " << auxid;
            continue;
         }
         // Copy over all elements:
         for( std::size_t i = 0; i < csize; ++i ) {
            r.copy( auxid, dst, i, src, i );
         }
      }
      return;
   }

} // private namespace

#endif // XAODEGAMMAATHENAPOOL_COPYAUX_H
