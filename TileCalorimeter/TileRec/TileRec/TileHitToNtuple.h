/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileHitToNtuple.h
// Author   : Gia
// Created  : March. 2003
//
// DESCRIPTION
// 
//    To create Hit Ntuple file from TileHitContainer
//
// Properties (JobOption Parameters):
//
//    TileHitContainer            string   key value of Hits in TDS 
//    NtupleLoc                   string   pathname of ntuple file
//    NtupleID                    string   ID of ntuple
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************
#ifndef TileHitToNtuple_H
#define TileHitToNtuple_H

#include "GaudiKernel/NTuple.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class TileID;

#include <string>

class TileHitToNtuple : public AthAlgorithm {
public:
    //Constructor
    TileHitToNtuple(const std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileHitToNtuple();                         
    
    //Gaudi Hooks
    StatusCode initialize();    
    StatusCode execute();
    StatusCode finalize();

private:
    NTuple::Tuple* m_ntuplePtr;
    std::string m_ntupleID;
    std::string m_ntupleLoc;
    bool m_commitNtuple;
    int  m_maxLength;

    NTuple::Item<int> m_nchan;
    NTuple::Item<float> m_tolE;

    NTuple::Array<float> m_energy;
    NTuple::Array<float> m_time;
    NTuple::Array<int> m_pmt ; 

    NTuple::Array<int> m_detector;
    NTuple::Array<int> m_side;
    NTuple::Array<int> m_sample;
    NTuple::Array<int> m_eta;
    NTuple::Array<int> m_phi;
    
    std::string m_hitContainer;

    const TileID*   m_tileID;
};

#endif
