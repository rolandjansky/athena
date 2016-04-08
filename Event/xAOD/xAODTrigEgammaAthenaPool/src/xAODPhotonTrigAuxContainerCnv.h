// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPhotonTrigAuxContainerCnv.h 705790 2015-11-04 13:09:23Z krasznaa $
#ifndef XAODTRIGEGAMMAATHENAPOOL_XAODPHOTONTRIGAUXCONTAINERCNV_H
#define XAODTRIGEGAMMAATHENAPOOL_XAODPHOTONTRIGAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigEgamma/PhotonTrigAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::PhotonTrigAuxContainer,
                               xAOD::PhotonTrigAuxContainer >
   xAODPhotonTrigAuxContainerCnvBase;

/**
 * @short POOL converter for the xAOD::PhotonTrigAuxContainer class
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 705790 $
 * $Date: 2015-11-04 14:09:23 +0100 (Wed, 04 Nov 2015) $
 */
class xAODPhotonTrigAuxContainerCnv :
   public xAODPhotonTrigAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODPhotonTrigAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODPhotonTrigAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::PhotonTrigAuxContainer*
   createPersistent( xAOD::PhotonTrigAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::PhotonTrigAuxContainer* createTransient();

}; // class xAODPhotonTrigAuxContainerCnv

#endif // XAODTRIGEGAMMAATHENAPOOL_XAODPHOTONTRIGAUXCONTAINERCNV_H
