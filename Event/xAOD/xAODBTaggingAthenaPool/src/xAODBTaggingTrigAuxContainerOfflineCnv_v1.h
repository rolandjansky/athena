// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingTrigAuxContainerOfflineCnv_v1.h 705820 2015-11-04 14:54:05Z krasznaa $
#ifndef XAODBTAGGINGATHENAPOOL_XAODBTAGGINGTRIGAUXCONTAINEROFFLINECNV_V1_H
#define XAODBTAGGINGATHENAPOOL_XAODBTAGGINGTRIGAUXCONTAINEROFFLINECNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AuxContainerCopyTPCnv.h"

// EDM include(s):
#include "xAODBTagging/versions/BTaggingAuxContainer_v1.h"
#include "xAODBTagging/BTaggingTrigAuxContainer.h"

/// Converter class used to read xAOD::BTaggingAuxContainer_v1
///
/// Since we switched from using the offline xAOD::BTaggingAuxContainer
/// type to using a trigger specific xAOD::BTaggingTrigAuxContainer type
/// at the end of 2015, we now need to provide means of translating the offline
/// objects into the trigger ones automatically, to be able to read old
/// RAW files correctly.
///
typedef T_AuxContainerCopyTPCnv< xAOD::BTaggingTrigAuxContainer,
                                 xAOD::BTaggingAuxContainer_v1 >
  xAODBTaggingTrigAuxContainerOfflineCnv_v1;


#endif // XAODBTAGGINGATHENAPOOL_XAODBTAGGINGTRIGAUXCONTAINEROFFLINECNV_V1_H
