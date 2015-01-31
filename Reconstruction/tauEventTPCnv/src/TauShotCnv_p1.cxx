/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////
// Implementation of the TP converter
//////////////////////////////////////////////////////

//trick to access private members in TauShot
//TODO: should be avoided
//see: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/TransientPersistentSeparation#Avoiding_the_define_private_publ
#define private public
#define protected public
#include "tauEvent/TauShot.h"
#undef private
#undef protected
#include "tauEventTPCnv/TauShotCnv_p1.h"

void TauShotCnv_p1::persToTrans(const TauShot_p1 *persObj, Analysis::TauShot *transObj, MsgStream &log)
{
  /// links to CaloClusters
  //m_clusterVectCnv.persToTrans( &persObj->m_CaloClusterVector, &transObj->m_CaloClusterVector, log );
  m_clusterCnv.persToTrans( &persObj->m_cluster, &transObj->m_cluster, log );
  m_seedCellCnv.persToTrans( &persObj->m_seedCell, &transObj->m_seedCell, log );

  // 4momentum
  m_P4EEtaPhiMCnv.persToTrans(&persObj->m_P4EEtaPhiM,(P4EEtaPhiM*)transObj,log);
  /// simple standard member data
  // vectors
  //transObj->m_clusterValueVector1 = persObj->m_clusterValueVector1;
  // floats or ints
  transObj->m_nCellsInEta = persObj->m_nCellsInEta;
  transObj->m_pt1 = persObj->m_pt1;
  transObj->m_pt3 = persObj->m_pt3;
  transObj->m_pt5 = persObj->m_pt5;
  transObj->m_ws5 = persObj->m_ws5;
  transObj->m_sdevEta5_WRTmean = persObj->m_sdevEta5_WRTmean;
  transObj->m_sdevEta5_WRTmode = persObj->m_sdevEta5_WRTmode;
  transObj->m_sdevPt5 = persObj->m_sdevPt5;
  transObj->m_deltaPt12_min = persObj->m_deltaPt12_min;
  transObj->m_Fside_3not1 = persObj->m_Fside_3not1;
  transObj->m_Fside_5not1 = persObj->m_Fside_5not1;
  transObj->m_Fside_5not3 = persObj->m_Fside_5not3;
  transObj->m_fracSide_3not1 = persObj->m_fracSide_3not1;
  transObj->m_fracSide_5not1 = persObj->m_fracSide_5not1;
  transObj->m_fracSide_5not3 = persObj->m_fracSide_5not3;
  transObj->m_pt1OverPt3 = persObj->m_pt1OverPt3;
  transObj->m_pt3OverPt5 = persObj->m_pt3OverPt5;
  transObj->m_mergedScore = persObj->m_mergedScore;
  transObj->m_signalScore = persObj->m_signalScore;
  transObj->m_nPhotons = persObj->m_nPhotons;
}

void TauShotCnv_p1::transToPers(const Analysis::TauShot *transObj, TauShot_p1 *persObj, MsgStream &log)
{
  /// links to CaloClusters
  //m_clusterVectCnv.transToPers( &transObj->m_CaloClusterVector, &persObj->m_CaloClusterVector, log );
  m_clusterCnv.transToPers( &transObj->m_cluster, &persObj->m_cluster, log );
  m_seedCellCnv.transToPers( &transObj->m_seedCell, &persObj->m_seedCell, log );

  // 4momentum
  m_P4EEtaPhiMCnv.transToPers((P4EEtaPhiM*)transObj,&persObj->m_P4EEtaPhiM,log); 
  /// simple standard member data
  // vector
  //persObj->m_clusterValueVector1 = transObj->m_clusterValueVector1;
  // floats or ints
  persObj->m_nCellsInEta = transObj->m_nCellsInEta;
  persObj->m_pt1 = transObj->m_pt1;
  persObj->m_pt3 = transObj->m_pt3;
  persObj->m_pt5 = transObj->m_pt5;
  persObj->m_ws5 = transObj->m_ws5;
  persObj->m_sdevEta5_WRTmean = transObj->m_sdevEta5_WRTmean;
  persObj->m_sdevEta5_WRTmode = transObj->m_sdevEta5_WRTmode;
  persObj->m_sdevPt5 = transObj->m_sdevPt5;
  persObj->m_deltaPt12_min = transObj->m_deltaPt12_min;
  persObj->m_Fside_3not1 = transObj->m_Fside_3not1;
  persObj->m_Fside_5not1 = transObj->m_Fside_5not1;
  persObj->m_Fside_5not3 = transObj->m_Fside_5not3;
  persObj->m_fracSide_3not1 = transObj->m_fracSide_3not1;
  persObj->m_fracSide_5not1 = transObj->m_fracSide_5not1;
  persObj->m_fracSide_5not3 = transObj->m_fracSide_5not3;
  persObj->m_pt1OverPt3 = transObj->m_pt1OverPt3;
  persObj->m_pt3OverPt5 = transObj->m_pt3OverPt5;
  persObj->m_mergedScore = transObj->m_mergedScore;
  persObj->m_signalScore = transObj->m_signalScore;
  persObj->m_nPhotons = transObj->m_nPhotons;
}


