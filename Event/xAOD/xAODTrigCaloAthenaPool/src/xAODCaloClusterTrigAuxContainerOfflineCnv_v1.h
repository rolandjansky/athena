// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterTrigAuxContainerOfflineCnv_v1.h 705793 2015-11-04 13:13:26Z krasznaa $
#ifndef XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINEROFFLINECNV_V1_H
#define XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINEROFFLINECNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v1.h"
#include "xAODTrigCalo/CaloClusterTrigAuxContainer.h"

/// Converter class used to read xAOD::CaloClusterAuxContainer_v1
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
class xAODCaloClusterTrigAuxContainerOfflineCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::CaloClusterTrigAuxContainer,
                                 xAOD::CaloClusterAuxContainer_v1 > {

public:
   /// Default constructor
   xAODCaloClusterTrigAuxContainerOfflineCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::CaloClusterAuxContainer_v1* oldObj,
                             xAOD::CaloClusterTrigAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::CaloClusterTrigAuxContainer*,
                             xAOD::CaloClusterAuxContainer_v1*,
                             MsgStream& log );

}; // class xAODCaloClusterTrigAuxContainerOfflineCnv_v1

#endif // XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINEROFFLINECNV_V1_H
