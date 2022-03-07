/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXJetAlgo - JetFinder algorithm for gFEX
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#include <vector>

#include "L1CaloFEXSim/gFEXJetAlgo.h"
#include "L1CaloFEXSim/gFEXJetTOB.h"
#include "L1CaloFEXSim/gTowerContainer.h"
#include "L1CaloFEXSim/gTower.h"

namespace LVL1 {

  // default constructor for persistency
gFEXJetAlgo::gFEXJetAlgo(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent)
  {
    declareInterface<IgFEXJetAlgo>(this);
  }


StatusCode gFEXJetAlgo::initialize(){

  ATH_CHECK(m_gFEXJetAlgo_gTowerContainerKey.initialize());

  return StatusCode::SUCCESS;

}



std::vector<std::unique_ptr<gFEXJetTOB>> gFEXJetAlgo::largeRfinder(
                               gTowersCentral Atwr, gTowersCentral Btwr,
                               gTowersForward CNtwr, gTowersForward CPtwr,
                               int pucA, int pucB, int gLJ_seedThrA, int gLJ_seedThrB, 
                               int gJ_ptMinToTopoCounts1, int gJ_ptMinToTopoCounts2,
                               int jetThreshold, int gLJ_ptMinToTopoCounts1, int gLJ_ptMinToTopoCounts2,
                               std::array<uint32_t, 7> & ATOB1_dat, std::array<uint32_t, 7> & ATOB2_dat,
                               std::array<uint32_t, 7> & BTOB1_dat, std::array<uint32_t, 7> & BTOB2_dat) {

  // Arrays for gJets
  // s = status (1 if above threshold)
  // v = value 200 MeV LSB
  // eta and phi are bin numbers
  std::array<int, FEXAlgoSpaceDefs::gJetTOBfib> gJetTOBs   = {{0,0,0,0}};
  std::array<int, FEXAlgoSpaceDefs::gJetTOBfib> gJetTOBv   = {{0,0,0,0}};
  std::array<int, FEXAlgoSpaceDefs::gJetTOBfib> gJetTOBeta = {{0,0,0,0}};
  std::array<int, FEXAlgoSpaceDefs::gJetTOBfib> gJetTOBphi = {{0,0,0,0}};

  // Arrays for gBlocks  (so far only 2 gBlocks per fiber)
  // first index is for the fiber number (A 0 and 1, B 0 and 1, C P and C N )
  // second index is for leading, sub-leading, and sub-sub-leading (not used)
  std::array<std::array<int, 3>, FEXAlgoSpaceDefs::BTOBFIB> gBlockTOBv   = {{ {{0,0,0}}, {{0,0,0}}, {{0,0,0}}, {{0,0,0}}, {{0,0,0}}, {{0,0,0}} }};
  std::array<std::array<int, 3>, FEXAlgoSpaceDefs::BTOBFIB> gBlockTOBeta = {{ {{0,0,0}}, {{0,0,0}}, {{0,0,0}}, {{0,0,0}}, {{0,0,0}}, {{0,0,0}} }};
  std::array<std::array<int, 3>, FEXAlgoSpaceDefs::BTOBFIB> gBlockTOBphi = {{ {{0,0,0}}, {{0,0,0}}, {{0,0,0}}, {{0,0,0}}, {{0,0,0}}, {{0,0,0}} }};


  // calculate A & B  remote partial sums first
  gTowersPartialSums RAlps_out, RArps_out;
  RemotePartialAB(Atwr, RAlps_out, RArps_out);

  gTowersPartialSums RBlps_out, RBrps_out;
  RemotePartialAB(Btwr, RBlps_out, RBrps_out);

  gTowersPartialSums RCNrps_out;
  RemotePartialCN(CNtwr,  RCNrps_out);
  gTowersPartialSums RCPlps_out;
  RemotePartialCP(CPtwr,  RCPlps_out);


  // find all possible jets within the FPGA (32 x 12 for FPGAs A and B )

  gTowersCentral AjetsRestricted;
  singleAB(Atwr, AjetsRestricted);
  
  gTowersCentral BjetsRestricted;
  singleAB(Btwr, BjetsRestricted);


  // find gBlocks
  gTowersCentral gBLKA;
  gBlockAB(Atwr, gBLKA);


  gTowersCentral gBLKB;
  gBlockAB(Btwr, gBLKB);

  // sorting by jet engine -- not done in FPGA
  std::array<int, 32> AgBlockOutL;
  std::array<int, 32> AgBlockEtaIndL;
  std::array<int, 32> AgBlockOutR;
  std::array<int, 32> AgBlockEtaIndR;
  blkOutAB( gBLKA, AgBlockOutL,AgBlockEtaIndL, AgBlockOutR, AgBlockEtaIndR);


  std::array<int, 32> BgBlockOutL;
  std::array<int, 32> BgBlockEtaIndL;
  std::array<int, 32> BgBlockOutR;
  std::array<int, 32> BgBlockEtaIndR;
  blkOutAB( gBLKB, BgBlockOutL,BgBlockEtaIndL, BgBlockOutR, BgBlockEtaIndR);


  // sorting by 192 blocks 0 = left, 1 = right

  // find the leading and subleading gBlock  in the left column of FPGA A
  gBlockMax2(gBLKA, 1, 0, gBlockTOBv[0], gBlockTOBeta[0], gBlockTOBphi[0]);
   // find the leading and subleading gBlock  in the right column of FPGA A
  gBlockMax2(gBLKA, 2, 1, gBlockTOBv[1], gBlockTOBeta[1], gBlockTOBphi[1]);

  // find the leading and subleading gBlock  in the left column of FPGA B
  gBlockMax2(gBLKB, 3, 0, gBlockTOBv[2], gBlockTOBeta[2], gBlockTOBphi[2]);
  // find the leading and subleading gBlock  in the right column of FPGA B
  gBlockMax2(gBLKB, 4, 1, gBlockTOBv[3], gBlockTOBeta[3], gBlockTOBphi[3]);


  if( FEXAlgoSpaceDefs::ENABLE_INTER_AB ) {
    // input partial sums from FPGA B to A (lps_out -> rps_in)
    addRemoteRin(AjetsRestricted, RBlps_out);

    // input partial sums from FPGA B to A (lps_out -> rps_in)
    addRemoteLin(BjetsRestricted, RArps_out);

  }

  if( FEXAlgoSpaceDefs::ENABLE_INTER_C ) {
    addRemoteLin(AjetsRestricted, RCNrps_out);

    addRemoteRin(BjetsRestricted, RCPlps_out);

  }

  // Apply pileup threshold
  pileUpCorrectionAB(AjetsRestricted,pucA);
  pileUpCorrectionAB(BjetsRestricted,pucB);


  //Emulate switch to unsigned values by zeroing everything below the jet threshold
  //https://gitlab.cern.ch/atlas-l1calo/gfex/firmware/-/blob/devel/common/jet_finder/HDL/jet_eng.vhd#L550
 
  gJetVetoAB(AjetsRestricted,  jetThreshold);
  gJetVetoAB(BjetsRestricted,  jetThreshold);
  // gJetVetoAB(CCjetsRestricted, jetThreshold);

  // Apply gBlock trheshold to jet array

  gBlockVetoAB(AjetsRestricted, gBLKA, gLJ_seedThrA);
  gBlockVetoAB(BjetsRestricted, gBLKB, gLJ_seedThrB);

  std::array<int, 32> AjetOutL;
  std::array<int, 32> AetaIndL;
  std::array<int, 32> AjetOutR;
  std::array<int, 32> AetaIndR;


  jetOutAB(AjetsRestricted, gBLKA, gLJ_seedThrA, AjetOutL, AetaIndL, AjetOutR, AetaIndR);

  std::array<int, 32> BjetOutL;
  std::array<int, 32> BetaIndL;
  std::array<int, 32> BjetOutR;
  std::array<int, 32> BetaIndR;

  jetOutAB(BjetsRestricted, gBLKB, gLJ_seedThrB,  BjetOutL, BetaIndL, BjetOutR, BetaIndR);

  gJetTOBgen(AjetOutL, AetaIndL, 0, jetThreshold, gJetTOBs, gJetTOBv, gJetTOBeta, gJetTOBphi);
  gJetTOBgen(AjetOutR, AetaIndR, 1, jetThreshold, gJetTOBs, gJetTOBv, gJetTOBeta, gJetTOBphi);

  gJetTOBgen(BjetOutL, BetaIndL, 2, jetThreshold,  gJetTOBs, gJetTOBv, gJetTOBeta, gJetTOBphi);
  gJetTOBgen(BjetOutR, BetaIndR, 3, jetThreshold,  gJetTOBs, gJetTOBv, gJetTOBeta, gJetTOBphi);

  ///Define a vector to be filled with all the TOBs of one event
  std::vector<std::unique_ptr<gFEXJetTOB>> tobs_v;
  tobs_v.resize(14);

  // fill in TOBs
  ATOB1_dat[0]  = 0;
  BTOB1_dat[0]  = 0;
  //
  ATOB2_dat[0]  =  (( pucA & 0x00000FFF ) << 8);
  BTOB2_dat[0]  =  (( pucB & 0x00000FFF ) << 8);

  // //First available TOBs are the gRho for each central FPGA
  tobs_v[0] = std::make_unique<gFEXJetTOB>();
  tobs_v[0]->setWord(ATOB2_dat[0]);
  tobs_v[0]->setET(pucA);
  tobs_v[0]->setEta(0);
  tobs_v[0]->setPhi(0);
  tobs_v[0]->setTobID(0);
  tobs_v[0]->setStatus(1);//is 1 only if rho calculation is valid, but rho is not calculated at the moment

  tobs_v[1] = std::make_unique<gFEXJetTOB>();
  tobs_v[1]->setWord(BTOB2_dat[0]);
  tobs_v[1]->setET(pucB);
  tobs_v[1]->setEta(0);
  tobs_v[1]->setPhi(0);
  tobs_v[1]->setTobID(0);
  tobs_v[1]->setStatus(1);//is 1 only if rho calculation is valid, but rho is not calculated at the moment
  
  // leading gBlocks  (available first and go in first TOB value)
  // TOBs 2-5 are leading gBlocks
  ATOB1_dat[1] =  0x00000001; //set the TOB ID in the corresponding slot (LSB)
  if(  gBlockTOBv[0][0] > gJ_ptMinToTopoCounts2 )  ATOB1_dat[1] = ATOB1_dat[1] | 0x00000080;//status
  ATOB1_dat[1] =  ATOB1_dat[1] | ( ( gBlockTOBv[0][0]   & 0x00000FFF ) << 8);
  ATOB1_dat[1] =  ATOB1_dat[1] | ( ( gBlockTOBeta[0][0] & 0x0000003F ) <<20);
  ATOB1_dat[1] =  ATOB1_dat[1] | ( ( gBlockTOBphi[0][0] & 0x0000001F ) <<26);

  tobs_v[2] = std::make_unique<gFEXJetTOB>();
  tobs_v[2]->setWord(ATOB1_dat[1]);
  tobs_v[2]->setET(gBlockTOBv[0][0]);
  tobs_v[2]->setEta(gBlockTOBeta[0][0]);
  tobs_v[2]->setPhi(gBlockTOBphi[0][0]);
  tobs_v[2]->setTobID(1);
  if(  gBlockTOBv[0][0] > gJ_ptMinToTopoCounts2 ) tobs_v[2]->setStatus(1);
  else tobs_v[2]->setStatus(0);

  ATOB2_dat[1] =  0x00000002;
  if(  gBlockTOBv[1][0] > gJ_ptMinToTopoCounts1 ) ATOB2_dat[1] =  ATOB2_dat[1] | 0x00000080;
  ATOB2_dat[1] =  ATOB2_dat[1] | ( ( gBlockTOBv[1][0]   & 0x00000FFF ) << 8);
  ATOB2_dat[1] =  ATOB2_dat[1] | ( ( gBlockTOBeta[1][0] & 0x0000003F ) <<20);
  ATOB2_dat[1] =  ATOB2_dat[1] | ( ( gBlockTOBphi[1][0] & 0x0000001F ) <<26);

  tobs_v[3] = std::make_unique<gFEXJetTOB>();
  tobs_v[3]->setWord(ATOB2_dat[1]);
  tobs_v[3]->setET(gBlockTOBv[1][0]);
  tobs_v[3]->setEta(gBlockTOBeta[1][0]);
  tobs_v[3]->setPhi(gBlockTOBphi[1][0]);
  tobs_v[3]->setTobID(2);
  if(  gBlockTOBv[1][0] > gJ_ptMinToTopoCounts1 ) tobs_v[3]->setStatus(1);
  else tobs_v[3]->setStatus(0);


  BTOB1_dat[1] =  0x00000001;
  if(  gBlockTOBv[2][0] > gJ_ptMinToTopoCounts1 ) BTOB1_dat[1] = BTOB1_dat[1] | 0x00000080;
  BTOB1_dat[1] =  BTOB1_dat[1] | ( ( gBlockTOBv[2][0]   & 0x00000FFF ) << 8);
  BTOB1_dat[1] =  BTOB1_dat[1] | ( ( gBlockTOBeta[2][0] & 0x0000003F ) <<20);
  BTOB1_dat[1] =  BTOB1_dat[1] | ( ( gBlockTOBphi[2][0] & 0x0000001F) <<26);

  tobs_v[4] = std::make_unique<gFEXJetTOB>();
  tobs_v[4]->setWord(BTOB1_dat[1]);
  tobs_v[4]->setET(gBlockTOBv[2][0]);
  tobs_v[4]->setEta(gBlockTOBeta[2][0]);
  tobs_v[4]->setPhi(gBlockTOBphi[2][0]);
  tobs_v[4]->setTobID(1);
  if(  gBlockTOBv[2][0] > gJ_ptMinToTopoCounts1 ) tobs_v[4]->setStatus(1);
  else tobs_v[4]->setStatus(0);


  BTOB2_dat[1] =  0x00000002;
  if(  gBlockTOBv[3][0] > gJ_ptMinToTopoCounts2 ) BTOB2_dat[1] = BTOB2_dat[1] | 0x00000080;
  BTOB2_dat[1] =  BTOB2_dat[1] | ( ( gBlockTOBv[3][0]   & 0x00000FFF ) << 8);
  BTOB2_dat[1] =  BTOB2_dat[1] | ( ( gBlockTOBeta[3][0] & 0x0000003F ) <<20);
  BTOB2_dat[1] =  BTOB2_dat[1] | ( ( gBlockTOBphi[3][0] & 0x0000001F ) <<26);

  tobs_v[5] = std::make_unique<gFEXJetTOB>();
  tobs_v[5]->setWord(BTOB2_dat[1]);
  tobs_v[5]->setET(gBlockTOBv[3][0]);
  tobs_v[5]->setEta(gBlockTOBeta[3][0]);
  tobs_v[5]->setPhi(gBlockTOBphi[3][0]);
  tobs_v[5]->setTobID(2);
  if(  gBlockTOBv[3][0] > gJ_ptMinToTopoCounts2 ) tobs_v[5]->setStatus(1);
  else tobs_v[5]->setStatus(0);


  // subleading gBlocks
  // TOBs 6-9 are subleading gBlocks
  ATOB1_dat[2] =  0x00000003;
  if(  gBlockTOBv[0][1] > gJ_ptMinToTopoCounts2 ) ATOB1_dat[2] = ATOB1_dat[2] | 0x00000080;
  ATOB1_dat[2] =  ATOB1_dat[2] | ( ( gBlockTOBv[0][1]   & 0x00000FFF ) << 8);
  ATOB1_dat[2] =  ATOB1_dat[2] | ( ( gBlockTOBeta[0][1] & 0x0000003F ) <<20);
  ATOB1_dat[2] =  ATOB1_dat[2] | ( ( gBlockTOBphi[0][1] & 0x0000001F ) <<26);

  tobs_v[6] = std::make_unique<gFEXJetTOB>();
  tobs_v[6]->setWord(ATOB1_dat[2]);
  tobs_v[6]->setET(gBlockTOBv[0][1]);
  tobs_v[6]->setEta(gBlockTOBeta[0][1]);
  tobs_v[6]->setPhi(gBlockTOBphi[0][1]);
  tobs_v[6]->setTobID(3);
  if(  gBlockTOBv[0][1] > gJ_ptMinToTopoCounts2 ) tobs_v[6]->setStatus(1);
  else tobs_v[6]->setStatus(0);


  ATOB2_dat[2] =  0x00000004;
  if(  gBlockTOBv[1][1] > gJ_ptMinToTopoCounts1 ) ATOB2_dat[2] =  ATOB2_dat[2] | 0x00000080;
  ATOB2_dat[2] =  ATOB2_dat[2] | ( ( gBlockTOBv[1][1]   & 0x00000FFF ) << 8);
  ATOB2_dat[2] =  ATOB2_dat[2] | ( ( gBlockTOBeta[1][1] & 0x0000003F ) <<20);
  ATOB2_dat[2] =  ATOB2_dat[2] | ( ( gBlockTOBphi[1][1] & 0x0000001F ) <<26);

  tobs_v[7] = std::make_unique<gFEXJetTOB>();
  tobs_v[7]->setWord(ATOB2_dat[2]);
  tobs_v[7]->setET(gBlockTOBv[1][1]);
  tobs_v[7]->setEta(gBlockTOBeta[1][1]);
  tobs_v[7]->setPhi(gBlockTOBphi[1][1]);
  tobs_v[7]->setTobID(4);
  if(  gBlockTOBv[1][1] > gJ_ptMinToTopoCounts1 ) tobs_v[7]->setStatus(1);
  else tobs_v[7]->setStatus(0);


  BTOB1_dat[2] =  0x00000003;
  if(  gBlockTOBv[2][1] > gJ_ptMinToTopoCounts1 ) BTOB1_dat[2] = BTOB1_dat[2] | 0x00000080;
  BTOB1_dat[2] =  BTOB1_dat[2] | ( ( gBlockTOBv[2][1]   & 0x00000FFF ) << 8);
  BTOB1_dat[2] =  BTOB1_dat[2] | ( ( gBlockTOBeta[2][1] & 0x0000003F ) <<20);
  BTOB1_dat[2] =  BTOB1_dat[2] | ( ( gBlockTOBphi[2][1] & 0x0000001F ) <<26);

  tobs_v[8] = std::make_unique<gFEXJetTOB>();
  tobs_v[8]->setWord(BTOB1_dat[2]);
  tobs_v[8]->setET(gBlockTOBv[2][1]);
  tobs_v[8]->setEta(gBlockTOBeta[2][1]);
  tobs_v[8]->setPhi(gBlockTOBphi[2][1]);
  tobs_v[8]->setTobID(3);
  if(  gBlockTOBv[2][1] > gJ_ptMinToTopoCounts1 ) tobs_v[8]->setStatus(1);
  else tobs_v[8]->setStatus(0);


  BTOB2_dat[2] =  0x00000004;
  if(  gBlockTOBv[3][1] > gJ_ptMinToTopoCounts2 ) BTOB2_dat[2] = BTOB2_dat[2] | 0x00000080;
  BTOB2_dat[2] =  BTOB2_dat[2] | ( ( gBlockTOBv[3][1]   & 0x00000FFF ) << 8);
  BTOB2_dat[2] =  BTOB2_dat[2] | ( ( gBlockTOBeta[3][1] & 0x0000003F ) <<20);
  BTOB2_dat[2] =  BTOB2_dat[2] | ( ( gBlockTOBphi[3][1] & 0x0000001F ) <<26);

  tobs_v[9] = std::make_unique<gFEXJetTOB>();
  tobs_v[9]->setWord(BTOB2_dat[2]);
  tobs_v[9]->setET(gBlockTOBv[3][1]);
  tobs_v[9]->setEta(gBlockTOBeta[3][1]);
  tobs_v[9]->setPhi(gBlockTOBphi[3][1]);
  tobs_v[9]->setTobID(4);
  if(  gBlockTOBv[3][1] > gJ_ptMinToTopoCounts2 ) tobs_v[9]->setStatus(1);
  else tobs_v[9]->setStatus(0);

  // finally the main event -- lead gJET
  // according the specification https://docs.google.com/spreadsheets/d/15YVVtGofhXMtV7jXRFzWO0FVUtUAjS-X-aQjh3FKE_w/edit#gid=523371660
  // we should have an lsb of 3.2 GeV -- so ignore lower 4 bits.

  // TOBs 10-13 are leading gJets
  // shift is done before sorting as in firmware!
  int tobvShift = 8;
  int tobvMask   = 0x00000FFF;


  ATOB1_dat[3] =  0x00000005;
  if(  gJetTOBv[0] > gLJ_ptMinToTopoCounts2 ) ATOB1_dat[3] = ATOB1_dat[3] | 0x00000080;
  ATOB1_dat[3] =  ATOB1_dat[3] | ( ( gJetTOBv[0]   & tobvMask) << tobvShift);
  ATOB1_dat[3] =  ATOB1_dat[3] | ( ( gJetTOBeta[0] & 0x0000003F ) <<20);
  ATOB1_dat[3] =  ATOB1_dat[3] | ( ( gJetTOBphi[0] & 0x0000001F ) <<26);

  tobs_v[10] = std::make_unique<gFEXJetTOB>();
  tobs_v[10]->setWord(ATOB1_dat[3]);
  tobs_v[10]->setET(gJetTOBv[0]);
  tobs_v[10]->setEta(gJetTOBeta[0]);
  tobs_v[10]->setPhi(gJetTOBphi[0]);
  tobs_v[10]->setTobID(5);
  if(gJetTOBv[0] > gLJ_ptMinToTopoCounts2 ) tobs_v[10]->setStatus(1);
  else tobs_v[10]->setStatus(0);


  ATOB2_dat[3] =  0x00000006;
  if(  gJetTOBv[1] > gLJ_ptMinToTopoCounts1 )  ATOB2_dat[3] = ATOB2_dat[3] | 0x00000080;
  ATOB2_dat[3] =  ATOB2_dat[3] | ( ( gJetTOBv[1]   & tobvMask ) << tobvShift);
  ATOB2_dat[3] =  ATOB2_dat[3] | ( ( gJetTOBeta[1] & 0x0000003F ) <<20);
  ATOB2_dat[3] =  ATOB2_dat[3] | ( ( gJetTOBphi[1] & 0x0000001F ) <<26);

  tobs_v[11] = std::make_unique<gFEXJetTOB>();
  tobs_v[11]->setWord(ATOB2_dat[3]);
  tobs_v[11]->setET(gJetTOBv[1]);
  tobs_v[11]->setEta(gJetTOBeta[1]);
  tobs_v[11]->setPhi(gJetTOBphi[1]);
  tobs_v[11]->setTobID(6);
  if(  gJetTOBv[1] > gLJ_ptMinToTopoCounts1 ) tobs_v[11]->setStatus(1);
  else tobs_v[11]->setStatus(0);


  BTOB1_dat[3] =  0x00000005;
  if(  gJetTOBv[2] > gLJ_ptMinToTopoCounts1 ) BTOB1_dat[3] = BTOB1_dat[3] | 0x00000080;
  BTOB1_dat[3] =  BTOB1_dat[3] | ( ( gJetTOBv[2]   & tobvMask) << tobvShift);
  BTOB1_dat[3] =  BTOB1_dat[3] | ( ( gJetTOBeta[2] & 0x0000003F ) <<20);
  BTOB1_dat[3] =  BTOB1_dat[3] | ( ( gJetTOBphi[2] & 0x0000001F ) <<26);

  tobs_v[12] = std::make_unique<gFEXJetTOB>();
  tobs_v[12]->setWord(BTOB1_dat[3]);
  tobs_v[12]->setET(gJetTOBv[2]);
  tobs_v[12]->setEta(gJetTOBeta[2]);
  tobs_v[12]->setPhi(gJetTOBphi[2]);
  tobs_v[12]->setTobID(5);
  if(  gJetTOBv[2] > gLJ_ptMinToTopoCounts1 ) tobs_v[12]->setStatus(1);
  else tobs_v[12]->setStatus(0);


  BTOB2_dat[3] =  0x00000006;
  if(  gJetTOBv[3] > gLJ_ptMinToTopoCounts2 ) BTOB2_dat[3] = BTOB2_dat[3] | 0x00000080;
  BTOB2_dat[3] =  BTOB2_dat[3] | ( ( gJetTOBv[3]   & tobvMask   ) << tobvShift);
  BTOB2_dat[3] =  BTOB2_dat[3] | ( ( gJetTOBeta[3] & 0x0000003F ) <<20);
  BTOB2_dat[3] =  BTOB2_dat[3] | ( ( gJetTOBphi[3] & 0x0000001F ) <<26);

  tobs_v[13] = std::make_unique<gFEXJetTOB>();
  tobs_v[13]->setWord(BTOB2_dat[3]);
  tobs_v[13]->setET(gJetTOBv[3]);
  tobs_v[13]->setEta(gJetTOBeta[3]);
  tobs_v[13]->setPhi(gJetTOBphi[3]);
  tobs_v[13]->setTobID(6);
  if(  gJetTOBv[3] > gLJ_ptMinToTopoCounts2 ) tobs_v[13]->setStatus(1);
  else tobs_v[13]->setStatus(0);

   return tobs_v;

}


void gFEXJetAlgo::RemotePartialAB(gTowersCentral twrs, gTowersPartialSums & lps, gTowersPartialSums & rps){

  // Computes partial sums for FPGA A or B
  // twrs are the 32 x 12 = 284 gTowers in FPGA A or B

  // number of rows above/below for right partial sum
  // when sending to the right send values for largest partial sum first, i.e. for column 0
  typedef  std::array<std::array<int, 4>, 4> gTowersNeighbours;
  gTowersNeighbours NUpDwnR = {{ {{2,3,4,4}}, {{0,2,3,4}}, {{0,0,2,3}}, {{0,0,0,2}} }};

  // number of rows above or below for left partial sum
  // when sending to the left send values for smallest partial sum first (i.e. for column 8 )
  gTowersNeighbours NUpDwnL = {{ {{2,0,0,0}}, {{3,2,0,0}}, {{4,3,2,0}}, {{4,4,3,2}} }};

  // Do partial sum for output to right FPGA first
  for( int irow = 0; irow < FEXAlgoSpaceDefs::ABCrows; irow++ ){
    for(int rcolumn = 0; rcolumn<FEXAlgoSpaceDefs::n_partial; rcolumn++){
      // start calculating right partial sums for remote column rcolumn
      rps[irow][rcolumn] = 0;
      for(int lcolumn = 0; lcolumn<FEXAlgoSpaceDefs::n_partial; lcolumn++){
        // add in any energy from towers in this row
        // no need of modular arithmetic
        if ( NUpDwnR[rcolumn][lcolumn] > 0 ) {
          rps[irow][rcolumn] = rps[irow][rcolumn] + twrs[irow][lcolumn+8];
        }
        // now add rup1, rup2, rup3, rup4, ldn1, ldn2, ln3, ln4 -- use a loop instead of enumeratin in firmware
        for( int rowOff = 1 ; rowOff < NUpDwnR[rcolumn][lcolumn]+1; rowOff++){
          int rowModUp =  (irow + rowOff)%32;
          int rowModDn =  (irow - rowOff + 32 )%32;
          // printf("row %d remote column %d local column %d rowOff %d rowModUp %d rowModDn %d \n",
          rps[irow][rcolumn] =  rps[irow][rcolumn] + twrs[rowModUp][lcolumn+8] + twrs[rowModDn][lcolumn+8];
        }
      }
    }
  }

  // do  partial sum for output to left FPGA second
  for( int irow = 0; irow < FEXAlgoSpaceDefs::ABCrows; irow++ ){
    for(int rcolumn = 0; rcolumn<FEXAlgoSpaceDefs::n_partial; rcolumn++){
      // start calculating right partial sums for remote column rcolumn
      lps[irow][rcolumn] = 0;
      for(int lcolumn = 0; lcolumn<FEXAlgoSpaceDefs::n_partial; lcolumn++){
        // add in any energy from towers in this row
        // no need of modular arithmetic
        if ( NUpDwnL[rcolumn][lcolumn] > 0 ) {
          lps[irow][rcolumn] = lps[irow][rcolumn] + twrs[irow][lcolumn];
        }
        // now add rup1, rup2, rup3, rup4, ldn1, ldn2, ln3, ln4 -- use a loop instead of enumeratin in firmware
        for( int rowOff = 1 ; rowOff < NUpDwnL[rcolumn][lcolumn]+1; rowOff++){
          int rowModUp =  (irow + rowOff)%32;
          int rowModDn =  (irow - rowOff + 32 )%32;
          lps[irow][rcolumn] =  lps[irow][rcolumn] + twrs[rowModUp][lcolumn] + twrs[rowModDn][lcolumn];
        }
      }
    }
  }

}


void gFEXJetAlgo::RemotePartialCN(gTowersForward twrs, gTowersPartialSums & rps ){

  // Copied from AB
  typedef  std::array<std::array<int, 4>, 4> gTowersNeighbours;
  gTowersNeighbours NUpDwnR = {{ {{2,3,4,4}}, {{0,2,3,4}}, {{0,0,2,3}}, {{0,0,0,2}} }};

  // same as AB for right partial sum
  for( int irow = 0; irow < FEXAlgoSpaceDefs::ABCrows; irow++ ){
    for(int rcolumn = 0; rcolumn<FEXAlgoSpaceDefs::n_partial; rcolumn++){
      // start calculating right partial sums for remote column rcolumn
      rps[irow][rcolumn] = 0;
      for(int lcolumn = 0; lcolumn<FEXAlgoSpaceDefs::n_partial; lcolumn++){
        // add in any energy from towers in this row
        // no need of modular arithmetic
        if ( NUpDwnR[rcolumn][lcolumn] > 0 ) {
          rps[irow][rcolumn] = rps[irow][rcolumn] + twrs[irow][lcolumn];
        }
        // now add rup1, rup2, rup3, rup4, ldn1, ldn2, ln3, ln4 -- use a loop instead of enumeratin in firmware
        for( int rowOff = 1 ; rowOff < NUpDwnR[rcolumn][lcolumn]+1; rowOff++){
          int rowModUp =  (irow + rowOff)%32;
          int rowModDn =  (irow - rowOff + 32 )%32;
          rps[irow][rcolumn] =  rps[irow][rcolumn] + twrs[rowModUp][lcolumn] + twrs[rowModDn][lcolumn];
        }
      }
    }
  }

}


void gFEXJetAlgo::RemotePartialCP(gTowersForward twrs, gTowersPartialSums & lps ){

  typedef  std::array<std::array<int, 4>, 4> gTowersNeighbours;
  gTowersNeighbours NUpDwnL = {{ {{2,0,0,0}}, {{2,3,0,0}}, {{4,3,2,0}}, {{4,4,3,2}} }};
  // do  partial sum for output to left FPGA second
  for( int irow = 0; irow < FEXAlgoSpaceDefs::ABCrows; irow++ ){
    for(int rcolumn = 0; rcolumn<FEXAlgoSpaceDefs::n_partial; rcolumn++){
      // start calculating right partial sums for remote column rcolumn
      lps[irow][rcolumn] = 0;
      for(int lcolumn = 0; lcolumn<FEXAlgoSpaceDefs::n_partial; lcolumn++){
        // add in any energy from towers in this row
        // no need of modular arithmetic
        if ( NUpDwnL[rcolumn][lcolumn] > 0 ) {
          lps[irow][rcolumn] = lps[irow][rcolumn] + twrs[irow][lcolumn];
        }
        // now add rup1, rup2, rup3, rup4, ldn1, ldn2, ln3, ln4 -- use a loop instead of enumeratin in firmware
        for( int rowOff = 1 ; rowOff < NUpDwnL[rcolumn][lcolumn]+1; rowOff++){
          int rowModUp =  (irow + rowOff)%32;
          int rowModDn =  (irow - rowOff + 32 )%32;
          lps[irow][rcolumn] =  lps[irow][rcolumn] + twrs[rowModUp][lcolumn] + twrs[rowModDn][lcolumn];
        }
      }
    }
  }

}


void gFEXJetAlgo::singleAB(gTowersCentral twrs, gTowersCentral & FPGAsum){
  // Finds jets in a single FPGA

  // Number of up and down FPGAs to add
  std::array<int, 9> NUpDwn =  {{2,3,4,4,4,4,4,3,2}};

  //  Evaluate jet centered on each possible eta and phi gTower
  for( int irow = 0; irow < FEXAlgoSpaceDefs::ABCrows; irow++ ){
    for(int jcolumn = 0; jcolumn<FEXAlgoSpaceDefs::ABcolumns; jcolumn++){
      // zero jet sum here
      FPGAsum[irow][jcolumn] = 0;

      // local column goes from 0 to  8
      for(int localColumn = 0; localColumn<9; localColumn++){
        int sumColumn = jcolumn + localColumn - 4;

        // check if tower to be summed is actually in FPGA
        if( (sumColumn < FEXAlgoSpaceDefs::ABcolumns) && (sumColumn > -1) ) {

          // sum tower in the same row as jet center
          FPGAsum[irow][jcolumn] = FPGAsum[irow][jcolumn] + twrs[irow][sumColumn];

          // add rows above and below according to NUpDwn
          // localRow goes from 1 to 2, 1 to 3 or 1 to 4
          for(int localRow = 1; localRow<=NUpDwn[localColumn]; localRow++){
            int krowUp = (irow + localRow);
            if( krowUp > 31 ) krowUp = krowUp - 32;
            int krowDn = (irow - localRow);
            if( krowDn < 0 ) krowDn = krowDn + 32;
            FPGAsum[irow][jcolumn] =  FPGAsum[irow][jcolumn] +
                                      twrs[krowUp][sumColumn] + twrs[krowDn][sumColumn];
          }
        }
      }
    }
  }
}


void gFEXJetAlgo::gBlockAB(gTowersCentral twrs, gTowersCentral & gBlkSum){

  int rows = twrs.size();
  int cols = twrs[0].size();
  for( int irow = 0; irow < rows; irow++ ){
    for(int jcolumn = 0; jcolumn<cols; jcolumn++){
      // zero jet sum here
      gBlkSum[irow][jcolumn] = 0;
      int krowUp = (irow + 1)%32;
      int krowDn = (irow - 1 +32)%32;
      if( (jcolumn == 0) || (jcolumn == 6) ) {
        //left edge case
        gBlkSum[irow][jcolumn] =
          twrs[irow][jcolumn]   + twrs[krowUp][jcolumn]   + twrs[krowDn][jcolumn]   +
          twrs[irow][jcolumn+1] + twrs[krowUp][jcolumn+1] + twrs[krowDn][jcolumn+1];
            } else if( (jcolumn == 5) || (jcolumn == 11)) {
        //  right edge case
        gBlkSum[irow][jcolumn] =
          twrs[irow][jcolumn]   + twrs[krowUp][jcolumn]   + twrs[krowDn][jcolumn]   +
          twrs[irow][jcolumn-1] + twrs[krowUp][jcolumn-1] + twrs[krowDn][jcolumn-1];
            } else{
        // normal case
        gBlkSum[irow][jcolumn] =
          twrs[irow][jcolumn]   + twrs[krowUp][jcolumn]   + twrs[krowDn][jcolumn]   +
          twrs[irow][jcolumn-1] + twrs[krowUp][jcolumn-1] + twrs[krowDn][jcolumn-1] +
          twrs[irow][jcolumn+1] + twrs[krowUp][jcolumn+1] + twrs[krowDn][jcolumn+1];
        }
        // switch to 800 MeV LSB 
        gBlkSum[irow][jcolumn] =  gBlkSum[irow][jcolumn]/4;
        // limit result to an unsigned integer of 12 bits ( 2376 GeV) 
        if ( gBlkSum[irow][jcolumn] < 0 ){
          gBlkSum[irow][jcolumn] = 0;
        }
        if ( gBlkSum[irow][jcolumn] > 4091 ){
          gBlkSum[irow][jcolumn] = 4091;
        }  

    }
  }

}


void gFEXJetAlgo::blkOutAB(gTowersCentral blocks,
                           std::array<int, FEXAlgoSpaceDefs::ABCrows> jetOutL,
                           std::array<int, FEXAlgoSpaceDefs::ABCrows> etaIndL,
                           std::array<int, FEXAlgoSpaceDefs::ABCrows> jetOutR,
                           std::array<int, FEXAlgoSpaceDefs::ABCrows> etaIndR ){

  // find maximum in each jet engine for gBlocks  (not done in hardware)
  int rows = blocks.size();
  //loop over left engines
  for(int ieng=0; ieng<rows; ieng++){
    jetOutL[ieng] = 0;
    etaIndL[ieng] = 0;
    for(int localEta = 0; localEta<6; localEta++){
      if( blocks[ieng][localEta] >  jetOutL[ieng]  ){
        jetOutL[ieng] = blocks[ieng][localEta];
        etaIndL[ieng] = localEta;
      }
    }
  }
  // loop over right engines
  for(int ieng=0; ieng<rows; ieng++){
    jetOutR[ieng] = 0;
    etaIndR[ieng] = 0;
    for(int localEta = 0; localEta<6; localEta++){
      if(  blocks[ieng][localEta+6] >  jetOutR[ieng] ) {
        jetOutR[ieng] = blocks[ieng][localEta+6];
        etaIndR[ieng] = localEta;
      }
    }
  }
}


void gFEXJetAlgo::gBlockMax2(gTowersCentral gBlkSum, int BjetColumn, int localColumn, std::array<int, 3> & gBlockV, std::array<int, 3> & gBlockEta, std::array<int, 3> & gBlockPhi){

  //  gBlkSum are the 9 or 6 gTower sums  
  //  BjetColumn is the Block Column -- 0 for CN, 1, 2 for A 3, 4 for B and 5 for CP 

  gTowersJetEngine gBlkSumC;
 
  for( int irow = 0; irow<FEXAlgoSpaceDefs::ABCrows; irow++){
    for( int icolumn =0; icolumn<FEXAlgoSpaceDefs::ABcolumnsEng; icolumn++){
      gBlkSumC[irow][icolumn] = gBlkSum[irow][icolumn + localColumn*FEXAlgoSpaceDefs::ABcolumnsEng];
    }
  }


  gBlockMax192(gBlkSumC, gBlockV, gBlockEta, gBlockPhi, 0);

  for(int i = -1; i<2; i++){
    int iGlobal = i + gBlockPhi[0];
    // map row number in to 0-31
    if ( iGlobal < 0 ) iGlobal  = iGlobal + 32;
    if ( iGlobal > 31 ) iGlobal = iGlobal - 32;
    // don't do anything outside of the six columsn
    for( int j = -1; j<2; j++){
      int jGlobal = j + gBlockEta[0];
      //printf("zero %d %d \n", iGlobal, jGlobal);
      if( (jGlobal > -1)  && (jGlobal < 6) ) {
        gBlkSumC[iGlobal][jGlobal] = 0;
      }
    }
  }

  gBlockMax192(gBlkSumC, gBlockV, gBlockEta, gBlockPhi, 1);

  // need to wait until the end for this!
  gBlockEta[0] =  gBlockEta[0] + 2 + 6*BjetColumn;
  gBlockEta[1] =  gBlockEta[1] + 2 + 6*BjetColumn;
}


void gFEXJetAlgo::gBlockMax192(  gTowersJetEngine gBlkSum,
                                 std::array<int, 3> & gBlockVp,
                                 std::array<int, 3> & gBlockEtap,
                                 std::array<int, 3> & gBlockPhip,
                                 int index){

  int inpv[192];
  int maxv1[96];
  int maxv2[48];
  int maxv3[24];
  int maxv4[12];
  int maxv5[6];
  int maxv6[3];

  int inpi[192];
  int maxi1[96];
  int maxi2[48];
  int maxi3[24];
  int maxi4[12];
  int maxi5[6];
  int maxi6[3];

  int maxvall;
  int maxiall;

  // FEXAlgoSpaceDefs::ENABLE_INTER_AB is 6 in hardware
  int maxv = 0;

  int rows = gBlkSum.size();
  int cols = gBlkSum[0].size();

  for(int icolumn = 0;  icolumn<cols; icolumn++){
    for( int irow = 0; irow<rows; irow++){
      inpv[irow + icolumn*rows] = gBlkSum[irow][icolumn];
      inpi[irow + icolumn*rows] = irow + icolumn*rows;
      if( gBlkSum[irow][icolumn] > maxv){
        maxv = gBlkSum[irow][icolumn];
      }
    }
  }

  for(int i = 0; i<96; i++){
    if( inpv[2*i+1] > inpv[2*i] ) {
      maxv1[i] = inpv[2*i+1];
      maxi1[i] = inpi[2*i+1];
    } else {
      maxv1[i] = inpv[2*i];
      maxi1[i] = inpi[2*i];
    }
  }

  for(int i = 0; i<48; i++){
    if( maxv1[2*i+1] > maxv1[2*i] ) {
      maxv2[i] = maxv1[2*i+1];
      maxi2[i] = maxi1[2*i+1];
    } else {
      maxv2[i] = maxv1[2*i];
      maxi2[i] = maxi1[2*i];
    }
  }

  for(int i = 0; i<24; i++){
    if( maxv2[2*i+1] > maxv2[2*i] ) {
      maxv3[i] = maxv2[2*i+1];
      maxi3[i] = maxi2[2*i+1];
    } else {
      maxv3[i] = maxv2[2*i];
      maxi3[i] = maxi2[2*i];
    }
  }

  for(int i = 0; i<12; i++){
    if( maxv3[2*i+1] > maxv3[2*i] ) {
      maxv4[i] = maxv3[2*i+1];
      maxi4[i] = maxi3[2*i+1];
    } else {
      maxv4[i] = maxv3[2*i];
      maxi4[i] = maxi3[2*i];
    }
  }

  for(int i = 0; i<6; i++){
    if( maxv4[2*i+1] > maxv4[2*i] ) {
      maxv5[i] = maxv4[2*i+1];
      maxi5[i] = maxi4[2*i+1];
    } else {
      maxv5[i] = maxv4[2*i];
      maxi5[i] = maxi4[2*i];
    }
  }

  for(int i = 0; i<3; i++){
    if( maxv5[2*i+1] > maxv5[2*i] ) {
      maxv6[i] = maxv5[2*i+1];
      maxi6[i] = maxi5[2*i+1];
    } else {
      maxv6[i] = maxv5[2*i];
      maxi6[i] = maxi5[2*i];
    }
  }

  if( ( maxv6[1] > maxv6[0] ) && ( maxv6[1] > maxv6[2] ) ) {
    maxvall = maxv6[1];
    maxiall = maxi6[1];
  } else {
    if( maxv6[0] > maxv6[2] ){
      maxvall = maxv6[0];
      maxiall = maxi6[0];
    } else {
      maxvall = maxv6[2];
      maxiall = maxi6[2];
    }
  }


  gBlockVp[index]   =  maxvall ;
  gBlockEtap[index] =  maxiall/32;
  gBlockPhip[index] =  maxiall%32;

}


void gFEXJetAlgo::addRemoteRin(gTowersCentral &jets, const gTowersPartialSums &partial){

  int rows = partial.size();
  int cols = partial[0].size();
  // add partial sums
  for(int irow=0;irow<rows;irow++){
    for(int ipartial= 0; ipartial <cols; ipartial++){
      // current behavior
      int truncPart = partial[irow][ipartial];
      if(truncPart < 0 ) truncPart = 0;
      // change LSB from 200 MeV to 800 MeV and then back to 200 MeV.
      truncPart = (truncPart >> 2);
      if( truncPart > 511 ) truncPart = 511;
      truncPart = (truncPart << 2);
      jets[irow][8+ipartial] =  jets[irow][8+ipartial]  + truncPart;
    }
  }
}

void gFEXJetAlgo::addRemoteLin(gTowersCentral &jets, const gTowersPartialSums &partial){
  int rows = partial.size();
  int cols = partial[0].size();
  // add partial sums
  for(int irow=0;irow<rows;irow++){
    for(int ipartial= 0; ipartial <cols; ipartial++){
      // current behavior
      int truncPart = partial[irow][ipartial];
      if(truncPart < 0 ) truncPart = 0;
      // change LSB from 200 MeV to 800 MeV and then back to 200 MeV.
      truncPart = (truncPart >> 2);
      if( truncPart > 511 ) truncPart = 511;
      truncPart = (truncPart << 2);
      jets[irow][ipartial] =  jets[irow][ipartial]  + truncPart;
    }
  }
}


void gFEXJetAlgo::pileUpCalculation(gTowersCentral &twrs, int rhoThreshold_Max, int rhoThreshold_Min, int inputScale,  int &PUCp) { 
  // input are 50 MeV "fine" scale towers (i.e. inputScale = 1)
  // to use 200 MeV towers use inputScale = 4  
  //PUCp output is the pileup correction for 69 towers at 200 MeV energy scale 

  int rows = twrs.size();
  int cols = twrs[0].size();
  int pucSum = 0;
  int nSum   = 0; 
  for(int irow=0; irow<rows; irow++){
    for( int icolumn=0; icolumn<cols; icolumn++){
      int fineGT = twrs[irow][icolumn]*inputScale; 
      if( (fineGT > rhoThreshold_Min) && (fineGT < rhoThreshold_Max) ) {
      pucSum = pucSum + fineGT;
      nSum = nSum + 1;
      }
    }
  }
  // in firmware this is done with a lookup table see https://gitlab.cern.ch/atlas-l1calo/gfex/firmware/-/blob/devel/common/jet_finder/HDL/inv_lut19.vhd
  // See also https://gitlab.cern.ch/atlas-l1calo/gfex/firmware/-/blob/devel/common/jet_finder/HDL/gt_build_all_AB.vhd#L1471
  int oneOverN = 69*4096;
  if( nSum > 0 ) {
    oneOverN = oneOverN/nSum;
  } else {
    oneOverN = 0.0;
  }

  PUCp = pucSum * oneOverN;
  // divide by 4096 and convert to 200 MeV LSB 
  PUCp = PUCp/(4*4096); 
}



void gFEXJetAlgo::pileUpCorrectionAB(gTowersCentral &jets, int puc){
  int rows = jets.size();
  int cols = jets[0].size();
  // add partial sums
  for(int irow=0;irow<rows;irow++){
    for(int icolumn=0;icolumn<cols;icolumn++){
      jets[irow][icolumn] =   jets[irow][icolumn] - puc;
    }
  }
}

void gFEXJetAlgo::gJetVetoAB( gTowersCentral &twrs ,int jet_threshold ){
  int rows = twrs.size();
  int cols = twrs[0].size();
  for( int irow = 0; irow < rows; irow++ ){
    for(int jcolumn = 0; jcolumn<cols; jcolumn++){
      if( twrs[irow][jcolumn] < jet_threshold+1 ) {
        twrs[irow][jcolumn] = 0; 
      }
    } 
  }
}

void gFEXJetAlgo::gBlockVetoAB( gTowersCentral &twrs,
                                gTowersCentral blocks,
                                int seed_threshold  ){
  int rows = twrs.size();
  int cols = twrs[0].size();
  for( int irow = 0; irow < rows; irow++ ){
    for(int jcolumn = 0; jcolumn<cols; jcolumn++){
      if( blocks[irow][jcolumn] < seed_threshold+1 ) {
        twrs[irow][jcolumn] = 0;
      }
    }
  }
}


void gFEXJetAlgo::jetOutAB(gTowersCentral jets, gTowersCentral blocks, int seedThreshold,
                           std::array<int, 32> & jetOutL, std::array<int, 32> & etaIndL,
                           std::array<int, 32> & jetOutR, std::array<int, 32> & etaIndR ){
  // find maximum in each jet engine or either gJets and requires corresponding gBlock be above threhsold
  //loop over left engines
  for(int ieng=0; ieng<FEXAlgoSpaceDefs::ABCrows; ieng++){
    jetOutL[ieng] = 0;
    etaIndL[ieng] = 0;
    for(int localEta = 0; localEta<6; localEta++){
      if( (jets[ieng][localEta] >  jetOutL[ieng] ) && (blocks[ieng][localEta] > seedThreshold) ){
        jetOutL[ieng] = jets[ieng][localEta];
        etaIndL[ieng] = localEta;
      }
    }
    // Turncate to 15 bits as in firmware
    if( jetOutL[ieng] >  (1<<16) - 1 )  jetOutL[ieng] = 0x00007FFF;
    // reduce by 3 bits prior to sorting done here
    jetOutL[ieng] = jetOutL[ieng]/8; 
  }
  // loop over right engines
  for(int ieng=0; ieng<FEXAlgoSpaceDefs::ABCrows; ieng++){
    jetOutR[ieng] = 0;
    etaIndR[ieng] = 0;
    for(int localEta = 0; localEta<6; localEta++){
      if( (jets[ieng][localEta+6] > jetOutR[ieng]) && (blocks[ieng][localEta+6] > seedThreshold) ) {
       jetOutR[ieng] = jets[ieng][localEta+6];
       etaIndR[ieng] = localEta;
      }
    }
    // Turncate to 15 bits as in firmware 
    if( jetOutR[ieng] >  (1<<16) - 1 )  jetOutR[ieng] = 0x00007FFF;
    // reduce by 3 bits prior to sorting done here
    jetOutR[ieng] = jetOutR[ieng]/8; 

  }

}

void gFEXJetAlgo::gJetTOBgen(std::array<int, FEXAlgoSpaceDefs::ABCrows>  jetOut,
                             std::array<int, FEXAlgoSpaceDefs::ABCrows>  etaInd,
                             int TOBnum, int jetThreshold, std::array<int, FEXAlgoSpaceDefs::gJetTOBfib> & gJetTOBs,
                             std::array<int, FEXAlgoSpaceDefs::gJetTOBfib> & gJetTOBv,
                             std::array<int, FEXAlgoSpaceDefs::gJetTOBfib> & gJetTOBeta,
                             std::array<int, FEXAlgoSpaceDefs::gJetTOBfib> & gJetTOBphi ){

  int jetOutZS[FEXAlgoSpaceDefs::ABCrows]; 
  // apply the tobthreshold to the values 
  //note that jetThreshold is not a configurable parameter in firmware, it is used to check that jet values are positive
  for( int irow =0; irow<FEXAlgoSpaceDefs::ABCrows; irow++){
    if(  jetOut[irow] > jetThreshold ) {
      jetOutZS[irow] = jetOut[irow];
    } else {
      jetOutZS[irow] = 0 ; 
    }
  }


  // offset of TOBs according to official format
  int etaOff[4] = {8,14,20,26};

  // see tob_gen.vhd
  int l1mv[16];
  int l1met[16];
  int l1mphi[16];

  int l2mv[8];
  int l2met[8];
  int l2mphi[8];

  int l3mv[4];
  int l3met[4];
  int l3mphi[4];

  int l4mv[2];
  int l4met[2];
  int l4mphi[2];
  

  for(int i=0; i<16; i++){
    if( jetOut[2*i + 1] > jetOutZS[2*i] ){
      l1mv[i]   = jetOutZS[2*i + 1];
      l1met[i]  = etaInd[2*i + 1];
      l1mphi[i] =        2*i + 1;
    } else {
      l1mv[i]   = jetOutZS[2*i ];
      l1met[i]  = etaInd[2*i ];
      l1mphi[i] =        2*i ;
    }
  }

  for(int i=0; i<8; i++){
    if( l1mv[2*i + 1] > l1mv[2*i] ){
      l2mv[i]   = l1mv[  2*i + 1 ];
      l2met[i]  = l1met[ 2*i + 1];
      l2mphi[i] = l1mphi[2*i + 1];
    } else {
      l2mv[i]   = l1mv[  2*i ];
      l2met[i]  = l1met[ 2*i ];
      l2mphi[i] = l1mphi[2*i ];
    }
  }

  for(int i=0; i<4; i++){
    if( l2mv[2*i + 1] > l2mv[2*i] ){
      l3mv[i]   = l2mv[  2*i + 1];
      l3met[i]  = l2met[ 2*i + 1];
      l3mphi[i] = l2mphi[2*i + 1];
    } else {
      l3mv[i]   = l2mv[2*i   ];
      l3met[i]  = l2met[2*i  ];
      l3mphi[i] = l2mphi[2*i ];
    }
  }

  for(int i=0; i<2; i++){
    if( l3mv[2*i + 1] > l3mv[2*i] ){
      l4mv[i]   = l3mv[  2*i + 1];
      l4met[i]  = l3met[ 2*i + 1];
      l4mphi[i] = l3mphi[2*i + 1];
    } else {
      l4mv[i]   = l3mv[2*i   ];
      l4met[i]  = l3met[2*i  ];
      l4mphi[i] = l3mphi[2*i ];
    }
  }

  if(  l4mv[1] > l4mv[0]  ){
    if(l4mv[1] > jetThreshold){
      gJetTOBv[TOBnum]   = l4mv[1];
      gJetTOBeta[TOBnum] = l4met[1] + etaOff[TOBnum];
      gJetTOBphi[TOBnum] = l4mphi[1];
      gJetTOBs[TOBnum]   = 1;
    } else {
      gJetTOBv[TOBnum]   = l4mv[1];
      gJetTOBeta[TOBnum] = l4met[1] + etaOff[TOBnum];
      gJetTOBphi[TOBnum] = l4mphi[1];
      gJetTOBs[TOBnum]   = 0;
    }
  } else {
    if(l4mv[0] > jetThreshold){
      gJetTOBv[TOBnum]   = l4mv[0];
      gJetTOBeta[TOBnum] = l4met[0] + etaOff[TOBnum];
      gJetTOBphi[TOBnum] = l4mphi[0];
      gJetTOBs[TOBnum]   = 1;
    } else {
      gJetTOBv[TOBnum]   = l4mv[0];
      gJetTOBeta[TOBnum] = l4met[0] + etaOff[TOBnum];
      gJetTOBphi[TOBnum] = l4mphi[0];
      gJetTOBs[TOBnum]   = 0;
    }
 }

}



} // namespace LVL1
