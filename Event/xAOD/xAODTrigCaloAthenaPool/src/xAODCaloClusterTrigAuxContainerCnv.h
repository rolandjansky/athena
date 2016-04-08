// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterTrigAuxContainerCnv.h
#ifndef XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINERCNV_H
#define XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigCalo/CaloClusterTrigAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::CaloClusterTrigAuxContainer,
                               xAOD::CaloClusterTrigAuxContainer >
   xAODCaloClusterTrigAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CaloClusterTrigAuxContainer class
 *
 * @author Aranzazu Ruiz Martinez <Aranzazu.Ruiz.Martinez@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 705793 $
 * $Date: 2015-11-04 14:13:26 +0100 (Wed, 04 Nov 2015) $
 */
class xAODCaloClusterTrigAuxContainerCnv :
   public xAODCaloClusterTrigAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCaloClusterTrigAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODCaloClusterTrigAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CaloClusterTrigAuxContainer*
   createPersistent( xAOD::CaloClusterTrigAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::CaloClusterTrigAuxContainer* createTransient();

}; // class xAODCaloClusterTrigAuxContainerCnv

#endif // XAODTRIGCALOATHENAPOOL_XAODCALOCLUSTERTRIGAUXCONTAINERCNV_H
