// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterAuxContainerCnv.h 565402 2013-10-15 09:44:40Z krasznaa $
#ifndef XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERAUXCONTAINERCNV_H
#define XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::CaloClusterAuxContainer,
                               xAOD::CaloClusterAuxContainer >
   xAODCaloClusterAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CaloClusterAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::CaloClusterContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 565402 $
 * $Date: 2013-10-15 11:44:40 +0200 (Tue, 15 Oct 2013) $
 */
class xAODCaloClusterAuxContainerCnv :
   public xAODCaloClusterAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODCaloClusterAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODCaloClusterAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CaloClusterAuxContainer*
   createPersistent( xAOD::CaloClusterAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::CaloClusterAuxContainer* createTransient();

}; // class xAODCaloClusterAuxContainerCnv

#endif // XAODCALOEVENTATHENAPOOL_XAODCALOCLUSTERAUXCONTAINERCNV_H
