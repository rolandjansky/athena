// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronAuxContainerCnv.h 614830 2014-09-03 09:41:57Z krasznaa $
#ifndef XAODEGAMMAATHENAPOOL_XAODELECTRONAUXCONTAINERCNV_H
#define XAODEGAMMAATHENAPOOL_XAODELECTRONAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODEgamma/ElectronAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::ElectronAuxContainer,
                               xAOD::ElectronAuxContainer >
   xAODElectronAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::ElectronAuxContainer class
 *
 *         This is the first xAOD POOL converter to implement real schema
 *         evolution. Because ROOT can't handle the change in inheritance
 *         that was done between _v1 and _v2.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 614830 $
 * $Date: 2014-09-03 11:41:57 +0200 (Wed, 03 Sep 2014) $
 */
class xAODElectronAuxContainerCnv : public xAODElectronAuxContainerCnvBase {

   /// Declare the factory as our friend
   friend class CnvFactory< xAODElectronAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODElectronAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::ElectronAuxContainer*
   createPersistent( xAOD::ElectronAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::ElectronAuxContainer* createTransient();

}; // class xAODElectronAuxContainerCnv

#endif // XAODEGAMMAATHENAPOOL_XAODELECTRONAUXCONTAINERCNV_H
