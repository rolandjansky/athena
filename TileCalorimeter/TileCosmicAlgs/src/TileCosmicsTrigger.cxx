/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileCosmicsTrigger.cxx
//  Author   : jose.maneira@cern.ch
//  Created  : Nov 2005
//  Reviewed : March 2007 remove std.alone ntp, output to TileEvent/TileTrigger
//            May   2007 set 8 boards
//
//  DESCRIPTION:
//     Implement the algorithm
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

//Gaudi includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Calo include
#include "CaloIdentifier/CaloLVL1_ID.h" 

//TileCalo include
#include "TileEvent/TileTTL1Container.h"  
#include "TileEvent/TileTriggerContainer.h"  
#include "TileCosmicAlgs/TileCosmicsTrigger.h"
#include "TileIdentifier/TileTTL1Hash.h"

const int TileCosmicsTrigger::m_NMaxTowers = NMAXTOWERS;

TileCosmicsTrigger::TileCosmicsTrigger(const std::string name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
  , m_ttl1Container("TileTTL1Cnt")
  , m_TThreshold(5.0)
  , m_NBOARDS(8)
  , m_NDRAWERSPERBOARD(12)
  , m_NTOWERSPERDRAWER(8)
  , m_TileTriggerContainer("TileTriggerCnt")
  , m_TT_ID(0)
  , m_TTHash(0)
{
  for (int iboard = 0; iboard < m_NBOARDS; iboard++) {
    m_ConnectedDrawers[iboard].resize(m_NDRAWERSPERBOARD);
    m_ConnectedDrawers1.resize(m_NDRAWERSPERBOARD);
    m_ConnectedDrawers2.resize(m_NDRAWERSPERBOARD);
    m_ConnectedDrawers3.resize(m_NDRAWERSPERBOARD);
    m_ConnectedDrawers4.resize(m_NDRAWERSPERBOARD);
    m_ConnectedDrawers5.resize(m_NDRAWERSPERBOARD);
    m_ConnectedDrawers6.resize(m_NDRAWERSPERBOARD);
    m_ConnectedDrawers7.resize(m_NDRAWERSPERBOARD);
    m_ConnectedDrawers8.resize(m_NDRAWERSPERBOARD);
  }

//------------------------------------------------------------------
// Setting default configuration
// See talk by Christophe Clement March 1, 2007
// -----------------------------------------------------------------
//  Board   Connected to
//   1    bottom LBA
//   2    bottom EBA
//   3    top	 LBA
//   4    top	 EBA
//   5    bottom LBC
//   6    bottom EBC
//   7    top	 LBC
//   8    top	 EBC
//-------------------------------------------------------------------

  m_ConnectedDrawers1[0] = "LBA43";
  m_ConnectedDrawers1[1] = "LBA44";
  m_ConnectedDrawers1[2] = "LBA45";
  m_ConnectedDrawers1[3] = "LBA46";
  m_ConnectedDrawers1[4] = "LBA47";
  m_ConnectedDrawers1[5] = "LBA48";
  m_ConnectedDrawers1[6] = "LBA49";
  m_ConnectedDrawers1[7] = "LBA50";
  m_ConnectedDrawers1[8] = "LBA51";
  m_ConnectedDrawers1[9] = "LBA52";
  m_ConnectedDrawers1[10] = "LBA53";
  m_ConnectedDrawers1[11] = "LBA54";

  m_ConnectedDrawers2[0] = "EBA43";
  m_ConnectedDrawers2[1] = "EBA44";
  m_ConnectedDrawers2[2] = "EBA45";
  m_ConnectedDrawers2[3] = "EBA46";
  m_ConnectedDrawers2[4] = "EBA47";
  m_ConnectedDrawers2[5] = "EBA48";
  m_ConnectedDrawers2[6] = "EBA49";
  m_ConnectedDrawers2[7] = "EBA50";
  m_ConnectedDrawers2[8] = "EBA51";
  m_ConnectedDrawers2[9] = "EBA52";
  m_ConnectedDrawers2[10] = "EBA53";
  m_ConnectedDrawers2[11] = "EBA54";

  m_ConnectedDrawers3[0] = "LBA11";
  m_ConnectedDrawers3[1] = "LBA12";
  m_ConnectedDrawers3[2] = "LBA13";
  m_ConnectedDrawers3[3] = "LBA14";
  m_ConnectedDrawers3[4] = "LBA15";
  m_ConnectedDrawers3[5] = "LBA16";
  m_ConnectedDrawers3[6] = "LBA17";
  m_ConnectedDrawers3[7] = "LBA18";
  m_ConnectedDrawers3[8] = "LBA19";
  m_ConnectedDrawers3[9] = "LBA20";
  m_ConnectedDrawers3[10] = "LBA21";
  m_ConnectedDrawers3[11] = "LBA22";

  m_ConnectedDrawers4[0] = "EBA11";
  m_ConnectedDrawers4[1] = "EBA12";
  m_ConnectedDrawers4[2] = "EBA13";
  m_ConnectedDrawers4[3] = "EBA14";
  m_ConnectedDrawers4[4] = "EBA15";
  m_ConnectedDrawers4[5] = "EBA16";
  m_ConnectedDrawers4[6] = "EBA17";
  m_ConnectedDrawers4[7] = "EBA18";
  m_ConnectedDrawers4[8] = "EBA19";
  m_ConnectedDrawers4[9] = "EBA20";
  m_ConnectedDrawers4[10] = "EBA21";
  m_ConnectedDrawers4[11] = "EBA22";

  m_ConnectedDrawers5[0] = "LBC43";
  m_ConnectedDrawers5[1] = "LBC44";
  m_ConnectedDrawers5[2] = "LBC45";
  m_ConnectedDrawers5[3] = "LBC46";
  m_ConnectedDrawers5[4] = "LBC47";
  m_ConnectedDrawers5[5] = "LBC48";
  m_ConnectedDrawers5[6] = "LBC49";
  m_ConnectedDrawers5[7] = "LBC50";
  m_ConnectedDrawers5[8] = "LBC51";
  m_ConnectedDrawers5[9] = "LBC52";
  m_ConnectedDrawers5[10] = "LBC53";
  m_ConnectedDrawers5[11] = "LBC54";

  m_ConnectedDrawers6[0] = "EBC43";
  m_ConnectedDrawers6[1] = "EBC44";
  m_ConnectedDrawers6[2] = "EBC45";
  m_ConnectedDrawers6[3] = "EBC46";
  m_ConnectedDrawers6[4] = "EBC47";
  m_ConnectedDrawers6[5] = "EBC48";
  m_ConnectedDrawers6[6] = "EBC49";
  m_ConnectedDrawers6[7] = "EBC50";
  m_ConnectedDrawers6[8] = "EBC51";
  m_ConnectedDrawers6[9] = "EBC52";
  m_ConnectedDrawers6[10] = "EBC53";
  m_ConnectedDrawers6[11] = "EBC54";

  m_ConnectedDrawers7[0] = "LBC11";
  m_ConnectedDrawers7[1] = "LBC12";
  m_ConnectedDrawers7[2] = "LBC13";
  m_ConnectedDrawers7[3] = "LBC14";
  m_ConnectedDrawers7[4] = "LBC15";
  m_ConnectedDrawers7[5] = "LBC16";
  m_ConnectedDrawers7[6] = "LBC17";
  m_ConnectedDrawers7[7] = "LBC18";
  m_ConnectedDrawers7[8] = "LBC19";
  m_ConnectedDrawers7[9] = "LBC20";
  m_ConnectedDrawers7[10] = "LBC21";
  m_ConnectedDrawers7[11] = "LBC22";

  m_ConnectedDrawers8[0] = "EBC11";
  m_ConnectedDrawers8[1] = "EBC12";
  m_ConnectedDrawers8[2] = "EBC13";
  m_ConnectedDrawers8[3] = "EBC14";
  m_ConnectedDrawers8[4] = "EBC15";
  m_ConnectedDrawers8[5] = "EBC16";
  m_ConnectedDrawers8[6] = "EBC17";
  m_ConnectedDrawers8[7] = "EBC18";
  m_ConnectedDrawers8[8] = "EBC19";
  m_ConnectedDrawers8[9] = "EBC20";
  m_ConnectedDrawers8[10] = "EBC21";
  m_ConnectedDrawers8[11] = "EBC22";

  declareProperty("TileTTL1Container", m_ttl1Container);
  declareProperty("TileTriggerContainer", m_TileTriggerContainer);
  declareProperty("ConnectedDrawers1", m_ConnectedDrawers1);
  declareProperty("ConnectedDrawers2", m_ConnectedDrawers2);
  declareProperty("ConnectedDrawers3", m_ConnectedDrawers3);
  declareProperty("ConnectedDrawers4", m_ConnectedDrawers4);
  declareProperty("ConnectedDrawers5", m_ConnectedDrawers5);
  declareProperty("ConnectedDrawers6", m_ConnectedDrawers6);
  declareProperty("ConnectedDrawers7", m_ConnectedDrawers7);
  declareProperty("ConnectedDrawers8", m_ConnectedDrawers8);
  declareProperty("Threshold", m_TThreshold);

  // fix coverity complaints about these not being initialized in constructor
  for (int i = 0; i < m_NMaxTowers; i++) {
    m_ConnectedTowers[i] = 0;
    m_FiredTowers[i] = 0;
    m_TowerSum[i] = 0.0;
    m_BoardOfTower[i] = 0;
  }
}

TileCosmicsTrigger::~TileCosmicsTrigger() {
}

//****************************************************************************
//* Initialization
//****************************************************************************

StatusCode TileCosmicsTrigger::initialize() {


// retrieve TileID helper from det store

  CHECK( detStore()->retrieve(m_TT_ID) );


// convert string input into vector

  m_TTHash = new TileTTL1Hash();
  m_TTHash->initialize(m_TT_ID);
  for (int i = 0; i < m_NMaxTowers; i++)
    m_ConnectedTowers[i] = 0;

  for (int idrawer = 0; idrawer < m_NDRAWERSPERBOARD; idrawer++) {
    m_ConnectedDrawers[0][idrawer] = m_ConnectedDrawers1[idrawer];
    m_ConnectedDrawers[1][idrawer] = m_ConnectedDrawers2[idrawer];
    m_ConnectedDrawers[2][idrawer] = m_ConnectedDrawers3[idrawer];
    m_ConnectedDrawers[3][idrawer] = m_ConnectedDrawers4[idrawer];
    m_ConnectedDrawers[4][idrawer] = m_ConnectedDrawers5[idrawer];
    m_ConnectedDrawers[5][idrawer] = m_ConnectedDrawers6[idrawer];
    m_ConnectedDrawers[6][idrawer] = m_ConnectedDrawers7[idrawer];
    m_ConnectedDrawers[7][idrawer] = m_ConnectedDrawers8[idrawer];
  }
  for (int iboard = 0; iboard < m_NBOARDS; iboard++) {
    for (int idrawer = 0; idrawer < m_NDRAWERSPERBOARD; idrawer++) {
      if (m_ConnectedDrawers[iboard][idrawer] == "") continue;
      bool failure = 0;
      int section = 0;
      int side = 0;
      // get section (long or extended barrel)
      if (m_ConnectedDrawers[iboard][idrawer][0] == 'L'
          || m_ConnectedDrawers[iboard][idrawer][0] == 'l') {
        section = 1;
      } else if (m_ConnectedDrawers[iboard][idrawer][0] == 'E'
                || m_ConnectedDrawers[iboard][idrawer][0] == 'e') {
        section = 2;
      } else {
        failure = 1;
      }
      // get side (positive or negative)
      if (m_ConnectedDrawers[iboard][idrawer][2] == 'A'
          || m_ConnectedDrawers[iboard][idrawer][2] == 'a') {
        side = 1;
      } else if (m_ConnectedDrawers[iboard][idrawer][2] == 'C'
                 || m_ConnectedDrawers[iboard][idrawer][2] == 'c') {
        side = -1;
      } else {
        failure = 1;
      }
      int phi = -1 + atoi(m_ConnectedDrawers[iboard][idrawer].substr(3, m_ConnectedDrawers[iboard][idrawer].length() - 3).c_str());

      if (phi < 0 || phi > 63) failure = 1;

      if (failure) {
        ATH_MSG_ERROR( "Invalid Tile Drawer " << m_ConnectedDrawers[iboard][idrawer] );
        return StatusCode::FAILURE;
      }
      int mineta, maxeta;
      if (section == 1) {
        mineta = 0;
        maxeta = 7;
      } else {
        mineta = 9;
        maxeta = 14;
      }
      ATH_MSG_DEBUG( "Board " << iboard
                    << " Drawer " << idrawer
                    << " " << m_ConnectedDrawers[iboard][idrawer]
                    << " Section " << section
                    << " Side " << side
                    << " Phi " << phi );

      for (int ieta = mineta; ieta <= maxeta; ieta++) {
        Identifier ttid = m_TT_ID->tower_id(side, 1, 0, ieta, phi);
        int index = m_TTHash->operator()(m_TT_ID->tower_hash(ttid));
        m_ConnectedTowers[index] = 1;
        m_BoardOfTower[index] = iboard;
      }
    }
  }

  int nt = 0;
  for (int i = 0; i < m_NMaxTowers; i++) {
    ATH_MSG_DEBUG( "tower " << i << " connected " << m_ConnectedTowers[i] );
    if (m_ConnectedTowers[i] == 1) nt++;
  }

  ATH_MSG_INFO( "Initialization completed, number of towers " << nt );

  return StatusCode::SUCCESS;
}

//****************************************************************************
//* Execution
//****************************************************************************

StatusCode TileCosmicsTrigger::execute() {

  // step1: read TileTTL1s from TDS
  const TileTTL1Container* TTL1Cnt;
  CHECK( evtStore()->retrieve(TTL1Cnt, m_ttl1Container) );

  ATH_MSG_DEBUG( "loaded TileTTL1Container: " <<  m_ttl1Container );

  TileTriggerContainer* pTileTriggerContainer = new TileTriggerContainer();
  TileTrigger* pTileTrigger;

  // step2: reset towers 
  for (int i = 0; i < m_NMaxTowers; i++) {
    m_FiredTowers[i] = 0;
    m_TowerSum[i] = 0.;
  }

  int m_nbacktowers = 0;
  int m_nchan = 0;

  Identifier p_maxTowerID;
  double p_maxTowerSum = 0;
  std::vector<Identifier> p_boardTowerID;
  std::vector<double> p_boardTowerSum;
  std::vector<Identifier> p_backTowerID;
  std::vector<double> p_backTowerSum;
  p_boardTowerID.resize(m_NBOARDS);
  p_boardTowerSum.resize(m_NBOARDS);
  p_backTowerID.resize(NMAXTOWERS);
  p_backTowerSum.resize(NMAXTOWERS);

  // step3: check which towers fired the discriminator
  TileTTL1Container::const_iterator it = TTL1Cnt->begin();
  TileTTL1Container::const_iterator end = TTL1Cnt->end();

  for (; it != end; ++it) {
    const TileTTL1 * cinp = (*it);
    Identifier id = cinp->TTL1_ID();

    std::vector<float> samples = cinp->fsamples();
    samples.resize(1);

    ATH_MSG_VERBOSE( " ichan=" << m_nchan
                    << " " << m_TT_ID->pos_neg_z(id)
                    << " /"<< m_TT_ID->eta(id)
                    << " /" << m_TT_ID->phi(id)
                    << " peak =" << samples[0] );


    if (samples[0] > m_TThreshold) {
      int index = m_TTHash->operator()(m_TT_ID->tower_hash(id));
      if (m_ConnectedTowers[index]) {
        m_FiredTowers[index] = 1;
        m_TowerSum[index] = samples[0];
        if (samples[0] > p_maxTowerSum) {
          p_maxTowerSum = samples[0];
          p_maxTowerID = id;
        }
      }
    }

    m_nchan++;
    if (m_nchan >= m_NMaxTowers) {
      ATH_MSG_DEBUG( "Number of ttl1s exceeds maximum (" << m_NMaxTowers << "), ignore all the rest" );
      break;
    }
  }

  // step4: compute tower coincidence logic
  int phi1, phi2;
  phi1 = phi2 = 0;

  Identifier id1;
  Identifier id2;
  // register maximum tower in each board
  for (int i = 0; i < m_NMaxTowers; i++) {
    if (m_FiredTowers[i] == 0) continue;
    id1 = m_TT_ID->tower_id(m_TTHash->identifier(i));
    if (m_TowerSum[i] > p_boardTowerSum[m_BoardOfTower[i]]) {
      id1 = m_TT_ID->tower_id(m_TTHash->identifier(i));
      p_boardTowerID[m_BoardOfTower[i]] = id1;
      p_boardTowerSum[m_BoardOfTower[i]] = m_TowerSum[i];
    }
  }

  // loop through all tower pairs for back-to-back coincidence
  for (int i = 0; i < m_NMaxTowers - 1; i++) {
    if (m_FiredTowers[i] == 0) continue;
    id1 = m_TT_ID->tower_id(m_TTHash->identifier(i));
    for (int j = i + 1; j < m_NMaxTowers; j++) {
      //  check back-to-back trigger
      if (m_FiredTowers[j] == 0) continue;
      id2 = m_TT_ID->tower_id(m_TTHash->identifier(j));
      phi1 = m_TT_ID->phi(id1);
      phi2 = m_TT_ID->phi(id2);
      if ((std::max(phi1, phi2) - std::min(phi1, phi2)) != 32) continue;
      if (m_TT_ID->pos_neg_z(id1) == m_TT_ID->pos_neg_z(id2)) continue;
      p_backTowerID[m_nbacktowers] = id1;
      p_backTowerSum[m_nbacktowers] = m_TowerSum[i];
      m_nbacktowers++;
      p_backTowerID[m_nbacktowers] = id2;
      p_backTowerSum[m_nbacktowers] = m_TowerSum[j];
      m_nbacktowers++;
    }
  }

  p_backTowerID.resize(m_nbacktowers);
  p_backTowerSum.resize(m_nbacktowers);
  pTileTrigger = new TileTrigger(p_maxTowerID, p_maxTowerSum, p_boardTowerID, p_boardTowerSum, p_backTowerID, p_backTowerSum);

  pTileTriggerContainer->push_back(pTileTrigger);

  // step8: register the TileTrigger container in the TES
  CHECK( evtStore()->record(pTileTriggerContainer, m_TileTriggerContainer, false) );
  ATH_MSG_DEBUG( "TileTriggerContainer registered successfully (" << m_TileTriggerContainer << ")" );

  // Execution completed.
  ATH_MSG_DEBUG( "execute() completed successfully" );

  return StatusCode::SUCCESS;
}

//****************************************************************************
//* Finalize
//****************************************************************************

StatusCode TileCosmicsTrigger::finalize() {

  ATH_MSG_INFO( "finalize() completed successfully" );

  return StatusCode::SUCCESS;
}

