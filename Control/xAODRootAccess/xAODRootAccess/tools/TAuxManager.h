// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TAuxManager.h 607344 2014-07-18 13:27:49Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TAUXMANAGER_H
#define XAODROOTACCESS_TOOLS_TAUXMANAGER_H

// System include(s):
#include <memory>

// Local include(s):
#include "TVirtualManager.h"

// Forward declaration(s):
namespace SG {
   class IConstAuxStore;
}

namespace xAOD {

   // Forward declaration(s):
   class TAuxStore;

   /// @short Manager for TAuxStore objects
   ///
   /// This class is used when connecting TAuxStore objects to the
   /// input tree as the auxiliary store of a DV container.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 607344 $
   /// $Date: 2014-07-18 15:27:49 +0200 (Fri, 18 Jul 2014) $
   ///
   class TAuxManager : public TVirtualManager {

   public:
      /// Constructor getting hold of an auxiliary store object
      TAuxManager( TAuxStore* store, ::Bool_t sharedOwner = kTRUE );
      /// Copy constructor
      TAuxManager( const TAuxManager& parent );

      /// Assignment operator
      TAuxManager& operator= ( const TAuxManager& rhs );

      /// Function for updating the object in memory if needed
      virtual ::Int_t getEntry( ::Long64_t entry, ::Int_t getall = 0 );

      /// Function getting a pointer to the object being handled
      virtual void* object() const;
      /// Function replacing the object being handled
      virtual void setObject( void* obj );

      /// Check if the object was set for the current event
      virtual ::Bool_t isSet( ::Bool_t forceSet = kTRUE ) const;
      /// Reset the object at the end of processing of an event
      virtual void reset();

      /// Get a type-specific pointer to the managed object
      TAuxStore* getStore() const;
      /// Get a convenience pointer to the managed object
      const SG::IConstAuxStore* getConstStore() const;

   private:
      /// The auxiliary store object
      std::shared_ptr< TAuxStore > m_store;
      /// Pointer to the auxiliary store object
      TAuxStore* m_storePtr;

   }; // class TAuxManager

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TAUXMANAGER_H
