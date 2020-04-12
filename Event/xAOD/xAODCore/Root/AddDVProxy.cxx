/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODCore/AddDVProxy.h"

// ROOT include(s):
#include <TClass.h>
#include <TError.h>
#include <TInterpreter.h>

// System include(s):
#include <mutex>

namespace xAOD {

   /// The constructor of TDVCollectionProxy needs the dictionary of
   /// DataVector<xAOD::TDVCollectionProxyDummy> to be loaded already.
   /// This function makes sure that this is the case.
   ///
   void AddDVProxy::loadDictionaries() {

      // Enable library auto-loading. Only once per job.
      static std::once_flag libLoadFlag;
      std::call_once( libLoadFlag, []( TInterpreter& interpreter ) {

         // Enable library auto-loading.
         TClass::ReadRules();
         interpreter.LoadLibraryMap();
         interpreter.SetClassAutoloading( true );

         // Make sure that the minimal set of dictionaries are loaded:
         if( ! TClass::GetClass( "DataVector<xAOD::TDVCollectionProxyDummy>" ) ) {
            ::Error( "xAOD::AddDVProxy::loadDictionaries",
                     "Couldn't load the dictionary for "
                     "DataVector<xAOD::TDVCollectionProxyDummy>" );
         }

      }, *gInterpreter );

      return;
   }

} // namespace xAOD
