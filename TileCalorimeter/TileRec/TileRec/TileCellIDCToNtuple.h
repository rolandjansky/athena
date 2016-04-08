/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileCellIDCToNtuple.h
// Author   : Zhifang
// Created  : Jan. 2003
//
// DESCRIPTION
// 
//    To create Cell Ntuple file from CellIDC container
//
// Properties (JobOption Parameters):
//
//    TileCellContainer           string   key value of Cells in TDS 
//    NtupleLoc                   string   pathname of ntuple file
//    NtupleID                    int      ID of ntuple
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************
#ifndef TileCellIDCToNtuple_H
#define TileCellIDCToNtuple_H

#include "GaudiKernel/NTuple.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class TileID;

#include <string>

class TileCellIDCToNtuple : public AthAlgorithm {
public:
    //Constructor
    TileCellIDCToNtuple(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileCellIDCToNtuple();                         
    
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
    NTuple::Array<float> m_enediff;
    NTuple::Array<float> m_time;
    NTuple::Array<float> m_quality;

    NTuple::Array<int> m_detector;
    NTuple::Array<int> m_side;
    NTuple::Array<int> m_sample;
    NTuple::Array<int> m_eta;
    NTuple::Array<int> m_phi;
    
    std::string m_cellContainer;
    const TileID*   m_tileID;
};

#endif
