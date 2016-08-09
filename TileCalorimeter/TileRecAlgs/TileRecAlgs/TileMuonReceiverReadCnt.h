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

#include "AthenaBaseComps/AthAlgorithm.h"

class TileID;
class TileHWID;

#include <string>

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

    std::string m_TileMuRcvContainer;    
    std::string m_TileRawChannelContainer;    
    std::string m_TileDigitsContainer;

    const TileID* m_tileID;
    const TileHWID* m_tileHWID;
};

#endif // TILERECALGS_TILECELLVERIFY_H

