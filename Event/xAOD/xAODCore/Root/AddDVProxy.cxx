/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AddDVProxy.cxx 591472 2014-04-05 11:23:29Z krasznaa $

// ROOT include(s):
#include <TClass.h>
#include <TError.h>

// Local include(s):
#include "xAODCore/AddDVProxy.h"

namespace xAOD {

   /// The constructor of TDVCollectionProxy needs the dictionary of
   /// DataVector<xAOD::TDVCollectionProxyDummy> to be loaded already.
   /// This function makes sure that this is the case.
   ///
   void AddDVProxy::loadDictionaries() {

      // Make sure that the minimal set of dictionaries are loaded:
      TClass* dummyCl =
         TClass::GetClass( "DataVector<xAOD::TDVCollectionProxyDummy>" );
      if( ! dummyCl ) {
         ::Error( "xAOD::AddDVProxy::loadDictionaries",
                  "Couldn't load the dictionary for "
                  "DataVector<xAOD::TDVCollectionProxyDummy>" );
      }

      return;
   }

} // namespace edm
