/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigFTK_RawDataTPCnv/FTK_RawTrackCnv_p1.h"
#include "TrigFTK_RawDataTPCnv/FTK_RawTrack_p1.h"

void FTK_RawTrackCnv_p1::persToTrans(const FTK_RawTrack_p1* persObj,
				     FTK_RawTrack* transObj,
				     MsgStream &/*log*/)
{

  transObj->setTH1(persObj->m_pars[th1]);
  transObj->setTH2(persObj->m_pars[th2]);
  transObj->setTH3(persObj->m_pars[th3]);
  transObj->setTH4(persObj->m_pars[th4]);
  transObj->setTH5(persObj->m_pars[th5]);
  transObj->setTH6(persObj->m_pars[th6]);
  transObj->setBarcode(persObj->m_barcode);

  // Pixel Clusters //
  std::vector<FTK_RawPixelCluster> pix_clusters;
  unsigned int iword=0;
  for (unsigned int ipix=0; ipix<4; ipix++) {
    FTK_RawPixelCluster pix_clus(persObj->m_pars_pix[iword], persObj->m_pars_pix[iword+1],persObj->m_barcodes_pix[ipix]);
    iword+=2;
    pix_clusters.push_back(pix_clus);
  }
  transObj->setPixelClusters(pix_clusters);

  std::vector<FTK_RawSCT_Cluster> sct_clusters;

  for (unsigned int isct=0; isct<8; isct++) {
    FTK_RawSCT_Cluster sct_clus(persObj->m_pars_sct[isct], persObj->m_barcodes_sct[isct]);
    sct_clusters.push_back(sct_clus);
  }
  transObj->setSCTClusters(sct_clusters);
}

void FTK_RawTrackCnv_p1::transToPers(const FTK_RawTrack* transObj,
				     FTK_RawTrack_p1* persObj,
				     MsgStream &log)
{
  log << MSG::DEBUG << "FTK_RawTrackCnv_p1::transToPers called" << endmsg;

  persObj->m_pars[th1] = transObj->getTH1();
  persObj->m_pars[th2] = transObj->getTH2();
  persObj->m_pars[th3] = transObj->getTH3();
  persObj->m_pars[th4] = transObj->getTH4();
  persObj->m_pars[th5] = transObj->getTH5();
  persObj->m_pars[th6] = transObj->getTH6();
  persObj->m_barcode   = transObj->getBarcode();

  //Pixel Clusters
  persObj->m_pars_pix[0]     = transObj->getPixelWordA(0);
  persObj->m_pars_pix[1]     = transObj->getPixelWordB(0);
  persObj->m_barcodes_pix[0] = transObj->getPixelBarcode(0);


  persObj->m_pars_pix[2]     = transObj->getPixelCluster(1).getWordA();
  persObj->m_pars_pix[3]     = transObj->getPixelCluster(1).getWordB();
  persObj->m_barcodes_pix[1] = transObj->getPixelCluster(1).getBarcode();

  persObj->m_pars_pix[4]     = transObj->getPixelCluster(2).getWordA();
  persObj->m_pars_pix[5]     = transObj->getPixelCluster(2).getWordB();
  persObj->m_barcodes_pix[2] = transObj->getPixelCluster(2).getBarcode();

  persObj->m_pars_pix[6]     = transObj->getPixelCluster(3).getWordA();
  persObj->m_pars_pix[7]     = transObj->getPixelCluster(3).getWordB();
  persObj->m_barcodes_pix[3] = transObj->getPixelCluster(3).getBarcode();

  // SCT Clusters //
  persObj->m_pars_sct[0]     = transObj->getSCTCluster(0).getWord();
  persObj->m_barcodes_sct[0] = transObj->getSCTCluster(0).getBarcode();
  
  persObj->m_pars_sct[1]     = transObj->getSCTCluster(1).getWord();
  persObj->m_barcodes_sct[1] = transObj->getSCTCluster(1).getBarcode();

  persObj->m_pars_sct[2]     = transObj->getSCTCluster(2).getWord();
  persObj->m_barcodes_sct[2] = transObj->getSCTCluster(2).getBarcode();

  persObj->m_pars_sct[3]     = transObj->getSCTCluster(3).getWord();
  persObj->m_barcodes_sct[3] = transObj->getSCTCluster(3).getBarcode();

  persObj->m_pars_sct[4]     = transObj->getSCTCluster(4).getWord();
  persObj->m_barcodes_sct[4] = transObj->getSCTCluster(4).getBarcode();

  persObj->m_pars_sct[5]     = transObj->getSCTCluster(5).getWord();
  persObj->m_barcodes_sct[5] = transObj->getSCTCluster(5).getBarcode();

  persObj->m_pars_sct[6]     = transObj->getSCTCluster(6).getWord();
  persObj->m_barcodes_sct[6] = transObj->getSCTCluster(6).getBarcode();

  persObj->m_pars_sct[7]     = transObj->getSCTCluster(7).getWord();
  persObj->m_barcodes_sct[7] = transObj->getSCTCluster(7).getBarcode();
}


