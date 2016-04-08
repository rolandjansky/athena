/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileHitVecToNtuple.h
// Author   : Mikhail Makouski
// Created  : July. 2005
//
// DESCRIPTION
//
//    To create Hit Ntuple file from TileHitVector
//
// Properties (JobOption Parameters):
//
//    TileHitVector               string   key value of HitVector
//    NtupleLoc                   string   pathname of ntuple file
//    NtupleID                    string   ID of ntuple
//
// BUGS:
//
// History:
//
//
//****************************************************************************

#ifndef TileHitVecToNtuple_H
#define TileHitVecToNtuple_H

#include "GaudiKernel/NTuple.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class TileID;

#include <string>

class TileHitVecToNtuple : public AthAlgorithm {
public:
    // Constructor
    TileHitVecToNtuple(const std::string& name, ISvcLocator* pSvcLocator);
  
    //Destructor
    virtual ~TileHitVecToNtuple();
  
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
  
    std::string    m_hitVector;
    
    const TileID*   m_tileID;
};

#endif
