// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloTowerAuxContainerCnv.h 700671 2015-10-15 08:33:38Z wlampl $
#ifndef XAODCALOEVENTATHENAPOOL_XAODCALOTOWERAUXCONTAINERCNV_H
#define XAODCALOEVENTATHENAPOOL_XAODCALOTOWERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODCaloEvent/CaloTowerAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::CaloTowerAuxContainer,
                               xAOD::CaloTowerAuxContainer >
   xAODCaloTowerAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::CaloTowerAuxContainer class
 *
 *         This is the first xAOD POOL converter to implement real schema
 *         evolution. Because ROOT can't handle the change in inheritance
 *         that was done between _v1 and _v2.
 *
 *
 * $Revision: 700671 $
 * $Date: 2015-10-15 10:33:38 +0200 (Thu, 15 Oct 2015) $
 */
class xAODCaloTowerAuxContainerCnv : public xAODCaloTowerAuxContainerCnvBase {

   /// Declare the factory as our friend
   friend class CnvFactory< xAODCaloTowerAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODCaloTowerAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::CaloTowerAuxContainer*
   createPersistent( xAOD::CaloTowerAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::CaloTowerAuxContainer* createTransient();

}; // class xAODCaloTowerAuxContainerCnv

#endif // XAODCALOEVENTATHENAPOOL_XAODCALOTOWERAUXCONTAINERCNV_H
