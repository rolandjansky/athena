// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauJetContainerCnv.h 632169 2014-11-29 15:13:34Z krasznaa $
#ifndef XAODTAUEVENTATHENAPOOL_XAODTAUJETCONTAINERCNV_H
#define XAODTAUEVENTATHENAPOOL_XAODTAUJETCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"

// EDM include(s):
#include "xAODTau/TauJetContainer.h"
#include "xAODTauJetContainerCnv_v1.h"
#include "xAODTauJetContainerCnv_v2.h"

/// Type definition for the converter's base
typedef T_AthenaPoolxAODCnv< xAOD::TauJetContainer,
                             xAODTauJetContainerCnv_v2,
                             xAODTauJetContainerCnv_v1 >
   xAODTauJetContainerCnv;


#endif // XAODTAUEVENTATHENAPOOL_XAODTAUJETCONTAINERCNV_H
