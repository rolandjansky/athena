//
// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "xAODBTagging/BTaggingUtilities.h"

namespace xAOD::BTaggingUtilities {

   /// Convenience type for the accessor
   typedef SG::AuxElement::Accessor< ElementLink< BTaggingContainer > >
      Accessor_t;
   /// Accessor for the default @c xAOD::BTagging link
   static const Accessor_t DEFAULT_ACC( DEFAULT_BTAG_LINK_NAME );

   /// Convenience type for the decorator
   typedef SG::AuxElement::Decorator< ElementLink< BTaggingContainer > >
      Decorator_t;
   /// Decorator for the default @c xAOD::BTagging link
   static const Decorator_t DEFAULT_DEC( DEFAULT_BTAG_LINK_NAME );

   namespace {

      /// Helper function for accessing an @c xAOD::BTagging object
      const xAOD::BTagging* getBTagging( const SG::AuxElement& part,
                                         const Accessor_t& acc ) {

         if( acc.isAvailable( part ) && acc( part ).isValid() ) {
            return *( acc( part ) );
         } else {
            return nullptr;
         }
      }

   } // private namespace

   const BTagging* getBTagging( const SG::AuxElement& part ) {

     return getBTagging( part, DEFAULT_ACC );
   }

   const BTagging* getBTagging( const SG::AuxElement& part,
                                const std::string& linkName ) {

      const Accessor_t btagAcc( linkName );
      return getBTagging( part, btagAcc );
   }

   const ElementLink< xAOD::BTaggingContainer >&
   getBTaggingLink( const SG::AuxElement& part ) {

     // cppcheck-suppress returnTempReference; false positive
     return DEFAULT_ACC( part );
   }

   const ElementLink< xAOD::BTaggingContainer >&
   getBTaggingLink( const SG::AuxElement& part,
                    const std::string& linkName ) {

      const Accessor_t btagAcc( linkName );
      return btagAcc( part );
   }

   void setBTaggingLink( const SG::AuxElement& part,
                         const ElementLink< xAOD::BTaggingContainer >& el ) {

      DEFAULT_DEC( part ) = el;
   }

   void setBTaggingLink( const SG::AuxElement& part,
                         const ElementLink< xAOD::BTaggingContainer >& el,
                         const std::string& linkName ) {

      const Decorator_t btagDec( linkName );
      btagDec( part ) = el;
   }

} // namespace xAOD::BTaggingUtilities
