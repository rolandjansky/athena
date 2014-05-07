// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauJetAuxContainerCnv.h 581660 2014-02-05 15:52:29Z janus $
#ifndef XAODTAUATHENAPOOL_XAODTAUJETAUXCONTAINERCNV_H
#define XAODTAUATHENAPOOL_XAODTAUJETAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTau/TauJetAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TauJetAuxContainer,
                               xAOD::TauJetAuxContainer >
   xAODTauJetAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TauJetAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TauJetContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author Michel Janus <janus@cern.ch>
 *
 * $Revision: 581660 $
 * $Date: 2014-02-05 16:52:29 +0100 (Wed, 05 Feb 2014) $
 */
class xAODTauJetAuxContainerCnv :
   public xAODTauJetAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTauJetAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTauJetAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TauJetAuxContainer*
   createPersistent( xAOD::TauJetAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TauJetAuxContainer* createTransient();

}; // class xAODTauJetAuxContainerCnv

#endif // XAODTAUATHENAPOOL_XAODTAUJETAUXCONTAINERCNV_H
