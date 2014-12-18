// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterAuxContainerCnv.h 628099 2014-11-13 09:30:05Z krasznaa $
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
 *         This is the first xAOD POOL converter to implement real schema
 *         evolution. Because ROOT can't handle the change in inheritance
 *         that was done between _v1 and _v2.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 628099 $
 * $Date: 2014-11-13 10:30:05 +0100 (Thu, 13 Nov 2014) $
 */
class xAODCaloClusterAuxContainerCnv : public xAODCaloClusterAuxContainerCnvBase {

   /// Declare the factory as our friend
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
