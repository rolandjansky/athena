/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileMuonReceiverReadCnt.h
// Author   : 
// Created  : April 2014
//
// DESCRIPTION
//    To read a container of the TileContainer type  
// 
// Properties (JobOption Parameters):
//
//
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILERECALGS_TILEMUONRECEIVERREADCNT_H
#define TILERECALGS_TILEMUONRECEIVERREADCNT_H

// Tile includes
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileMuonReceiverContainer.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"

#include <string>

class TileID;
class TileHWID;

class TileMuonReceiverReadCnt : public AthAlgorithm {
public:
    // Constructor
    TileMuonReceiverReadCnt(std::string name, ISvcLocator* pSvcLocator);

    // Destructor 
    virtual ~TileMuonReceiverReadCnt();                         
    
    // Gaudi Hooks
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private:

    SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey{this,"TileDigitsContainer",
                                                                "MuRcvDigitsCnt", "Input Tile digits container key"};

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,"TileRawChannelContainer",
                                                                        "MuRcvRawChCnt", 
                                                                        "Input Tile raw channel container key"};

    SG::ReadHandleKey<TileMuonReceiverContainer> m_muonReceiverContainerKey{this,"TileMuonReceiverContainer",
                                                                           "TileMuRcvCnt", 
                                                                           "Input Tile muon receiver container key"};



    const TileID* m_tileID;
    const TileHWID* m_tileHWID;
};

#endif // TILERECALGS_TILECELLVERIFY_H

