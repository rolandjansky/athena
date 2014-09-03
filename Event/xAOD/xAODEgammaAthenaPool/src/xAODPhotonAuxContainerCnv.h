// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPhotonAuxContainerCnv.h 614855 2014-09-03 11:42:57Z krasznaa $
#ifndef XAODEGAMMAATHENAPOOL_XAODPHOTONAUXCONTAINERCNV_H
#define XAODEGAMMAATHENAPOOL_XAODPHOTONAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODEgamma/PhotonAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::PhotonAuxContainer,
                               xAOD::PhotonAuxContainer >
   xAODPhotonAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::PhotonAuxContainer class
 *
 *         This is one of the the first xAOD POOL converter to implement
 *         real schema evolution. Because ROOT can't handle the change in
 *         inheritance that was done between _v1 and _v2.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 614855 $
 * $Date: 2014-09-03 13:42:57 +0200 (Wed, 03 Sep 2014) $
 */
class xAODPhotonAuxContainerCnv : public xAODPhotonAuxContainerCnvBase {

   /// Declare the factory as our friend
   friend class CnvFactory< xAODPhotonAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODPhotonAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::PhotonAuxContainer*
   createPersistent( xAOD::PhotonAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::PhotonAuxContainer* createTransient();

}; // class xAODPhotonAuxContainerCnv

#endif // XAODEGAMMAATHENAPOOL_XAODPHOTONAUXCONTAINERCNV_H
