/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterCellLinkContainerCnv.cxx 781569 2016-11-01 12:16:14Z wlampl $

// System include(s):
#include <exception>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "CaloClusterCellLinkContainerCnv.h"

CaloClusterCellLinkContainerCnv::CaloClusterCellLinkContainerCnv( ISvcLocator* svcLoc )
  : CaloClusterCellLinkContainerCnvBase( svcLoc ) {

}


CaloClusterCellLinkContainer_p1* CaloClusterCellLinkContainerCnv::createPersistent(CaloClusterCellLinkContainer* trans ) {

  CaloClusterCellLinkContainer_p1* result=new CaloClusterCellLinkContainer_p1();
  m_converter.transToPers(trans,result,msg());
   return result;
}

CaloClusterCellLinkContainer* CaloClusterCellLinkContainerCnv::createTransient() {
  // The known ID(s) for this container:
  static pool::Guid v0_guid( "545AC204-2749-4AAC-9783-B1E5A7A0030F");
  static pool::Guid v1_guid( "C70A8262-05DB-48FC-8E4A-73793B4E58B9");
   // Check if we're reading the most up to date type:
  if( compareClassGuid( v0_guid ) ) {
    std::unique_ptr<CaloClusterCellLinkContainer> p( poolReadObject<CaloClusterCellLinkContainer >() );
    if( p->ownPolicy() == SG::VIEW_ELEMENTS ) {
      CaloClusterCellLinkContainer* result = new CaloClusterCellLinkContainer( p->begin(), p->end(), SG::OWN_ELEMENTS );
      return result;
    }
    return p.release();
  }

  if ( compareClassGuid(v1_guid) ) {
    std::unique_ptr<CaloClusterCellLinkContainer_p1> pers( poolReadObject<CaloClusterCellLinkContainer_p1 >() );

    CaloClusterCellLinkContainer* trans=new CaloClusterCellLinkContainer();
    m_converter.persToTrans(pers.get(),trans,msg());
    return trans;
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
