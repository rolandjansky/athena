// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigCaloClusterAuxContainerCnv.h
#ifndef XAODTRIGCALOATHENAPOOL_XAODTRIGCALOCLUSTERAUXCONTAINERCNV_H
#define XAODTRIGCALOATHENAPOOL_XAODTRIGCALOCLUSTERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigCalo/TrigCaloClusterAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TrigCaloClusterAuxContainer,
                               xAOD::TrigCaloClusterAuxContainer >
   xAODTrigCaloClusterAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TrigCaloClusterAuxContainer class
 *
 * @author Aranzazu Ruiz Martinez <Aranzazu.Ruiz.Martinez@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 592328 $
 * $Date: 2014-04-10 12:58:05 +0200 (Thu, 10 Apr 2014) $
 */
class xAODTrigCaloClusterAuxContainerCnv :
   public xAODTrigCaloClusterAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTrigCaloClusterAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTrigCaloClusterAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TrigCaloClusterAuxContainer*
   createPersistent( xAOD::TrigCaloClusterAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TrigCaloClusterAuxContainer* createTransient();

}; // class xAODTrigCaloClusterAuxContainerCnv

#endif // XAODTRIGCALOATHENAPOOL_XAODTRIGCALOCLUSTERAUXCONTAINERCNV_H
