// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJETATHENAPOOL_XAODJETTRIGAUXCONTAINERCNV_H
#define XAODJETATHENAPOOL_XAODJETTRIGAUXCONTAINERCNV_H

#ifndef SIMULATIONBASE

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODJet/JetTrigAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::JetTrigAuxContainer,
                               xAOD::JetTrigAuxContainer >
   xAODJetTrigAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::JetTrigAuxContainer class
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
class xAODJetTrigAuxContainerCnv :
   public xAODJetTrigAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODJetTrigAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODJetTrigAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::JetTrigAuxContainer*
   createPersistent( xAOD::JetTrigAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::JetTrigAuxContainer* createTransient();

}; // class xAODJetTrigAuxContainerCnv
#endif // SIMULATIONBASE
#endif // XAODJETATHENAPOOL_XAODJETAUXCONTAINERCNV_H
