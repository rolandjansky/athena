/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "AFP_EventTPCnv/AFP_SIDLocRecoEvCollectionCnv_p1.h"
#include "AFP_EventTPCnv/AFP_SIDLocRecoEventCnv_p1.h"
#include "AFP_EventTPCnv/AFP_SIDSimHitCnv_p1.h"
#include "AFP_EventTPCnv/AFP_SIDSimHitCollectionCnv_p1.h"
#include "AFP_EventTPCnv/AFP_SiDigiCnv_p1.h"
#include "AFP_EventTPCnv/AFP_SiDigiCollectionCnv_p1.h"
#include "AFP_EventTPCnv/AFP_TDDigiCnv_p1.h"
#include "AFP_EventTPCnv/AFP_TDDigiCollectionCnv_p1.h"
#include "AFP_EventTPCnv/AFP_TDLocRecoEvCollectionCnv_p1.h"
#include "AFP_EventTPCnv/AFP_TDLocRecoEventCnv_p1.h"
#include "AFP_EventTPCnv/AFP_TDSimHitCnv_p1.h"
#include "AFP_EventTPCnv/AFP_TDSimHitCollectionCnv_p1.h"


DECLARE_TPCNV_FACTORY (AFP_SIDLocRecoEvCollectionCnv_p1,
                       AFP_SIDLocRecoEvCollection,
                       AFP_SIDLocRecoEvCollection_p1,
                       Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY (AFP_SIDLocRecoEventCnv_p1,
                       AFP_SIDLocRecoEvent,
                       AFP_SIDLocRecoEvent_p1,
                       Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY (AFP_SIDSimHitCnv_p1,
                       AFP_SIDSimHit,
                       AFP_SIDSimHit_p1,
                       Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY (AFP_SIDSimHitCollectionCnv_p1,
                       AFP_SIDSimHitCollection,
                       AFP_SIDSimHitCollection_p1,
                       Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY (AFP_SiDigiCnv_p1,
                       AFP_SiDigi,
                       AFP_SiDigi_p1,
                       Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY (AFP_SiDigiCollectionCnv_p1,
                       AFP_SiDigiCollection,
                       AFP_SiDigiCollection_p1,
                       Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY (AFP_TDDigiCnv_p1,
                       AFP_TDDigi,
                       AFP_TDDigi_p1,
                       Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY (AFP_TDDigiCollectionCnv_p1,
                       AFP_TDDigiCollection,
                       AFP_TDDigiCollection_p1,
                       Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY (AFP_TDLocRecoEvCollectionCnv_p1,
                       AFP_TDLocRecoEvCollection,
                       AFP_TDLocRecoEvCollection_p1,
                       Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY (AFP_TDLocRecoEventCnv_p1,
                       AFP_TDLocRecoEvent,
                       AFP_TDLocRecoEvent_p1,
                       Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY (AFP_TDSimHitCnv_p1,
                       AFP_TDSimHit,
                       AFP_TDSimHit_p1,
                       Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY (AFP_TDSimHitCollectionCnv_p1,
                       AFP_TDSimHitCollection,
                       AFP_TDSimHitCollection_p1,
                       Athena::TPCnvVers::Current)
