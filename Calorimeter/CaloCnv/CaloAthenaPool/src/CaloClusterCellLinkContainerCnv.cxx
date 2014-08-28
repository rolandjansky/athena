/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterCellLinkContainerCnv.cxx 613575 2014-08-26 19:23:37Z gemmeren $

// System include(s):
#include <exception>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "CaloClusterCellLinkContainerCnv.h"

CaloClusterCellLinkContainerCnv::CaloClusterCellLinkContainerCnv( ISvcLocator* svcLoc )
  : CaloClusterCellLinkContainerCnvBase( svcLoc ) {

}


CaloClusterCellLinkContainer* CaloClusterCellLinkContainerCnv::createPersistent(CaloClusterCellLinkContainer* trans ) {


   // Create a view copy of the container:
   CaloClusterCellLinkContainer* result =
     new CaloClusterCellLinkContainer( trans->begin(), trans->end(),
   				       SG::VIEW_ELEMENTS );
   result->toPersistent();

   // Return the new container:
   return result;
}

CaloClusterCellLinkContainer* CaloClusterCellLinkContainerCnv::createTransient() {
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "545AC204-2749-4AAC-9783-B1E5A7A0030F");

   // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    std::unique_ptr<CaloClusterCellLinkContainer> p( poolReadObject<CaloClusterCellLinkContainer >() );
    if( p->ownPolicy() == SG::VIEW_ELEMENTS ) {
      CaloClusterCellLinkContainer* result = new CaloClusterCellLinkContainer( p->begin(), p->end(), SG::OWN_ELEMENTS );
      return result;
    }
    return p.release();
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
                             "CaloClusterCellLinkContainer found" );
  return 0;
}

void CaloClusterCellLinkContainerCnv::toPersistent(CaloClusterCellLink* lnk ) const {

   lnk->toPersistent();
   return;
}
