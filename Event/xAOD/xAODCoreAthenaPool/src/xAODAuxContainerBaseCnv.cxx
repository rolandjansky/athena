/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODAuxContainerBaseCnv.cxx 635273 2014-12-11 13:58:24Z krasznaa $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// Local include(s):
#include "xAODAuxContainerBaseCnv.h"

xAODAuxContainerBaseCnv::xAODAuxContainerBaseCnv( ISvcLocator* svcLoc )
   : xAODAuxContainerBaseCnvBase( svcLoc ) {

}

xAOD::AuxContainerBase*
xAODAuxContainerBaseCnv::createPersistent( xAOD::AuxContainerBase* trans ) {

   // Access the thinning svc, if it is defined for the current object:
   IThinningSvc* thinSvc = IThinningSvc::instance();

   // Create the persistent object using the helper function from AthContainers:
   return SG::copyThinned( *trans, thinSvc );
}

xAOD::AuxContainerBase* xAODAuxContainerBaseCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid id( "C87C71B3-B03F-42FC-AF99-DF497F148397" );

   // It's very unlikely that we'll ever have a different ID for this type,
   // but let's check nevertheless:
   if( compareClassGuid( id ) ) {
      // Yep, it's the right version:
      return poolReadObject< xAOD::AuxContainerBase >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of xAOD::AuxContainerBase "
                             "found" );
   return 0;
}
