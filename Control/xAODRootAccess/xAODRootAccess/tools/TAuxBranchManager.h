// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TAuxBranchManager.h 595278 2014-05-03 09:31:05Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TAUXBRANCHMANAGER_H
#define XAODROOTACCESS_TOOLS_TAUXBRANCHMANAGER_H

// EDM include(s):
#include "AthContainersInterfaces/AuxTypes.h"

// Local include(s):
#include "TVirtualManager.h"

// Forward declaration(s):
namespace SG {
   class IAuxTypeVector;
}

namespace xAOD {

   // Forward declaration(s):
   class THolder;

   /// @short Manager for auxiliary branches created dynamically
   ///
   /// This manager class is meant to deal with "simple"
   /// auxiliary branches in the xAOD files.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 595278 $
   /// $Date: 2014-05-03 11:31:05 +0200 (Sat, 03 May 2014) $
   ///
   class TAuxBranchManager : public TVirtualManager {

   public:
      /// Definition of the auxiliary ID type
      typedef SG::auxid_t auxid_t;

      /// Constructor getting hold of a possible branch
      TAuxBranchManager( auxid_t auxid,
                         ::TBranch* br = 0, THolder* holder = 0 );
      /// Copy constructor
      TAuxBranchManager( const TAuxBranchManager& parent );
      /// Destructor
      ~TAuxBranchManager();

      /// Assignment operator
      TAuxBranchManager& operator=( const TAuxBranchManager& rhs );

      /// Accessor to the branch
      ::TBranch* branch() const;
      /// Pointer to the branch's pointer
      ::TBranch** branchPtr();
      /// Accessor to the Holder object (constant version)
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
      mutable ::Bool_t m_isSet;

      /// Auxiliary variable type
      auxid_t m_auxId;
      /// Dummy auxiliary variable for the empty events
      mutable SG::IAuxTypeVector* m_vector;

   }; // class TAuxBranchManager

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TAUXBRANCHMANAGER_H
