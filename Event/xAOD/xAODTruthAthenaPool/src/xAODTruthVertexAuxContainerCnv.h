// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthVertexAuxContainerCnv.h 621073 2014-10-10 10:42:51Z krasznaa $
#ifndef XAODTRUTHATHENAPOOL_XAODTRUTHVERTEXAUXCONTAINERCNV_H
#define XAODTRUTHATHENAPOOL_XAODTRUTHVERTEXAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTruth/TruthVertexAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::TruthVertexAuxContainer,
                               xAOD::TruthVertexAuxContainer >
   xAODTruthVertexAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::TruthVertexAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::TruthVertexContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 621073 $
 * $Date: 2014-10-10 12:42:51 +0200 (Fri, 10 Oct 2014) $
 */
class xAODTruthVertexAuxContainerCnv :
   public xAODTruthVertexAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODTruthVertexAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODTruthVertexAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::TruthVertexAuxContainer*
   createPersistent( xAOD::TruthVertexAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::TruthVertexAuxContainer* createTransient();

}; // class xAODTruthVertexAuxContainerCnv

#endif // XAODTRUTHATHENAPOOL_XAODTRUTHVERTEXAUXCONTAINERCNV_H
