/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TrigTopoEventTPCnv/ElectronMuonTopoInfoContainerCnv_p1.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfoContainer_p1.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfoCnv_p1.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfo_p1.h"

DECLARE_TPCNV_FACTORY(ElectronMuonTopoInfoContainerCnv_p1,
                      ElectronMuonTopoInfoContainer,
                      ElectronMuonTopoInfoContainer_p1,
                      Athena::TPCnvVers::Current)
