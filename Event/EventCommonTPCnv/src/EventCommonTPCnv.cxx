/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "EventCommonTPCnv/HepLorentzVectorCnv_p1.h"
#include "EventCommonTPCnv/INav4MomAssocsCnv_p1.h"
#include "EventCommonTPCnv/INav4MomAssocsCnv_p2.h"
#include "EventCommonTPCnv/INav4MomAssocsCnv_p3.h"
#include "EventCommonTPCnv/INav4MomLinkContainerCnv_p1.h"
#include "EventCommonTPCnv/IParticleLinkContainerCnv_p1.h"
#include "EventCommonTPCnv/P4EEtaPhiMCnv_p1.h"
#include "EventCommonTPCnv/P4EEtaPhiMCnv_p2.h"
#include "EventCommonTPCnv/P4IPtCotThPhiMCnv_p1.h"
#include "EventCommonTPCnv/P4ImplEEtaPhiMCnv_p1.h"
#include "EventCommonTPCnv/P4ImplEEtaPhiMCnv_p2.h"
#include "EventCommonTPCnv/P4ImplIPtCotThPhiMCnv_p1.h"
#include "EventCommonTPCnv/P4ImplPtEtaPhiMCnv_p1.h"
#include "EventCommonTPCnv/P4ImplPtEtaPhiMCnv_p2.h"
#include "EventCommonTPCnv/P4ImplPxPyPzECnv_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p2.h"
#include "EventCommonTPCnv/P4PxPyPzECnv_p1.h"

DECLARE_TPCNV_FACTORY(INav4MomAssocsCnv_p1,
                      INav4MomAssocs,
                      INav4MomAssocs_p1,
                      Athena::TPCnvVers::Old)
DECLARE_TPCNV_FACTORY(INav4MomAssocsCnv_p2,
                      INav4MomAssocs,
                      INav4MomAssocs_p2,
                      Athena::TPCnvVers::Old)
DECLARE_TPCNV_FACTORY(INav4MomAssocsCnv_p3,
                      INav4MomAssocs,
                      INav4MomAssocs_p3,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(INav4MomLinkContainerCnv_p1,
                      INav4MomLinkContainer,
                      INav4MomLinkContainer_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(IParticleLinkContainerCnv_p1,
                      IParticleLinkContainer,
                      IParticleLinkContainer_p1,
                      Athena::TPCnvVers::Current)

