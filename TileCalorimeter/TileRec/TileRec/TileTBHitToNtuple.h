/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileTBHitToNtuple.h
// Author   : Anna
// Created  : June 2004
//
// DESCRIPTION
// 
//    To create Hit Ntuple file from TileHitContainer
//
// Properties (JobOption Parameters):
//
//    TileHitContainer            string   key value of Hits in TDS 
//    NtupleLoc                   string   pathname of ntuple file
//    NtupleID                    int      ID of ntuple
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************
#ifndef TileTBHitToNtuple_H
#define TileTBHitToNtuple_H

#include "GaudiKernel/NTuple.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class TileTBID;

#include <string>

class TileTBHitToNtuple : public AthAlgorithm {
public:
    //Constructor
    TileTBHitToNtuple(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileTBHitToNtuple();                         
    
    //Gaudi Hooks
    StatusCode initialize();    
    StatusCode execute();
    StatusCode finalize();

private:
    NTuple::Tuple* m_ntuplePtr;
    std::string m_ntupleID;
    std::string m_ntupleLoc;

    NTuple::Item<int> m_nchan;
    NTuple::Item<float> m_tolE;

    NTuple::Array<float> m_energy;
    NTuple::Array<float> m_time;

    NTuple::Array<int> m_type;
    NTuple::Array<int> m_channel;
    NTuple::Array<int> m_module;
    
    std::string m_hitContainer;

    const TileTBID*   m_tileTBID;
};

#endif
