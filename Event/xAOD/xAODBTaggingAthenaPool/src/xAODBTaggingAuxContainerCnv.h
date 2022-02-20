// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingAuxContainerCnv.h 566967 2013-10-24 13:24:31Z krasznaa $
#ifndef XAODBTAGGINGATHENAPOOL_XAODBTAGGINGAUXCONTAINERCNV_H
#define XAODBTAGGINGATHENAPOOL_XAODBTAGGINGAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTaggingAuxContainerCnv_v1.h"

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
using xAODBTaggingAuxContainerCnv = T_AthenaPoolAuxContainerCnv<
  xAOD::BTaggingAuxContainer,
  xAODBTaggingAuxContainerCnv_v1
  >;

#endif // XAODBTAGGINGATHENAPOOL_XAODBTAGGINGAUXCONTAINERCNV_H
