// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingTrigAuxContainerCnv.h 705820 2015-11-04 14:54:05Z krasznaa $
#ifndef XAODBTAGGINGATHENAPOOL_XAODBTAGGINGTRIGAUXCONTAINERCNV_H
#define XAODBTAGGINGATHENAPOOL_XAODBTAGGINGTRIGAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODBTagging/BTaggingTrigAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::BTaggingTrigAuxContainer,
                               xAOD::BTaggingTrigAuxContainer >
   xAODBTaggingTrigAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::BTaggingTrigAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::BTaggingContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 705820 $
 * $Date: 2015-11-04 15:54:05 +0100 (Wed, 04 Nov 2015) $
 */
class xAODBTaggingTrigAuxContainerCnv :
   public xAODBTaggingTrigAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODBTaggingTrigAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODBTaggingTrigAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::BTaggingTrigAuxContainer*
   createPersistent( xAOD::BTaggingTrigAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::BTaggingTrigAuxContainer* createTransient();

}; // class xAODBTaggingTrigAuxContainerCnv

#endif // XAODBTAGGINGATHENAPOOL_XAODBTAGGINGTRIGAUXCONTAINERCNV_H
