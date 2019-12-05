// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingAuxContainerCnv.h 566967 2013-10-24 13:24:31Z krasznaa $
#ifndef XAODBTAGGINGATHENAPOOL_XAODBTAGVERTEXAUXCONTAINERCNV_H
#define XAODBTAGGINGATHENAPOOL_XAODBTAGVERTEXAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODBTagging/BTagVertexAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::BTagVertexAuxContainer >
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
   public xAODBTagVertexAuxContainerCnvBase
{
public:
   using xAODBTagVertexAuxContainerCnvBase::xAODBTagVertexAuxContainerCnvBase;


   /// Function preparing the container to be written out
   virtual xAOD::BTagVertexAuxContainer*
   createPersistentWithKey( xAOD::BTagVertexAuxContainer* trans,
                            const std::string& key ) override;
}; // class xAODBTaggingAuxContainerCnv


#endif // XAODBTAGGINGATHENAPOOL_XAODBTAGGINGAUXCONTAINERCNV_H
