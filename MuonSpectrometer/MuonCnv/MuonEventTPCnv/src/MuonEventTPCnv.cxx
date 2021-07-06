/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "MuonEventTPCnv/MuonChamberT0s/ChamberT0sCnv_p1.h"

#include "MuonEventTPCnv/CscPrepDataContainerCnv_tlp1.h"
#include "MuonEventTPCnv/CscStripPrepDataContainerCnv_tlp1.h"
#include "MuonEventTPCnv/MdtPrepDataContainerCnv_tlp1.h"
#include "MuonEventTPCnv/RpcPrepDataContainerCnv_tlp1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/RpcCoinDataContainerCnv_p1.h"
#include "MuonEventTPCnv/TgcCoinDataContainerCnv_tlp1.h"
#include "MuonEventTPCnv/TgcCoinDataContainerCnv_tlp2.h"
#include "MuonEventTPCnv/TgcCoinDataContainerCnv_tlp3.h"
#include "MuonEventTPCnv/TgcPrepDataContainerCnv_tlp1.h"

DECLARE_TPCNV_FACTORY(ChamberT0sCnv_p1,
                      Muon::ChamberT0s,
                      Muon::ChamberT0s_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(CscPrepDataContainerCnv_tlp1,
                      Muon::CscPrepDataContainer,
                      Muon::CscPrepDataContainer_tlp1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(CscStripPrepDataContainerCnv_tlp1,
                      Muon::CscStripPrepDataContainer,
                      Muon::CscStripPrepDataContainer_tlp1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(MdtPrepDataContainerCnv_tlp1,
                      Muon::MdtPrepDataContainer,
                      Muon::MdtPrepDataContainer_tlp1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(RpcPrepDataContainerCnv_tlp1,
                      Muon::RpcPrepDataContainer,
                      Muon::RpcPrepDataContainer_tlp1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(Muon::RpcCoinDataContainerCnv_p1,
                      Muon::RpcCoinDataContainer,
                      Muon::MuonPRD_Container_p2<RpcCoinData_p1>,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(TgcCoinDataContainerCnv_tlp1,
                      Muon::TgcCoinDataContainer,
                      Muon::TgcCoinDataContainer_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TgcCoinDataContainerCnv_tlp2,
                      Muon::TgcCoinDataContainer,
                      Muon::TgcCoinDataContainer_tlp2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TgcCoinDataContainerCnv_tlp3,
                      Muon::TgcCoinDataContainer,
                      Muon::TgcCoinDataContainer_tlp3,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(TgcPrepDataContainerCnv_tlp1,
                      Muon::TgcPrepDataContainer,
                      Muon::TgcPrepDataContainer_tlp1,
                      Athena::TPCnvVers::Current)
