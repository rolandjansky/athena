/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////
// Implementation of the TP converter
//////////////////////////////////////////////////////

#include "tauEvent/TauShot.h"
#include "tauEventTPCnv/TauShotCnv_p1.h"

void TauShotCnv_p1::persToTrans(const TauShot_p1 *persObj, Analysis::TauShot *transObj, MsgStream &log)
{
  /// links to CaloClusters
  //m_clusterVectCnv.persToTrans( &persObj->m_CaloClusterVector, &transObj->m_CaloClusterVector, log );
  m_clusterCnv.persToTrans( &persObj->m_cluster, &transObj->clusterLink(), log );
  m_seedCellCnv.persToTrans( &persObj->m_seedCell, &transObj->seedCellLink(), log );

  // 4momentum
  P4EEtaPhiM mom;
  m_P4EEtaPhiMCnv.persToTrans(&persObj->m_P4EEtaPhiM,&mom,log);
  transObj->set4Mom (mom);
  /// simple standard member data
  // vectors
  //transObj->m_clusterValueVector1 = persObj->m_clusterValueVector1;
  // floats or ints
  transObj->setNCellsInEta (persObj->m_nCellsInEta);
  transObj->setpt1 (persObj->m_pt1);
  transObj->setpt3 (persObj->m_pt3);
  transObj->setpt5 (persObj->m_pt5);
  transObj->setws5 (persObj->m_ws5);
  transObj->setsdevEta5_WRTmean (persObj->m_sdevEta5_WRTmean);
  transObj->setsdevEta5_WRTmode (persObj->m_sdevEta5_WRTmode);
  transObj->setsdevPt5 (persObj->m_sdevPt5);
  transObj->setdeltaPt12_min (persObj->m_deltaPt12_min);
  transObj->setFside_3not1 (persObj->m_Fside_3not1);
  transObj->setFside_5not1 (persObj->m_Fside_5not1);
  transObj->setFside_5not3 (persObj->m_Fside_5not3);
  transObj->setfracSide_3not1 (persObj->m_fracSide_3not1);
  transObj->setfracSide_5not1 (persObj->m_fracSide_5not1);
  transObj->setfracSide_5not3 (persObj->m_fracSide_5not3);
  transObj->setpt1OverPt3 (persObj->m_pt1OverPt3);
  transObj->setpt3OverPt5 (persObj->m_pt3OverPt5);
  transObj->setmergedScore (persObj->m_mergedScore);
  transObj->setsignalScore (persObj->m_signalScore);
  transObj->setNPhotons (persObj->m_nPhotons);
}

void TauShotCnv_p1::transToPers(const Analysis::TauShot *transObj, TauShot_p1 *persObj, MsgStream &log)
{
  /// links to CaloClusters
  //m_clusterVectCnv.transToPers( &transObj->m_CaloClusterVector, &persObj->m_CaloClusterVector, log );
  m_clusterCnv.transToPers( &transObj->clusterLink(), &persObj->m_cluster, log );
  m_seedCellCnv.transToPers( &transObj->seedCellLink(), &persObj->m_seedCell, log );

  // 4momentum
  m_P4EEtaPhiMCnv.transToPers((P4EEtaPhiM*)transObj,&persObj->m_P4EEtaPhiM,log); 
  /// simple standard member data
  // vector
  //persObj->m_clusterValueVector1 = transObj->m_clusterValueVector1;
  // floats or ints
  persObj->m_nCellsInEta = transObj->nCellsInEta();
  persObj->m_pt1 = transObj->pt1();
  persObj->m_pt3 = transObj->pt3();
  persObj->m_pt5 = transObj->pt5();
  persObj->m_ws5 = transObj->ws5();
  persObj->m_sdevEta5_WRTmean = transObj->sdevEta5_WRTmean();
  persObj->m_sdevEta5_WRTmode = transObj->sdevEta5_WRTmode();
  persObj->m_sdevPt5 = transObj->sdevPt5();
  persObj->m_deltaPt12_min = transObj->deltaPt12_min();
  persObj->m_Fside_3not1 = transObj->Fside_3not1();
  persObj->m_Fside_5not1 = transObj->Fside_5not1();
  persObj->m_Fside_5not3 = transObj->Fside_5not3();
  persObj->m_fracSide_3not1 = transObj->fracSide_3not1();
  persObj->m_fracSide_5not1 = transObj->fracSide_5not1();
  persObj->m_fracSide_5not3 = transObj->fracSide_5not3();
  persObj->m_pt1OverPt3 = transObj->pt1OverPt3();
  persObj->m_pt3OverPt5 = transObj->pt3OverPt5();
  persObj->m_mergedScore = transObj->mergedScore();
  persObj->m_signalScore = transObj->signalScore();
  persObj->m_nPhotons = transObj->nPhotons();
}


