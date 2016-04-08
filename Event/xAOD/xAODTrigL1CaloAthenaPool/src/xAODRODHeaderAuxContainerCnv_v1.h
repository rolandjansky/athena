// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRODHeaderAuxContainerCnv_v1.h 694352 2015-09-11 12:05:29Z krasznaa $
#ifndef XAODTRIGL1CALO_XAODRODHEADERAUXCONTAINERCNV_V1_H
#define XAODTRIGL1CALO_XAODRODHEADERAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigL1Calo/versions/RODHeaderAuxContainer_v1.h"
#include "xAODTrigL1Calo/RODHeaderAuxContainer.h"

/// Converter class for reading xAOD::RODHeaderAuxContainer_v1
///
/// This is where most of the conversion happens. Translating the payload
/// of the old object into a new one.
///
/// @author Attila Krasznahorkay@cern.ch <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 694352 $
/// $Date: 2015-09-11 14:05:29 +0200 (Fri, 11 Sep 2015) $
///
class xAODRODHeaderAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::RODHeaderAuxContainer,
                                 xAOD::RODHeaderAuxContainer_v1 > {

public:
   /// Default constructor
   xAODRODHeaderAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::RODHeaderAuxContainer_v1* oldObj,
                             xAOD::RODHeaderAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::RODHeaderAuxContainer*,
                             xAOD::RODHeaderAuxContainer_v1*,
                             MsgStream& log );

}; // class xAODRODHeaderAuxContainerCnv_v1

#endif // XAODTRIGL1CALO_XAODRODHEADERAUXCONTAINERCNV_V1_H
