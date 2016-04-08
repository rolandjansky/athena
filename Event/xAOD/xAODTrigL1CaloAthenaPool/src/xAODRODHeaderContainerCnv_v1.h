// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRODHeaderContainerCnv_v1.h 694352 2015-09-11 12:05:29Z krasznaa $
#ifndef XAODTRIGL1CALO_XAODRODHEADERCONTAINERCNV_V1_H
#define XAODTRIGL1CALO_XAODRODHEADERCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigL1Calo/versions/RODHeaderContainer_v1.h"
#include "xAODTrigL1Calo/RODHeaderContainer.h"

/// Converter class for reading xAOD::RODHeaderContainer_v1
///
/// This is a very simple converter, as the interface container
/// doesn't contain any payload. So the code is mostly just demonstrating
/// how to write a skeleton for a schema evolution.
///
/// @author Attila Krasznahorkay@cern.ch <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 694352 $
/// $Date: 2015-09-11 14:05:29 +0200 (Fri, 11 Sep 2015) $
///
class xAODRODHeaderContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::RODHeaderContainer,
                                 xAOD::RODHeaderContainer_v1 > {

public:
   /// Default constructor
   xAODRODHeaderContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::RODHeaderContainer_v1* oldObj,
                             xAOD::RODHeaderContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::RODHeaderContainer*,
                             xAOD::RODHeaderContainer_v1*,
                             MsgStream& log );

}; // class xAODRODHeaderContainerCnv_v1

#endif // XAODTRIGL1CALO_XAODRODHEADERCONTAINERCNV_V1_H
