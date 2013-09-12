/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TrigMissingEtEventTPCnv/TrigMissingET_p1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingET_p2.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_tlp1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainerCnv_tlp1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_p1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p2.h"

#include "TrigMissingEtEventTPCnv/TrigMissingET_p3.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_p3.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainerCnv_p3.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p3.h"

DECLARE_TPCNV_FACTORY(TrigMissingETCnv_p1,
                      TrigMissingET,
                      TrigMissingET_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigMissingETCnv_p2,
                      TrigMissingET,
                      TrigMissingET_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigMissingETContainerCnv_tlp1,
                      TrigMissingETContainer,
                      TrigMissingETContainer_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigMissingETContainerCnv_p3,
                      TrigMissingETContainer,
                      TrigMissingETContainer_p3,
                      Athena::TPCnvVers::Current)
                      
