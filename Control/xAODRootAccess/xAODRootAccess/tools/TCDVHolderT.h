// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TCDVHolderT.h 656386 2015-03-24 13:42:48Z krasznaa $
#ifndef XAODROOTACCESS_TOOL_TCDVHOLDERT_H
#define XAODROOTACCESS_TOOL_TCDVHOLDERT_H

// EDM include(s):
#include "AthContainers/ConstDataVector.h"

// Local include(s):
#include "xAODRootAccess/tools/THolder.h"

namespace xAOD {

   /// Class managing ConstDataVector objects in transient memory
   ///
   /// In order to be able to record ConstDataVector objects into the transient
   /// store, and be able to retrieve them as "const DataVector", we need to use
   /// a specialised holder type. Very much like how we use a specialised
   /// SG::DataBucket for this type in Athena's StoreGate.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 656386 $
   /// $Date: 2015-03-24 14:42:48 +0100 (Tue, 24 Mar 2015) $
   ///
   template< class T >
   class TCDVHolderT : public THolder {

   public:
      /// Type of the managed object
      typedef ConstDataVector< T > Object_t;

      /// Constructor with the object, and its type
      TCDVHolderT( ConstDataVector< T >* object, const std::type_info& type,
                   ::Bool_t owner = kTRUE );

      /// Replace the managed object
      virtual void set( void* obj );

      /// Return the object as a specific, constant pointer
      virtual const void* getAsConst( const std::type_info& tid,
                                      ::Bool_t silent = kFALSE ) const;

   private:
      /// Type specific pointer to the managed object
      ConstDataVector< T >* m_cdvObject;
      /// Helper object managing the inheritance tree of the underlying
      /// DataVector type
      THolder m_holderHelper;

   }; // class TCDVHolderT

} // namespace xAOD

// Include the implementation:
#include "xAODRootAccess/tools/TCDVHolderT.icc"

#endif // XAODROOTACCESS_TOOL_TCDVHOLDERT_H
