/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "tauEventTPCnv/TauDetailsContainerCnv_p1.h"
#include "tauEventTPCnv/TauDetailsContainerCnv_tlp1.h"
#include "tauEventTPCnv/TauDetailsContainerCnv_tlp2.h"
#include "tauEventTPCnv/TauDetailsContainerCnv_tlp3.h"
#include "tauEventTPCnv/TauDetailsContainerCnv_tlp4.h"

#include "tauEvent/TauJetContainer.h"
#include "tauEventTPCnv/TauJetContainerCnv_p1.h"
#include "tauEventTPCnv/TauJetContainerCnv_p2.h"
#include "tauEventTPCnv/TauJetContainerCnv_p3.h"
#include "tauEventTPCnv/TauJetContainerCnv_p4.h"
#include "tauEventTPCnv/TauJetContainerCnv_p5.h"

DECLARE_TPCNV_FACTORY(TauDetailsContainerCnv_tlp1,
                      Analysis::TauDetailsContainer,
                      TauDetailsContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TauDetailsContainerCnv_tlp2,
                      Analysis::TauDetailsContainer,
                      TauDetailsContainer_tlp2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TauDetailsContainerCnv_tlp3,
                      Analysis::TauDetailsContainer,
                      TauDetailsContainer_tlp3,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TauDetailsContainerCnv_tlp4,
                      Analysis::TauDetailsContainer,
                      TauDetailsContainer_tlp4,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(TauJetContainerCnv_p1,
                      Analysis::TauJetContainer,
                      TauJetContainer_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TauJetContainerCnv_p2,
                      Analysis::TauJetContainer,
                      TauJetContainer_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TauJetContainerCnv_p3,
                      Analysis::TauJetContainer,
                      TauJetContainer_p3,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TauJetContainerCnv_p4,
                      Analysis::TauJetContainer,
                      TauJetContainer_p4,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TauJetContainerCnv_p5,
                      Analysis::TauJetContainer,
                      TauJetContainer_p5,
                      Athena::TPCnvVers::Current)

