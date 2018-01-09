/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileCosmicsTrigger.h
// Author   : jose.maneira@cern.ch
// Created  : Nov 2005
// Reviewed : March 2007 remove std.alone ntp, output to TileEvent/TileTrigger
//            May   2007 set 8 boards
//
// DESCRIPTION
// 
//    To emulate the cosmics trigger in several different configurations
//    To create Ntuple a vector with flags for each type of trigger
//
// Properties (JobOption Parameters):
//
//    TileTTL1Container           string   key value of TileTTL1 in TDS 
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************
#ifndef TILECOSMICALGS_TILECOSMICSTRIGGER_H
#define TILECOSMICALGS_TILECOSMICSTRIGGER_H
//TileCalo include
#include "TileEvent/TileTTL1Container.h"
#include "TileEvent/TileTriggerContainer.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class CaloLVL1_ID;
class TileTTL1Hash;

#include <string>

#define NMAXTOWERS 1920

/**
 *
 * @class TileCosmicsTrigger
 * @brief Simulates the trigger logic in the Tile standalone board.
 *
 * This class simulates the response of the Tile standalone cosmics
 * trigger board. It reads the TileTTL1 objects created at digitization level
 * and applies the board coincidence logic. The output is not just a flag,
 * but a set of energies per board, so that the trigger logic can be applied
 * for different thresholds at the analysis level.
 */

class TileCosmicsTrigger: public AthAlgorithm {
  public:
    //Constructor
    TileCosmicsTrigger(const std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileCosmicsTrigger();

    //Gaudi Hooks
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

// Input variables
//
    std::vector<std::string> m_connectedDrawers[8];
    std::vector<std::string> m_connectedDrawers1;
    std::vector<std::string> m_connectedDrawers2;
    std::vector<std::string> m_connectedDrawers3;
    std::vector<std::string> m_connectedDrawers4;
    std::vector<std::string> m_connectedDrawers5;
    std::vector<std::string> m_connectedDrawers6;
    std::vector<std::string> m_connectedDrawers7;
    std::vector<std::string> m_connectedDrawers8;
    float m_TThreshold;

// Constants and auxiliary variables
//
    static const int m_NMaxTowers;
    int m_NBOARDS;
    int m_NDRAWERSPERBOARD;
    //int m_NTOWERSPERDRAWER;

    bool m_connectedTowers[NMAXTOWERS];
    bool m_firedTowers[NMAXTOWERS];
    float m_towerSum[NMAXTOWERS];
    int m_boardOfTower[NMAXTOWERS];

// Store Gate & Auxiliary algorithms 
//

    const CaloLVL1_ID* m_TT_ID;
    TileTTL1Hash* m_TTHash;

    SG::ReadHandleKey<TileTTL1Container> m_ttl1ContainerKey{this, "TileTTL1Container", "TileTTL1Cnt", "Input TileTTL1Container name"};
    SG::WriteHandleKey<TileTriggerContainer> m_triggerContainerKey{this, "TileTriggerContainer","TileTriggerCnt", "Output TileTriggerContainer name"};

};

#endif // TILECOSMICALGS_TILECOSMICSTRIGGER_H
