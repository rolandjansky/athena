/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileL2Builder.cxx
//  Author   : Aranzazu Ruiz
//  Created  : February, 2009
//
//  DESCRIPTION:
//     AlgTool used to build the TileL2 objects.
//     This tool is used by TileRawChannelToL2 and TileROD_Decoder.
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
#include "TileL2Algs/TileL2Builder.h"
#include "TileEvent/TileL2.h"
#include "TileEvent/TileRawChannel.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolNoiseRawChn.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileDetDescr/TileDetDescrManager.h"

// Calo includes
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/TileTBID.h"
#include "CaloIdentifier/TileID.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadHandle.h"

// Gaudi incldes
#include "GaudiKernel/ServiceHandle.h"


static const InterfaceID IID_ITileL2Builder("TileL2Builder", 1, 0);

const InterfaceID& TileL2Builder::interfaceID() {
  return IID_ITileL2Builder;
}

TileL2Builder::TileL2Builder(const std::string& type, const std::string& name,
    const IInterface* parent)
    : AthAlgTool(type, name, parent)
  , m_noiseThreshold(100.0) // 100 MeV universal cut for now
  , m_noiseType(9999)       // this huge value means that noise cut is taken from JO
  , m_tileHWID(0)
  , m_tileToolEmscale("TileCondToolEmscale")
  , m_tileToolNoiseRawChn("TileCondToolNoiseRawChn")
  , m_tileBadChanTool("TileBadChanTool")
{

  declareInterface<TileL2Builder>(this);

  declareProperty("NoiseThreshold", m_noiseThreshold);       // use channels only above noise cut
  declareProperty("Noise", m_noiseType);            // choose between electronic or total noise
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);      // Name of calibration tool
  declareProperty("TileCondToolNoiseRawChn", m_tileToolNoiseRawChn=nullptr);  // Name of tool with noise RMS
  declareProperty("TileBadChanTool", m_tileBadChanTool);      // Name of bad channel tool
}

TileL2Builder::~TileL2Builder() {
}

StatusCode TileL2Builder::initialize() {

  CHECK( detStore()->retrieve(m_tileHWID) );

  // get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );

  // get TileBadChanTool
  CHECK( m_tileBadChanTool.retrieve() );

  // Initialize
  this->m_hashFunc.initialize(m_tileHWID);

  const TileID* tileID;
  CHECK( detStore()->retrieve(tileID) );

  const TileTBID* tileTBID;
  CHECK( detStore()->retrieve(tileTBID) );

  // retrieve Tile detector manager and TileID helper from det store
  const TileDetDescrManager* tileMgr;
  CHECK( detStore()->retrieve(tileMgr) );

  ServiceHandle<TileCablingSvc> cablingSvc("TileCablingSvc", name());
  CHECK( cablingSvc.retrieve());

  const TileCablingService* cabling = cablingSvc->cablingService();
  if (!cabling) {
    ATH_MSG_ERROR( "Unable to retrieve TileCablingService" );
    return StatusCode::FAILURE;
  }

  for (int ros = 0; ros < 4; ++ros) {
    for (int ch = 0; ch < 48; ++ch) {
      m_channelPairs[ros][ch] = ch;
      m_connected[ros][ch] = true;
    }
  }

  int drawer = 0;
  for (int ros = 1; ros < 5; ++ros) {
    int ros1 = ros - 1;
    for (int ch = 0; ch < 48; ++ch) {
      HWIdentifier ch_id = m_tileHWID->channel_id(ros, drawer, ch);
      Identifier cell_id = cabling->h2s_cell_id(ch_id);

      if (cell_id.is_valid()
          && !tileTBID->is_tiletb(cell_id)
          && tileID->sample(cell_id) != TileID::SAMP_E) {

        m_sinTh[ros1][ch] = tileMgr->get_cell_element(cell_id)->sinTh();
        m_sinThRound[ros1][ch] = round(m_sinTh[ros1][ch] * 32768) / 32768.; // set 15 bit presision a-la DSP
        m_cosTh[ros1][ch] = fabs(tileMgr->get_cell_element(cell_id)->cosTh()); // Always positive !!!
        m_cosThRound[ros1][ch] = round(m_cosTh[ros1][ch] * 32768) / 32768.; // set 15 bit presision a-la DSP

        Identifier pmt1_id = tileID->pmt_id(cell_id, 0);
        Identifier pmt2_id = tileID->pmt_id(cell_id, 1);
        HWIdentifier ch1_id = cabling->s2h_channel_id(pmt1_id);
        HWIdentifier ch2_id = cabling->s2h_channel_id(pmt2_id);
        int ch1 = m_tileHWID->channel(ch1_id);
        int ch2 = m_tileHWID->channel(ch2_id);
        if (ch1 != ch && ch2 != ch) {
          ATH_MSG_ERROR( "wrong cabling! ch=" << ch
                        << " ch1=" << ch1
                        << " ch2=" << ch2 );

        } else {
          m_channelPairs[ros1][ch1] = ch2;
          m_channelPairs[ros1][ch2] = ch1;
        }

        ATH_MSG_VERBOSE( "ros=" << ros
                        << " ch=" << ch
                        << " tower=" << tileID->tower(cell_id)
                        << " sample=" << tileID->sample(cell_id)
                        << " sinTh=" << m_sinThRound[ros1][ch]
                        << " cosTh=" << m_cosThRound[ros1][ch]
                        << " channels in a cell: " << ch1 << " " << ch2 );

      } else {
        m_sinTh[ros1][ch] = 0.0;
        m_sinThRound[ros1][ch] = 0;
        m_cosTh[ros1][ch] = 0.0;
        m_cosThRound[ros1][ch] = 0;
        m_connected[ros1][ch] = false;
        ATH_MSG_VERBOSE( "ros=" << ros
                        << " ch=" << ch
                        << " sinTh=" << m_sinThRound[ros1][ch]
                        << " cosTh=" << m_cosThRound[ros1][ch] );
      }
    }
  }

  ATH_CHECK( m_rawChannelContainerKey.initialize() );

  ATH_MSG_INFO( "TileL2Builder initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileL2Builder::process(int fragmin, int fragmax, TileL2Container *l2Container) {

  // Get TileRawChannels
  SG::ReadHandle<TileRawChannelContainer> rawChannelContainer(m_rawChannelContainerKey);
  ATH_CHECK( rawChannelContainer.isValid() );

  std::vector<unsigned int> extraWord;
  std::vector<float> EtaMuons;
  std::vector<float> EMuons0;
  std::vector<float> EMuons1;
  std::vector<float> EMuons2;
  std::vector<unsigned int> qf;
  std::vector<float> sumE(3);

  float E_MeV[48];
  bool bad[48];
  int gain[48];

  TileRawChannelUnit::UNIT rChUnit = rawChannelContainer->get_unit();
  bool dspCont = (rChUnit >= TileRawChannelUnit::OnlineOffset);
  bool recalibrate = (rChUnit != TileRawChannelUnit::MegaElectronVolts
                      && rChUnit != TileRawChannelUnit::OnlineMegaElectronVolts);

  // iterate over all collections in a container
  for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {

    HWIdentifier drawer_id = m_tileHWID->drawer_id(rawChannelCollection->identify());
    int ros = m_tileHWID->ros(drawer_id);
    int drawer = m_tileHWID->drawer(drawer_id);
    int fragId = m_tileHWID->frag(ros, drawer);
    int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

    if ((fragId >= fragmin) && (fragId <= fragmax)) {

      ATH_MSG_VERBOSE( "Unpacking frag 0x" << MSG::hex << fragId << MSG::dec );

      memset(E_MeV, 0, sizeof(E_MeV));
      memset(bad, 0, sizeof(bad));
      memset(gain, -1, sizeof(gain));

      // iterate over all raw channels in a collection
      for (const TileRawChannel* rawChannel : *rawChannelCollection) {
        HWIdentifier adc_id = rawChannel->adc_HWID();
        int channel = m_tileHWID->channel(adc_id);
        int adc = m_tileHWID->adc(adc_id);
        float ampl = rawChannel->amplitude();
        if (recalibrate) {
          E_MeV[channel] = m_tileToolEmscale->channelCalib(drawerIdx, channel, adc, ampl, rChUnit
                                                          , TileRawChannelUnit::MegaElectronVolts);
        } else {
          E_MeV[channel] = ampl; // no conversion since energy is in MeV already
        }
        if (dspCont) {
          bad[channel] = rawChannel->quality() > 15.99;
        } else {
          bad[channel] = m_tileBadChanTool->getAdcStatus(drawerIdx, channel, adc).isBad();
        }
        gain[channel] = adc;
      }

      // bad channel masking
      MaskBad(ros, E_MeV, gain, bad);

      // MET
      SumE(ros, drawer, TileRawChannelUnit::MegaElectronVolts, E_MeV, gain, sumE);
      (*l2Container)[m_hashFunc(fragId)]->setEt(sumE);

      // MTag

      EtaMuons.clear();
      EMuons0.clear();
      EMuons1.clear();
      EMuons2.clear();
      qf.clear();
      extraWord.clear();

      switch (ros) {
        case TileHWID::BARREL_POS:
          MTagLB(ros, drawer, E_MeV, EtaMuons, EMuons0, EMuons1, EMuons2, qf, extraWord);
          break;
        case TileHWID::BARREL_NEG:
          MTagLB(ros, drawer, E_MeV, EtaMuons, EMuons0, EMuons1, EMuons2, qf, extraWord);
          break;
        case TileHWID::EXTBAR_POS:
          MTagEB(ros, drawer, E_MeV, EtaMuons, EMuons0, EMuons1, EMuons2, qf, extraWord);
          break;
        case TileHWID::EXTBAR_NEG:
          MTagEB(ros, drawer, E_MeV, EtaMuons, EMuons0, EMuons1, EMuons2, qf, extraWord);
          break;
        default:
          ATH_MSG_ERROR( "incorrect ros value " << ros );
          break;
      }

      if (EtaMuons.size())
        (*l2Container)[m_hashFunc(fragId)]->setMu(EtaMuons, EMuons0, EMuons1, EMuons2, qf, extraWord);

    } // end loop over collections

  }

  // Execution completed
  ATH_MSG_DEBUG( "TileL2Builder execution completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileL2Builder::finalize() {

  ATH_MSG_INFO( "Finalizing" );

  return StatusCode::SUCCESS;
}

void TileL2Builder::MTagLB(int partition
                          , int drawer
                          , float* E
                          , std::vector<float> &EtaMuons
                          , std::vector<float> &EMuons0
                          , std::vector<float> &EMuons1
                          , std::vector<float> &EMuons2
                          , std::vector<unsigned int> &qf
                          , std::vector<unsigned int> &extraWord) {

  // Maximum number of muons tagged per superdrawer
  int cut = 4;

  // Energy thresholds (MeV)
  int Eth_low = 150;
  int Eth_LB_A[8] = { 1350, 960, 870, 750, 840, 840, 930, 840 };
  int Eth_LB_BC[8] = { 2550, 1980, 1770, 1890, 1860, 1800, 1890, 1860 };
  int Eth_LB_D[4] = { 1590, 1110, 1050, 1050 };

  // Eta cells
  float eta_LB_A[8] = { 0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75 }; // A1, A2, A3, A4, A5, A6, A7, A8
  float eta_LB_BC[8] = { 0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75 }; // BC1, BC2, BC3, BC4, BC5, BC6, BC7, BC8
  float eta_LB_D[4] = { 0.00, 0.20, 0.40, 0.60 };  // D0, D1, D2, D3

  // D cells to BC cells in LB
  int cell32[4 * 4] = {
      1, 0, 0, 0,  // D0 -> BC1
      2, 1, 2, 0,  // D1 -> BC2, BC3
      3, 3, 4, 5,  // D2 -> BC4, BC5, BC6
      3, 5, 6, 7   // D3 -> BC6, BC7, BC8
  };

  drawer = drawer % 2;

  int NMuons = 0;
  int quality;
  int pattern = 0;

  int k2, ks, ksto2;
  int cand = 0;

  unsigned int cquality[4] = { 0 };
  unsigned int flquality[4] = { 0 };
  unsigned int aquality[4] = { 0 };

  int eta[4] = { 0 };
  int splited[4] = { 0 };
  int found[4] = { 0 };

  float E_A[8];
  float E_BC[8];
  float E_D[4];

  /* A cells */

  E_A[0] = E[1] + E[4];   // A1
  E_A[1] = E[5] + E[8];   // A2
  E_A[2] = E[9] + E[10];  // A3
  E_A[3] = E[15] + E[18];  // A4
  E_A[4] = E[19] + E[20];  // A5
  E_A[5] = E[23] + E[26];  // A6
  E_A[6] = E[29] + E[32];  // A7
  E_A[7] = E[35] + E[38];  // A8

  /* BC cells */

  E_BC[0] = E[2] + E[3];   // BC1
  E_BC[1] = E[6] + E[7];   // BC2
  E_BC[2] = E[11] + E[12];  // BC3
  E_BC[3] = E[16] + E[17];  // BC4
  E_BC[4] = E[21] + E[22];  // BC5
  E_BC[5] = E[28] + E[27];  // BC6
  E_BC[6] = E[34] + E[33];  // BC7
  E_BC[7] = E[40] + E[39];  // BC8

  /* D cells */

  E_D[0] = E[0] * 2;         // D0
  E_D[1] = E[13] + E[14];  // D1
  E_D[2] = E[24] + E[25];  // D2
  E_D[3] = E[41] + E[44];  // D3

  /* Define candidates */

  for (int i = 0; i < 4; i++) {  // loop over D cells
    if (E_D[i] >= Eth_low) {
      eta[cand] = i;
      splited[cand] = -1;
      cquality[cand] = E_D[i] < Eth_LB_D[i] ? 0 : 1;
      cand++;
    }
  }

  /* Repair for muons splitted */

  if (cand > 4) std::abort();
  for (int i = 0; i < cand; i++) {
    for (int j = 0; j < i; j++) {
      if ((cquality[i] + cquality[j]) == 0) {
        if (abs(eta[i] - eta[j]) <= 1
            && (E_D[eta[i]] + E_D[eta[j]])
                < (Eth_LB_D[eta[i]] > Eth_LB_D[eta[j]] ? Eth_LB_D[eta[i]] : Eth_LB_D[eta[j]])) {

          splited[i] = j;

        }
      } else if ((cquality[i] + cquality[j]) == 1) {

        if (abs(eta[i] - eta[j]) <= 1) {

          splited[i] = j;

        }
      }
    }
  }

  /* Loop on the candidates */

  for (int i = 0;
      (i < cand && splited[i] < 0) || ((i < cand && splited[i] >= 0) && found[splited[i]] == 0);
      i++) {

    found[i] = 0;
    ksto2 = cell32[4 * eta[i]];

    for (ks = 1; ks <= ksto2 && found[i] != 1; ks++) {

      // eta position BC cell with a muon candidate 
      k2 = cell32[4 * eta[i] + ks];

      // Lower threshold BC cells
      if (E_BC[k2] > Eth_low) {
        flquality[i] = (E_BC[k2] < Eth_LB_BC[k2] ? 0 : 1);
        if (found[i] != 1 && E_A[k2] > Eth_low) {
          aquality[i] = (E_A[k2] < Eth_LB_A[k2] ? 0 : 1);
          quality = cquality[i] + flquality[i] + aquality[i];
          if (quality <= 1) {
            found[i] = 1;

            // energy deposited by the muon (A cell)
            EMuons0.push_back(E_A[k2]);

            // energy deposited by the muon (BC cell)
            EMuons1.push_back(E_BC[k2]);

            // energy deposited by the muon (D cell)
            EMuons2.push_back(E_D[eta[i]]);

            // quality factor
            qf.push_back(cquality[i] + flquality[i] + aquality[i]);

            // LBC partition
            if (partition == 1)
              EtaMuons.push_back((eta_LB_D[eta[i]] + eta_LB_BC[k2] + eta_LB_A[k2]) * (1. / 3));

            // LBA partition
            if (partition == 2)
              EtaMuons.push_back(-(eta_LB_D[eta[i]] + eta_LB_BC[k2] + eta_LB_A[k2]) * (1. / 3));

            if (eta[i] == 0 && k2 == 0) pattern = 0;  // D0, BC1, A1
            if (eta[i] == 1 && k2 == 1) pattern = 1;  // D1, BC2, A2
            if (eta[i] == 1 && k2 == 2) pattern = 2;  // D1, BC3, A3
            if (eta[i] == 2 && k2 == 3) pattern = 3;  // D2, BC4, A4
            if (eta[i] == 2 && k2 == 4) pattern = 4;  // D2, BC5, A5
            if (eta[i] == 2 && k2 == 5) pattern = 5;  // D2, BC6, A6
            if (eta[i] == 3 && k2 == 5) pattern = 6;  // D3, BC6, A6
            if (eta[i] == 3 && k2 == 6) pattern = 7;  // D3, BC7, A7
            if (eta[i] == 3 && k2 == 7) pattern = 8;  // D3, BC8, A8

            // Words with encoded muon information for one superdrawer
            extraWord.push_back( (quality << 31) | (drawer << 30) | (pattern << 25) | ((int) (2 * E_D[eta[i]])));
            extraWord.push_back((((int) (2 * E_BC[k2])) << 16) | ((int) (2 * E_A[k2])));

            NMuons++;
          }
        }
      }
    }
    if (NMuons >= cut) break;
  }
}

void TileL2Builder::MTagEB(int partition
                          , int drawer
                          , float* E
                          , std::vector<float> &EtaMuons
                          , std::vector<float> &EMuons0
                          , std::vector<float> &EMuons1
                          , std::vector<float> &EMuons2
                          , std::vector<unsigned int> &qf
                          , std::vector<unsigned int> &extraWord) {

  // Maximum number of muons tagged per superdrawer
  int cut = 4;

  // Energy thresholds (MeV)
  int Eth_low = 150;
  int Eth_EB_A[4] = { 810, 1140, 1230, 1380 };
  int Eth_EB_BC[5] = { 1050, 1380, 1440, 1500, 1680 };
  int Eth_EB_D[2] = { 2100, 2370 };

  // Eta cells
  float eta_EB_A[4] = { 1.15, 1.25, 1.35, 1.45 };  // A12, A13, A14, A15
  float eta_EB_BC[5] = { 1.05, 1.15, 1.25, 1.35, 1.45 };  // B11, B12, B13, B14, B15
  float eta_EB_D[2] = { 1.00, 1.20 };  // D5, D6

  // D cells to B cells in EB
  int cell32[2 * 6] = {
      3, 0, 1, 2, 0, 0,  // D5 -> B11, B12, B13
      5, 0, 1, 2, 3, 4   // D6 -> B11, B12, B13, B14, B15
  };

  // B cells to A cells in EB
  int cell21[5 * 4] = {
      1, 0, 0, 0,  // B11 -> A12
      2, 0, 1, 0,  // B12 -> A12, A13
      3, 0, 1, 2,  // B13 -> A12, A13, A14
      3, 1, 2, 3,  // B14 -> A13, A14, A15
      2, 2, 3, 0   // B15 -> A14, A15
  };

  drawer = drawer % 2;

  int NMuons = 0;
  int quality;
  int pattern = 0;

  int k2, ks, ksto2, k1, ksto1;
  int cand = 0;

  unsigned int cquality[4] = { 0 };
  unsigned int flquality[4] = { 0 };
  unsigned int aquality[4] = { 0 };

  int eta[4] = { 0 };
  int splited[4] = { 0 };
  int found[4] = { 0 };

  float E_A[4];
  float E_BC[5];
  float E_D[2];

  /* A cells */

  E_A[0] = E[6] + E[7];   // A12
  E_A[1] = E[10] + E[11];  // A13
  E_A[2] = E[20] + E[21];  // A14
  E_A[3] = E[31] + E[32];  // A15

  /* B cells */

  E_BC[0] = E[8] + E[9];   // B11
  E_BC[1] = E[14] + E[15];  // B12
  E_BC[2] = E[22] + E[23];  // B13
  E_BC[3] = E[30] + E[35];  // B14
  E_BC[4] = E[36] + E[39];  // B15

  /* D cells */

  E_D[0] = E[16] + E[17];  // D5
  E_D[1] = E[37] + E[38];  // D6

  /* Define candidates */

  for (int i = 0; i < 2; i++) {  // loop over D cells
    if (E_D[i] >= Eth_low) {
      eta[cand] = i;
      splited[cand] = -1;
      cquality[cand] = E_D[i] < Eth_EB_D[i] ? 0 : 1;
      cand++;
    }
  }

  /* Repair for muons splitted */

  if (cand > 4) std::abort();
  for (int i = 0; i < cand; i++) {
    for (int j = 0; j < i; j++) {
      if ((cquality[i] + cquality[j]) == 0) {
        if (abs(eta[i] - eta[j]) <= 1
            && (E_D[eta[i]] + E_D[eta[j]])
                < (Eth_EB_D[eta[i]] > Eth_EB_D[eta[j]] ? Eth_EB_D[eta[i]] : Eth_EB_D[eta[j]])) {

          splited[i] = j;
        }
      } else if ((cquality[i] + cquality[j]) == 1) {
        if (abs(eta[i] - eta[j]) <= 1) {
          splited[i] = j;
        }
      }
    }
  }

  /* Loop on the candidates */

  for (int i = 0;
      (i < cand && splited[i] < 0) || ((i < cand && splited[i] >= 0) && found[splited[i]] == 0);
      i++) {

    found[i] = 0;

    ksto2 = cell32[6 * eta[i]];

    for (ks = 1; ks <= ksto2 && found[i] != 1; ks++) {

      // eta position BC cell with a muon candidate 
      k2 = cell32[6 * eta[i] + ks];

      // Lower threshold BC cells
      if (E_BC[k2] > Eth_low) {

        flquality[i] = (E_BC[k2] < Eth_EB_BC[k2] ? 0 : 1);
        ksto1 = cell21[4 * k2 + 0];

        for (int kp = 1; kp <= ksto1 && found[i] != 1; kp++) {
          k1 = cell21[4 * k2 + kp];
          if (E_A[k1] > Eth_low) {
            aquality[i] = (E_A[k1] < Eth_EB_A[k1] ? 0 : 1);
            quality = cquality[i] + flquality[i] + aquality[i];
            if (quality <= 1) {
              found[i] = 1;

              // energy deposited by the muon (A cell)
              EMuons0.push_back(E_A[k1]);

              // energy deposited by the muon (B cell)
              EMuons1.push_back(E_BC[k2]);

              // energy deposited by the muon (D cell)
              EMuons2.push_back(E_D[eta[i]]);

              // quality factor
              qf.push_back(cquality[i] + flquality[i] + aquality[i]);

              // EBC partition
              if (partition == 3)
                EtaMuons.push_back((eta_EB_D[eta[i]] + eta_EB_BC[k2] + eta_EB_A[k1]) * (1. / 3));

              // EBA partition
              if (partition == 4)
                EtaMuons.push_back(-(eta_EB_D[eta[i]] + eta_EB_BC[k2] + eta_EB_A[k1]) * (1. / 3));

              if (eta[i] == 0 && k2 == 0 && k1 == 0) pattern = 0;   // D5, B11, A12
              if (eta[i] == 0 && k2 == 1 && k1 == 0) pattern = 1;   // D5, B12, A12
              if (eta[i] == 0 && k2 == 1 && k1 == 1) pattern = 2;   // D5, B12, A13
              if (eta[i] == 0 && k2 == 2 && k1 == 0) pattern = 3;   // D5, B13, A12
              if (eta[i] == 0 && k2 == 2 && k1 == 1) pattern = 4;   // D5, B13, A13
              if (eta[i] == 0 && k2 == 2 && k1 == 2) pattern = 5;   // D5, B13, A14
              if (eta[i] == 1 && k2 == 0 && k1 == 0) pattern = 6;   // D6, B11, A12
              if (eta[i] == 1 && k2 == 1 && k1 == 0) pattern = 7;   // D6, B12; A12
              if (eta[i] == 1 && k2 == 1 && k1 == 1) pattern = 8;   // D6, B12; A13
              if (eta[i] == 1 && k2 == 2 && k1 == 0) pattern = 9;   // D6, B13, A12
              if (eta[i] == 1 && k2 == 2 && k1 == 1) pattern = 10;  // D6, B13, A13
              if (eta[i] == 1 && k2 == 2 && k1 == 2) pattern = 11;  // D6, B13, A14
              if (eta[i] == 1 && k2 == 3 && k1 == 1) pattern = 12;  // D6, B14, A13
              if (eta[i] == 1 && k2 == 3 && k1 == 2) pattern = 13;  // D6, B14, A14
              if (eta[i] == 1 && k2 == 3 && k1 == 3) pattern = 14;  // D6, B14, A15
              if (eta[i] == 1 && k2 == 4 && k1 == 2) pattern = 15;  // D6, B15, A14
              if (eta[i] == 1 && k2 == 4 && k1 == 3) pattern = 16;  // D6, B15, A15

              // Words with encoded muon information for one superdrawer
              extraWord.push_back( (quality << 31)
                                   | (drawer << 30)
                                   | (pattern << 25)
                                   | ((int) (2 * E_D[eta[i]])));

              extraWord.push_back((((int) (2 * E_BC[k2])) << 16) | ((int) (2 * E_A[k1])));

              NMuons++;

            }
          }
        }
      }
    }
    if (NMuons >= cut) break;
  }
}

void TileL2Builder::SumE(int ros, int /* drawer */, int unit, float *E, int *gain,
    std::vector<float> &sumE) {
  // precision in DSP - 1/16 ADC counts or 1/2048 pC and CspC  or 1/2 MeV
  static const float AMPLITUDE_FACTOR_HG[4] = { 16.0, 32.0 * 64, 32.0 * 64., 2.0 };
  static const float AMPLITUDE_FACTOR_LG[4] = { 16.0, 32.0, 32.0, 2.0 / 64.0 };

  if (ros < 1 || ros > 4) {
    sumE[0] = sumE[1] = sumE[2] = 0.0;
    return;
  }

  unit &= 3; // keep last 2 bits, remove online offset
  const float scaleHG = AMPLITUDE_FACTOR_HG[unit];
  const float scaleLG = AMPLITUDE_FACTOR_LG[unit];
  int ros1 = ros - 1;
  float* wt = m_sinThRound[ros1];
  float* wz = m_cosThRound[ros1];
  bool* connected = m_connected[ros1];

  float Et = 0.0;
  float Ez = 0.0;
  float Es = 0.0;

  // for the moment only fixed noise cut implemented
  // and it works correctly only if energy is in MeV
  float cut = m_noiseThreshold * scaleHG;

  for (int i = 0; i < 48; ++i) {
    if (*connected++) {
      float e = (*gain) ? round((*E) * scaleHG) : round((*E) * scaleLG) * 64.;
      if (e > cut) {
        Et += round((*wt) * e);
        Ez += round((*wz) * e);
        Es += round(e);
      }
    }
    ++E;
    ++gain;
    ++wt;
    ++wz;
  }

  const float inv_scaleHG = 1. / scaleHG;
  sumE[0] = Et * inv_scaleHG;
  sumE[1] = Ez * inv_scaleHG;
  sumE[2] = Es * inv_scaleHG;
}

void TileL2Builder::SumE(int ros, int /* drawer */, float *E, std::vector<float> &sumE) {
  if (ros < 1 || ros > 4) {
    sumE[0] = sumE[1] = sumE[2] = 0.0;
    return;
  }

  int ros1 = ros - 1;
  float* wt = m_sinTh[ros1];
  float* wz = m_cosTh[ros1];
  bool* connected = m_connected[ros1];
  float Et = 0.0;
  float Ez = 0.0;
  float Es = 0.0;

  for (int i = 0; i < 48; ++i) {
    if (*connected++) {
      float e = (*E); // assuming that E is in MeV
      if (e > m_noiseThreshold) { // for the moment only fixed noise cut implemented
        Et += e * (*wt);
        Ez += e * (*wz);
        Es += e;
      }
    }
    ++E;
    ++wt;
    ++wz;
  }

  sumE[0] = Et;
  sumE[1] = Ez;
  sumE[2] = Es;
}

void TileL2Builder::MaskBad(int ros, float *E, int * gain, bool *bad) {
  for (int i = 0; i < 48; ++i) {
    if (bad[i]) {
      int j = m_channelPairs[ros - 1][i];
      if (bad[j]) { // both bad or i=j (i.e. single channel)
        E[i] = E[j] = 0.0;
        gain[i] = gain[j] = -1;
      } else {
        E[i] = E[j]; // copy from second channel
        gain[i] = gain[j];
      }
    }
  }
  return;
}

