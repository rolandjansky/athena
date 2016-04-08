// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronTrigAuxContainerCnv.h 705790 2015-11-04 13:09:23Z krasznaa $
#ifndef XAODTRIGEGAMMAATHENAPOOL_XAODELECTRONTRIGAUXCONTAINERCNV_H
#define XAODTRIGEGAMMAATHENAPOOL_XAODELECTRONTRIGAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigEgamma/ElectronTrigAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::ElectronTrigAuxContainer,
                               xAOD::ElectronTrigAuxContainer >
   xAODElectronTrigAuxContainerCnvBase;

/**
 * @short POOL converter for the xAOD::ElectronTrigAuxContainer class
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 705790 $
 * $Date: 2015-11-04 14:09:23 +0100 (Wed, 04 Nov 2015) $
 */
class xAODElectronTrigAuxContainerCnv :
   public xAODElectronTrigAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODElectronTrigAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODElectronTrigAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::ElectronTrigAuxContainer*
   createPersistent( xAOD::ElectronTrigAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::ElectronTrigAuxContainer* createTransient();

}; // class xAODElectronTrigAuxContainerCnv

#endif // XAODTRIGEGAMMAATHENAPOOL_XAODELECTRONTRIGAUXCONTAINERCNV_H
