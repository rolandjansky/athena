/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileRawChannelToNtuple.h
// Author   : Zhifang
// Created  : Nov. 2002
//
// DESCRIPTION
// 
//    To create RawChannel Ntuple file from RawChannel container
//
// Properties (JobOption Parameters):
//
//    TileRawChannelContainer     string   key value of RawChannels in TDS 
//    NtupleLoc                   string   pathname of ntuple file
//    NtupleID                    int      ID of ntuple
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************
#ifndef TileRawChannelToNtuple_H
#define TileRawChannelToNtuple_H

#include "GaudiKernel/NTuple.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class TileID;
class TileHWID;

#include <string>

class TileRawChannelToNtuple : public AthAlgorithm {
public:
    //Constructor
    TileRawChannelToNtuple(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileRawChannelToNtuple();                         
    
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
    NTuple::Array<float> m_quality;

    NTuple::Array<int> m_detector;
    NTuple::Array<int> m_side;
    NTuple::Array<int> m_sample;
    NTuple::Array<int> m_eta;
    NTuple::Array<int> m_phi;
    NTuple::Array<int> m_pmt;
    NTuple::Array<int> m_channel;
    NTuple::Array<int> m_gain;
    
    std::string m_rawChannelContainer;

    const TileID*   m_tileID;
    const TileHWID* m_tileHWID;
};

#endif
