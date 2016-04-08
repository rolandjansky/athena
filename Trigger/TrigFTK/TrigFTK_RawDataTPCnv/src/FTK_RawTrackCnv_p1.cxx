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
  //  log << MSG::INFO << "FTK_RawTrackCnv_p1::persToTrans called" << endreq;
  //  log << MSG::ATH << "FTK_RawTrackCnv_p1:: m_pars[th1] = " << persObj->m_pars[th1] << endreq;
  //  log << MSG::ATH << "FTK_RawTrackCnv_p1:: m_pars[th2] = " << persObj->m_pars[th2] << endreq;
  //  log << MSG::ATH << "FTK_RawTrackCnv_p1:: m_pars[th3] = " << persObj->m_pars[th3] << endreq;
  //  log << MSG::ATH << "FTK_RawTrackCnv_p1:: m_pars[th4] = " << persObj->m_pars[th4] << endreq;
  //  log << MSG::ATH << "FTK_RawTrackCnv_p1:: m_pars[th5] = " << persObj->m_pars[th5] << endreq;
  //  log << MSG::ATH << "FTK_RawTrackCnv_p1:: m_pars[th4] = " << persObj->m_barcode << endreq;

  transObj->setTH1(persObj->m_pars[th1]);
  transObj->setTH2(persObj->m_pars[th2]);
  transObj->setTH3(persObj->m_pars[th3]);
  transObj->setTH4(persObj->m_pars[th4]);
  transObj->setTH5(persObj->m_pars[th5]);
  transObj->setTH6(persObj->m_pars[th6]);
  transObj->setBarcode(persObj->m_barcode);

  // Pixel Clusters //
  FTK_RawPixelCluster pixel_1(0), pixel_2(1), pixel_3(2), pixel_4(3);
  /*FTK_RawPixelCluster* pixel_1 = new FTK_RawPixelCluster(0);
  FTK_RawPixelCluster* pixel_2 = new FTK_RawPixelCluster(1);
  FTK_RawPixelCluster* pixel_3 = new FTK_RawPixelCluster(2);
  FTK_RawPixelCluster* pixel_4 = new FTK_RawPixelCluster(3);

  pixel_1->setWordA(persObj->m_pars_pix[0]);
  pixel_1->setWordB(persObj->m_pars_pix[1]);
  pixel_1->setBarcode(persObj->m_barcodes_pix[0]);
  //  log << MSG::INFO << persObj->m_barcodes_pix[0] << endreq;

  pixel_2->setWordA(persObj->m_pars_pix[2]);
  pixel_2->setWordB(persObj->m_pars_pix[3]);
  pixel_2->setBarcode(persObj->m_barcodes_pix[1]);

  pixel_3->setWordA(persObj->m_pars_pix[4]);
  pixel_3->setWordB(persObj->m_pars_pix[5]);
  pixel_3->setBarcode(persObj->m_barcodes_pix[2]);

  pixel_4->setWordA(persObj->m_pars_pix[6]);
  pixel_4->setWordB(persObj->m_pars_pix[7]);
  pixel_3->setBarcode(persObj->m_barcodes_pix[3]);

  std::vector<FTK_RawPixelCluster*> pixel_clusters;
  pixel_clusters.push_back(pixel_1);
  pixel_clusters.push_back(pixel_2);
  pixel_clusters.push_back(pixel_3);
  pixel_clusters.push_back(pixel_4);
  transObj->setPixelClusters(pixel_clusters);*/

  pixel_1.setWordA(persObj->m_pars_pix[0]);
  pixel_1.setWordB(persObj->m_pars_pix[1]);
  pixel_1.setBarcode(persObj->m_barcodes_pix[0]);

  pixel_2.setWordA(persObj->m_pars_pix[2]);
  pixel_2.setWordB(persObj->m_pars_pix[3]);
  pixel_2.setBarcode(persObj->m_barcodes_pix[1]);

  pixel_3.setWordA(persObj->m_pars_pix[4]);
  pixel_3.setWordB(persObj->m_pars_pix[5]);
  pixel_3.setBarcode(persObj->m_barcodes_pix[2]);

  pixel_4.setWordA(persObj->m_pars_pix[6]);
  pixel_4.setWordB(persObj->m_pars_pix[7]);
  pixel_4.setBarcode(persObj->m_barcodes_pix[3]);

  std::vector<FTK_RawPixelCluster> pixel_clusters;

  pixel_clusters.push_back(pixel_1);
  pixel_clusters.push_back(pixel_2);
  pixel_clusters.push_back(pixel_3);
  pixel_clusters.push_back(pixel_4);

  transObj->setPixelClusters(pixel_clusters);

  // SCT Clusters //
  FTK_RawSCT_Cluster sct_1(4), sct_2(5), sct_3(6), sct_4(7), sct_5(8), sct_6(9), sct_7(10), sct_8(11);

  sct_1.setWord(    persObj->m_pars_sct[0]);
  sct_1.setBarcode( persObj->m_barcodes_sct[0]);

  sct_2.setWord(    persObj->m_pars_sct[1]);
  sct_2.setBarcode( persObj->m_barcodes_sct[1]);

  sct_3.setWord(    persObj->m_pars_sct[2]);
  sct_3.setBarcode( persObj->m_barcodes_sct[2]);

  sct_4.setWord(    persObj->m_pars_sct[3]);
  sct_4.setBarcode( persObj->m_barcodes_sct[3]);

  sct_5.setWord(    persObj->m_pars_sct[4]);
  sct_5.setBarcode( persObj->m_barcodes_sct[4]);

  sct_6.setWord(    persObj->m_pars_sct[5]);
  sct_6.setBarcode( persObj->m_barcodes_sct[5]);

  sct_7.setWord(    persObj->m_pars_sct[6]);
  sct_7.setBarcode( persObj->m_barcodes_sct[6]);

  sct_8.setWord(    persObj->m_pars_sct[7]);
  sct_8.setBarcode( persObj->m_barcodes_sct[7]);

  std::vector<FTK_RawSCT_Cluster> sct_clusters;
  sct_clusters.push_back(sct_1);
  sct_clusters.push_back(sct_2);
  sct_clusters.push_back(sct_3);
  sct_clusters.push_back(sct_4);
  sct_clusters.push_back(sct_5);
  sct_clusters.push_back(sct_6);
  sct_clusters.push_back(sct_7);
  sct_clusters.push_back(sct_8);
  transObj->setSCTClusters(sct_clusters);
}

void FTK_RawTrackCnv_p1::transToPers(const FTK_RawTrack* transObj,
				     FTK_RawTrack_p1* persObj,
				     MsgStream &log)
{
  log << MSG::DEBUG << "FTK_RawTrackCnv_p1::transToPers called" << endreq;

  persObj->m_pars[th1] = transObj->getTH1();
  persObj->m_pars[th2] = transObj->getTH2();
  persObj->m_pars[th3] = transObj->getTH3();
  persObj->m_pars[th4] = transObj->getTH4();
  persObj->m_pars[th5] = transObj->getTH5();
  persObj->m_pars[th6] = transObj->getTH6();
  persObj->m_barcode   = transObj->getBarcode();

  // Pixel Clusters //
  /*  persObj->m_pars_pix[0]     = transObj->getPixelCluster(0).getWordA();
  persObj->m_pars_pix[1]     = transObj->getPixelCluster(0).getWordB();
  persObj->m_barcodes_pix[0] = transObj->getPixelCluster(0).getBarcode();

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
  persObj->m_barcodes_sct[7] = transObj->getSCTCluster(7).getBarcode();*/

  // Pixel Clusters //
  //  persObj->m_pars_pix[0]     = transObj->getPixelCluster(0).getWordA();
  //  persObj->m_pars_pix[1]     = transObj->getPixelCluster(0).getWordB();
  //  persObj->m_barcodes_pix[0] = transObj->getPixelCluster(0).getBarcode();
  persObj->m_pars_pix[0]     = transObj->getPixelWordA(0);
  persObj->m_pars_pix[1]     = transObj->getPixelWordB(0);
  persObj->m_barcodes_pix[0] = transObj->getPixelBarcode(0);
  //  log << MSG::INFO << "FTK_RawTrackCnv_p1::pixel barcode" << transObj->getPixelCluster(0)->getBarcode() << endreq;
  //  log << MSG::INFO << "FTK_RawTrackCnv_p1::pixel barcode" << persObj->m_barcodes_pix[0] << endreq;
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


