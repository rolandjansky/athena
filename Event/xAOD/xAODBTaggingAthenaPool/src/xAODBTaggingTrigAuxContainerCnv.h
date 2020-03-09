// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingTrigAuxContainerCnv.h 705820 2015-11-04 14:54:05Z krasznaa $
#ifndef XAODBTAGGINGATHENAPOOL_XAODBTAGGINGTRIGAUXCONTAINERCNV_H
#define XAODBTAGGINGATHENAPOOL_XAODBTAGGINGTRIGAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODBTagging/BTaggingTrigAuxContainer.h"
#include "xAODBTaggingTrigAuxContainerOfflineCnv_v1.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::BTaggingTrigAuxContainer,
                                     xAODBTaggingTrigAuxContainerOfflineCnv_v1 >
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
   public xAODBTaggingTrigAuxContainerCnvBase
{
public:
   using xAODBTaggingTrigAuxContainerCnvBase::xAODBTaggingTrigAuxContainerCnvBase;


   /// Function preparing the container to be written out
   virtual xAOD::BTaggingTrigAuxContainer*
   createPersistentWithKey( xAOD::BTaggingTrigAuxContainer* trans,
                            const std::string& key) override;
}; // class xAODBTaggingTrigAuxContainerCnv


#endif // XAODBTAGGINGATHENAPOOL_XAODBTAGGINGTRIGAUXCONTAINERCNV_H
