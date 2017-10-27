/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTTPCNV_MUONEVENTTPCNVDICT_H
#define MUONEVENTTPCNV_MUONEVENTTPCNVDICT_H

//-----------------------------------------------------------------------------
//
// file:   MuonEventTPCnvDict_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonEventTPCnv/MuonMeasurements_tlp3.h"

#include "MuonEventTPCnv/CscStripPrepDataContainer_tlp1.h"

#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Collection_p2.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegment_p1.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegmentQuality_p1.h"
#include "MuonEventTPCnv/MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/MuonCoinDataCollection_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/MuonCoinDataContainer_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/RpcCoinData_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinData_p1.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinData_p2.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinData_p3.h"
#include "MuonEventTPCnv/TgcCoinDataContainer_tlp1.h"
#include "MuonEventTPCnv/TgcCoinDataContainer_tlp2.h"
#include "MuonEventTPCnv/TgcCoinDataContainer_tlp3.h"
#include "MuonEventTPCnv/MuonPrepRawData/MdtTwinPrepData_p1.h"
#include "MuonEventTPCnv/MuonChamberT0s/ChamberT0s_p1.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonDigitContainer_p1.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonDigitContainer_p2.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCollection_p1.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCollection_p2.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCollection_p1.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCollection_p2.h"
#include "MuonEventTPCnv/MuonRDO/STGC_RawDataContainer_p1.h"

struct MuonEventTPCnvDict 
{
    std::vector< Muon::CscPrepData_p2>              m_v1;
    std::vector< Muon::CscStripPrepData_p1>         m_v2;
    std::vector< Muon::RpcPrepData_p3>              m_v3;
    std::vector< Muon::TgcPrepData_p2 >             m_v4;
    std::vector< Muon::TgcPrepData_p3 >             m_v4a;
    std::vector< Muon::MdtPrepData_p2 >             m_v5;
    std::vector< Muon::MuonPRD_Collection_p2 >      m_v6;
    std::vector< Muon::MdtPrepDataContainer_p2 >    m_v7a;
    std::vector< Muon::RpcPrepDataContainer_p3 >    m_v7b;
    std::vector< Muon::CscPrepDataContainer_p2 >    m_v7c;
    std::vector< Muon::TgcPrepDataContainer_p2 >    m_v7d;
    std::vector< Muon::TgcPrepDataContainer_p3 >    m_v7e;
    std::vector< Muon::MuonSegment_p1>              m_v8;    
    std::vector< Muon::MuonSegmentQuality_p1>       m_v9;
    std::vector< Muon::CompetingMuonClustersOnTrack_p1> m_v10;
    std::vector< Muon::MuonCoinDataCollection_p1 >      m_v11;
    std::vector< Muon::MuonCoinDataContainer_p1 >       m_v12;
    std::vector< Muon::TgcCoinData_p2>              m_v13a;
    std::vector< Muon::TgcCoinData_p3>              m_v13b;
    Muon::MuonPRD_Container_p2<Muon::MdtPrepData_p2>      m_v14;
    Muon::MuonPRD_Container_p2<Muon::RpcPrepData_p3>      m_v14a;
    Muon::MuonPRD_Container_p2<Muon::CscPrepData_p2>      m_v14b;
    Muon::MuonPRD_Container_p2<Muon::TgcPrepData_p2>      m_v14c;
    Muon::MuonPRD_Container_p2<Muon::TgcPrepData_p3>      m_v14d;
    std::vector<Muon::MuonPRD_Container_p2<Muon::MdtPrepData_p2> > m_v15;
    std::vector< Muon::MdtTwinPrepData_p1 >             m_v16;
    std::vector< Muon::RpcCoinData_p1 >             m_v17;
    Muon::MuonPRD_Container_p2<Muon::RpcCoinData_p1>             m_v18;
    std::pair< unsigned int, float> m_v19;
    Muon::MuonPRD_Container_p2<Muon::sTgcPrepData_p1>      m_v20;
    Muon::MuonPRD_Container_p2<Muon::MMPrepData_p1>      m_v21;
    Muon::MuonDigitContainer_p1<Muon::MM_Digit_p1>      m_v22;
    Muon::MuonDigitContainer_p1<Muon::STGC_Digit_p1>      m_v23;
    Muon::MuonDigitContainer_p2<Muon::MM_Digit_p2>      m_v24;
<<<<<<< HEAD
    Muon::STGC_RawDataContainer_p1      m_v25;
=======
>>>>>>> Move some stuff to Muon namespace because why on earth is it not done already, and minor cleanup
};

#endif // MUONEVENTTPCNV_MUONEVENTTPCNVDICT_H
