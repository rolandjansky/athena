/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

//#include "RecTPCnv/MuonContainerCnvARA_p1.h"
#include "RecTPCnv/MuonContainerCnv_p1.h"
#include "RecTPCnv/MuonContainerCnv_p2.h"
#include "RecTPCnv/MuonContainerCnv_p3.h"
#include "RecTPCnv/MuonContainerCnv_p4.h"
#include "RecTPCnv/MuonContainerCnv_p5.h"
#include "RecTPCnv/MuonContainerCnv_p6.h"

#include "RecTPCnv/MissingETCnv_p1.h"
#include "RecTPCnv/MissingETCnv_p2.h"
#include "RecTPCnv/MissingETCnv_p3.h"
#include "RecTPCnv/MissingETCompositionCnv_p1.h"
#include "RecTPCnv/MissingETCompositionCnv_p2.h"

#include "RecTPCnv/MissingEtCaloCnv_p1.h"
#include "RecTPCnv/MissingEtCaloCnv_p2.h"
#include "RecTPCnv/MissingEtCaloCnv_p3.h"
#include "RecTPCnv/MissingEtTruthCnv_p1.h"
#include "RecTPCnv/MissingEtTruthCnv_p2.h"
#include "RecTPCnv/MissingEtTruthCnv_p3.h"

#include "RecTPCnv/MuonCaloEnergyContainerCnv_tlp1.h"
#include "RecTPCnv/MuonCaloEnergyContainerCnv_p1.h"
#include "RecTPCnv/MuonSpShowerContainerCnv_p1.h"

// DECLARE_TPCNV_FACTORY(MuonContainerCnvARA_p1,
//                       Analysis::MuonContainer,
//                       MuonContainer_p1,
//                       Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MuonContainerCnv_p2,
                      Analysis::MuonContainer,
                      MuonContainer_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MuonContainerCnv_p3,
                      Analysis::MuonContainer,
                      MuonContainer_p3,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MuonContainerCnv_p4,
                      Analysis::MuonContainer,
                      MuonContainer_p4,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MuonContainerCnv_p5,
                      Analysis::MuonContainer,
                      MuonContainer_p5,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MuonContainerCnv_p6,
                      Analysis::MuonContainer,
                      MuonContainer_p6,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(MissingETCnv_p1,
                      MissingET,
                      MissingET_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MissingETCnv_p2,
                      MissingET,
                      MissingET_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MissingETCnv_p3,
                      MissingET,
                      MissingET_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_NAMED_TPCNV_FACTORY(MissingETCompositionCnv_p1,
                            MissingETCompositionCnv_p1,
                            MissingETComposition,
                            MissingETComposition_p1,
                            Athena::TPCnvVers::Old)
                      
DECLARE_NAMED_TPCNV_FACTORY(MissingETCompositionCnv_p2,
                            MissingETCompositionCnv_p2,
                            MissingETComposition,
                            MissingETComposition_p2,
                            Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(MissingEtCaloCnv_p1,
                      MissingEtCalo,
                      MissingEtCalo_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MissingEtCaloCnv_p2,
                      MissingEtCalo,
                      MissingEtCalo_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MissingEtCaloCnv_p3,
                      MissingEtCalo,
                      MissingEtCalo_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(MissingEtTruthCnv_p1,
                      MissingEtTruth,
                      MissingEtTruth_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MissingEtTruthCnv_p2,
                      MissingEtTruth,
                      MissingEtTruth_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MissingEtTruthCnv_p3,
                      MissingEtTruth,
                      MissingEtTruth_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(MuonCaloEnergyContainerCnv_tlp1,
                      MuonCaloEnergyContainer,
                      MuonCaloEnergyContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MuonCaloEnergyContainerCnv_p1,
                      MuonCaloEnergyContainer,
                      MuonCaloEnergyContainer_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(MuonSpShowerContainerCnv_p1,
                      Rec::MuonSpShowerContainer,
                      MuonSpShowerContainer_p1,
                      Athena::TPCnvVers::Current)
                      
