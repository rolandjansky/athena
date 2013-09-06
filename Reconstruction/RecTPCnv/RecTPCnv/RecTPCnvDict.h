///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// File: RecTPCnv/RecTPCnvDict.h
/// Dictionary file for persistent representation(s) of AOD containers
/// Author: Sebastien Binet <binet@cern.ch>
/// Date:   December 2006

#ifndef RECTPCNV_RECTPCNVCNVDICT_H
#define RECTPCNV_RECTPCNVCNVDICT_H

#include "RecTPCnv/CaloEnergy_p2.h"
#include "RecTPCnv/MuonCaloEnergyContainer_p1.h"
#include "RecTPCnv/MuonCaloEnergyContainerCnv_p1.h"
#include "RecTPCnv/MuonCaloEnergyContainer_tlp1.h"
#include "RecTPCnv/MuonCaloEnergyContainerCnv_tlp1.h"

#include "RecTPCnv/CaloEnergy_p1.h"

#include "RecTPCnv/DepositInCalo_p2.h"
#include "RecTPCnv/DepositInCalo_p1.h"

#include "RecTPCnv/Muon_p6.h"
#include "RecTPCnv/MuonContainer_p6.h"
#include "RecTPCnv/MuonContainerCnv_p6.h"

#include "RecTPCnv/Muon_p5.h"
#include "RecTPCnv/MuonContainer_p5.h"
#include "RecTPCnv/MuonContainerCnv_p5.h"

#include "RecTPCnv/Muon_p4.h"
#include "RecTPCnv/MuonContainer_p4.h"
#include "RecTPCnv/MuonContainerCnv_p4.h"

#include "RecTPCnv/Muon_p3.h"
#include "RecTPCnv/MuonContainer_p3.h"
#include "RecTPCnv/MuonContainerCnv_p3.h"

#include "RecTPCnv/Muon_p2.h"
#include "RecTPCnv/MuonContainer_p2.h"
#include "RecTPCnv/MuonContainerCnv_p2.h"

#include "RecTPCnv/Muon_p1.h"
#include "RecTPCnv/MuonContainer_p1.h"
#include "RecTPCnv/MuonContainerCnvARA_p1.h"

#include "RecTPCnv/MissingET_p1.h"
#include "RecTPCnv/MissingET_p2.h"
#include "RecTPCnv/MissingET_p3.h"
#include "RecTPCnv/MissingETCnv_p1.h"
#include "RecTPCnv/MissingETCnv_p2.h"
#include "RecTPCnv/MissingETCnv_p3.h"
#include "RecTPCnv/MissingEtCalo_p1.h"
#include "RecTPCnv/MissingEtCalo_p2.h"
#include "RecTPCnv/MissingEtCalo_p3.h"
#include "RecTPCnv/MissingEtCaloCnv_p1.h"
#include "RecTPCnv/MissingEtCaloCnv_p2.h"
#include "RecTPCnv/MissingEtCaloCnv_p3.h"
#include "RecTPCnv/MissingEtTruth_p1.h"
#include "RecTPCnv/MissingEtTruth_p2.h"
#include "RecTPCnv/MissingEtTruth_p3.h"
#include "RecTPCnv/MissingEtTruthCnv_p1.h"
#include "RecTPCnv/MissingEtTruthCnv_p2.h"
#include "RecTPCnv/MissingEtTruthCnv_p3.h"
#include "RecTPCnv/MissingEtRegions_p1.h"
#include "RecTPCnv/MissingEtRegions_p2.h"
#include "RecTPCnv/MissingEtRegions_p3.h"
#include "RecTPCnv/MissingETComposition_p1.h"
#include "RecTPCnv/MissingETCompositionCnv_p1.h"
#include "RecTPCnv/MissingETComposition_p2.h"
#include "RecTPCnv/MissingETCompositionCnv_p2.h"

#include "RecTPCnv/MuonSpShower_p1.h"
#include "RecTPCnv/MuonSpShowerCnv_p1.h"
#include "RecTPCnv/MuonSpShowerContainer_p1.h"
#include "RecTPCnv/MuonSpShowerContainerCnv_p1.h"

#include "RecEvent/RecoTimingObj.h"
#include "RecTPCnv/RecoTimingObj_p1.h"
#include "RecTPCnv/RecoTimingObjCnv_p1.h"

struct dummy {
  T_TPCnv<Analysis::MuonContainer, MuonContainer_p6> m_muocnv6;
  T_TPCnv<Analysis::MuonContainer, MuonContainer_p5> m_muocnv5;
  T_TPCnv<Analysis::MuonContainer, MuonContainer_p4> m_muocnv4;
  T_TPCnv<Analysis::MuonContainer, MuonContainer_p3> m_muocnv3;
  T_TPCnv<Analysis::MuonContainer, MuonContainer_p2> m_muocnv2;
  T_TPCnv<Analysis::MuonContainer, MuonContainer_p1> m_muocnv;

  T_TPCnv<MuonCaloEnergyContainer, MuonCaloEnergyContainer_p1> m_caloenergycnv;
  T_TPCnv<MuonCaloEnergyContainer, MuonCaloEnergyContainer_tlp1> m_caloenergytlpcnv;

  T_TPCnv<MissingEtCalo,MissingEtCalo_p1>  m_missingEtCaloCnv1;
  T_TPCnv<MissingEtCalo,MissingEtCalo_p2>  m_missingEtCaloCnv2;
  T_TPCnv<MissingEtCalo,MissingEtCalo_p3>  m_missingEtCaloCnv3;
  T_TPCnv<MissingET,MissingET_p1> m_missingEtCnv_1;
  T_TPCnv<MissingET,MissingET_p2> m_missingEtCnv_2;
  T_TPCnv<MissingET,MissingET_p3> m_missingEtCnv_3;
  T_TPCnv<MissingEtTruth,MissingEtTruth_p1> m_missingEtTruthCnv1;
  T_TPCnv<MissingEtTruth,MissingEtTruth_p2> m_missingEtTruthCnv2;
  T_TPCnv<MissingEtTruth,MissingEtTruth_p3> m_missingEtTruthCnv3;
  T_TPCnv<MissingETComposition,MissingETComposition_p1> m_missingEtCompCnv_1;
  T_TPCnv<MissingETComposition,MissingETComposition_p2> m_missingEtCompCnv_2;
  T_TPCnv<Rec::MuonSpShowerContainer, MuonSpShowerContainer_p1> m_muonSpShowerContCnv;

  T_TPCnv<RecoTimingObj, RecoTimingObj_p1> m_rectimobjCnv;
};

#endif //RECTPCNV_RECTPCNVCNVDICT_H
