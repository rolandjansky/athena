/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileCellToNtuple.h
// Author   : Zhifang
// Created  : Jan. 2003
//
// DESCRIPTION
// 
//    To create Cell Ntuple file from CaloCell container
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
#ifndef TileCellToNtuple_H
#define TileCellToNtuple_H

#include "GaudiKernel/NTuple.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class TileID;
class TileTBID;

#include <string>

class TileCellToNtuple : public AthAlgorithm {
public:
    //Constructor
    TileCellToNtuple(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileCellToNtuple();                         
    
    //Gaudi Hooks
    StatusCode initialize();    
    StatusCode execute();
    StatusCode finalize();

private:
    NTuple::Tuple* m_ntuplePtr;
    std::string m_ntupleID;
    std::string m_ntupleLoc;
    bool m_scinCells;

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
    
    NTuple::Array<int> m_type;
    NTuple::Array<int> m_channel;
    NTuple::Array<int> m_module;

    std::string m_cellContainer;

    const TileID*   m_tileID;
    const TileTBID*   m_tileTBID;
};

#endif
