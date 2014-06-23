// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingAuxContainerCnv.h 566967 2013-10-24 13:24:31Z krasznaa $
#ifndef XAODBTAGGINGATHENAPOOL_XAODBTAGVERTEXAUXCONTAINERCNV_H
#define XAODBTAGGINGATHENAPOOL_XAODBTAGVERTEXAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODBTagging/BTagVertexAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::BTagVertexAuxContainer,
                               xAOD::BTagVertexAuxContainer >
   xAODBTagVertexAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::BTaggingAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::BTaggingContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 566967 $
 * $Date: 2013-10-24 15:24:31 +0200 (Thu, 24 Oct 2013) $
 */
class xAODBTagVertexAuxContainerCnv :
   public xAODBTagVertexAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODBTagVertexAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODBTagVertexAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::BTagVertexAuxContainer*
   createPersistent( xAOD::BTagVertexAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::BTagVertexAuxContainer* createTransient();

}; // class xAODBTaggingAuxContainerCnv

#endif // XAODBTAGGINGATHENAPOOL_XAODBTAGGINGAUXCONTAINERCNV_H
