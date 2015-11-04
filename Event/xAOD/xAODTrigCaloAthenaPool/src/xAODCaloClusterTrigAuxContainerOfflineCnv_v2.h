// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterTrigAuxContainerOfflineCnv_v2.h 705793 2015-11-04 13:13:26Z krasznaa $
#ifndef XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINEROFFLINECNV_V2_H
#define XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINEROFFLINECNV_V2_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v2.h"
#include "xAODTrigCalo/CaloClusterTrigAuxContainer.h"

/// Converter class used to read xAOD::CaloClusterAuxContainer_v2
///
/// Since we switched from using the offline xAOD::CaloClusterAuxContainer
/// type to using a trigger specific xAOD::CaloClusterTrigAuxContainer type
/// at the end of 2015, we now need to provide means of translating the offline
/// objects into the trigger ones automatically, to be able to read old
/// RAW files correctly.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 705793 $
/// $Date: 2015-11-04 14:13:26 +0100 (Wed, 04 Nov 2015) $
///
class xAODCaloClusterTrigAuxContainerOfflineCnv_v2 :
   public T_AthenaPoolTPCnvBase< xAOD::CaloClusterTrigAuxContainer,
                                 xAOD::CaloClusterAuxContainer_v2 > {

public:
   /// Default constructor
   xAODCaloClusterTrigAuxContainerOfflineCnv_v2();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::CaloClusterAuxContainer_v2* oldObj,
                             xAOD::CaloClusterTrigAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::CaloClusterTrigAuxContainer*,
                             xAOD::CaloClusterAuxContainer_v2*,
                             MsgStream& log );

}; // class xAODCaloClusterTrigAuxContainerOfflineCnv_v2

#endif // XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINEROFFLINECNV_V2_H
