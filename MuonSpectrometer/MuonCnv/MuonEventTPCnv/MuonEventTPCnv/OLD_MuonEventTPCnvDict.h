/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTTPCNV_OLD_MUONEVENTTPCNVDICT_H
#define MUONEVENTTPCNV_OLD_MUONEVENTTPCNVDICT_H

//-----------------------------------------------------------------------------
//
// file:   MuonEventTPCnvDict_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonEventTPCnv/MuonMeasurements_tlp1.h"
#include "MuonEventTPCnv/MuonMeasurements_tlp2.h"

#include "MuonEventTPCnv/CscPrepDataContainer_tlp1.h"
#include "MuonEventTPCnv/CscStripPrepDataContainer_tlp1.h"
#include "MuonEventTPCnv/RpcPrepDataContainer_tlp1.h"
#include "MuonEventTPCnv/TgcPrepDataContainer_tlp1.h"
#include "MuonEventTPCnv/MdtPrepDataContainer_tlp1.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Collection_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Collection_p2.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"
#include "MuonEventTPCnv/MuonPrepRawData/MdtPrepData_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/MdtPrepData_p2.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/RpcClusterOnTrack_p2.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/RpcClusterOnTrack_p1.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegment_p1.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegmentQuality_p1.h"
#include "MuonEventTPCnv/MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/MuonCoinDataCollection_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/MuonCoinDataContainer_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/RpcCoinData_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinData_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinData_p2.h"
#include "MuonEventTPCnv/TgcCoinDataContainer_tlp1.h"
#include "MuonEventTPCnv/TgcCoinDataContainer_tlp2.h"
#include "MuonEventTPCnv/MuonPrepRawData/MdtTwinPrepData_p1.h"
#include "MuonEventTPCnv/MuonChamberT0s/ChamberT0s_p1.h"

struct OLD_MuonEventTPCnvDict 
{
    std::vector< Muon::CscPrepData_p1>              m_v1;
    std::vector< Muon::RpcPrepData_p1>              m_v3;
    std::vector< Muon::TgcPrepData_p1 >             m_v4;
    std::vector< Muon::MdtPrepData_p1 >             m_v5;
    std::vector< Muon::MdtPrepData_p2 >             m_v5a;
    std::vector< Muon::MuonPRD_Collection_p1 >      m_v6;
    std::vector< Muon::MuonPRD_Container_p1 >       m_v7;
    std::vector< Muon::TgcCoinData_p1>              m_v13;
    std::vector<Muon::RpcClusterOnTrack_p2>         m_v14;
    std::vector<Muon::MuonPRD_Collection_p1>        m_v15;
};

#endif // MUONEVENTTPCNV_OLD_MUONEVENTTPCNVDICT_H
