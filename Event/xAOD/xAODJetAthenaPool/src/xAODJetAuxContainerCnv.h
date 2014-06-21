// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODJetAuxContainerCnv.h 566967 2013-10-24 13:24:31Z krasznaa $
#ifndef XAODJETATHENAPOOL_XAODJETAUXCONTAINERCNV_H
#define XAODJETATHENAPOOL_XAODJETAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODJet/JetAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::JetAuxContainer,
                               xAOD::JetAuxContainer >
   xAODJetAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JetAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::JetContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 566967 $
 * $Date: 2013-10-24 15:24:31 +0200 (Thu, 24 Oct 2013) $
 */
class xAODJetAuxContainerCnv :
   public xAODJetAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODJetAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODJetAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::JetAuxContainer*
   createPersistent( xAOD::JetAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::JetAuxContainer* createTransient();

}; // class xAODJetAuxContainerCnv

#endif // XAODJETATHENAPOOL_XAODJETAUXCONTAINERCNV_H
