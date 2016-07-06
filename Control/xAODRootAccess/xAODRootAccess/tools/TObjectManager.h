// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TObjectManager.h 725531 2016-02-22 16:14:25Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TOBJECTMANAGER_H
#define XAODROOTACCESS_TOOLS_TOBJECTMANAGER_H

// Local include(s):
#include "TVirtualManager.h"

// Forward declaration(s):
class TBranch;

namespace xAOD {

   // Forward declaration(s):
   class THolder;

   ///
   /// @short Manager for EDM objects created by ROOT
   ///
   /// This class is used when an EDM object is meant to be created
   /// by ROOT's schema evolution system, behind the scenes.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 725531 $
   /// $Date: 2016-02-22 17:14:25 +0100 (Mon, 22 Feb 2016) $
   ///
   class TObjectManager : public TVirtualManager {

   public:
      /// Constructor, getting hold of the created objects
      TObjectManager( ::TBranch* br = 0, THolder* holder = 0,
                      ::Bool_t renewOnRead = kFALSE );
      /// Copy constructor
      TObjectManager( const TObjectManager& parent );
      /// Destructor
      ~TObjectManager();

      /// Assignment operator
      TObjectManager& operator=( const TObjectManager& parent );

      /// Accessor to the branch
      ::TBranch* branch() const;
      /// Pointer to the branch's pointer
      ::TBranch** branchPtr();
      /// Accessor to the Holder object
      const THolder* holder() const;
      /// Accessor to the Holder object
      THolder* holder();

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

   private:
      /// Pointer keeping track of the branch
      ::TBranch* m_branch;
      /// Holder object for the EDM object
      THolder* m_holder;
      /// The last entry that was loaded for this branch
      ::Long64_t m_entry;
      /// Was the object set for the current event?
      ::Bool_t m_isSet;
      /// Should the object be recreated on each read?
      ::Bool_t m_renewOnRead;

   }; // class TObjectManager

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TOBJECTMANAGER_H
