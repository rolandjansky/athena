/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileTTL1ToNtuple.h
// Author   : Sanya.Solodkov@cern.ch
// Created  : Nov 2005
//
// DESCRIPTION
// 
//    To create Ntuple with all TileTTL1 file from TileTTL1Container
//
// Properties (JobOption Parameters):
//
//    TileTTL1Container           string   key value of TileTTL1 in TDS 
//    NtupleLoc                   string   pathname of ntuple file
//    NtupleID                    string   ID of ntuple
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************
#ifndef TileTTL1ToNtuple_H
#define TileTTL1ToNtuple_H

#include "GaudiKernel/NTuple.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class CaloLVL1_ID;
class TileTBID;

#include <string>

class TileTTL1ToNtuple : public AthAlgorithm {
public:
    //Constructor
    TileTTL1ToNtuple(const std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileTTL1ToNtuple();                         
    
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
    int  m_nSamples;

    NTuple::Item<int> m_nchan;

    NTuple::Array<int> m_side;
    NTuple::Array<int> m_eta;
    NTuple::Array<int> m_phi;
    
    NTuple::Matrix<float> m_samples;

    std::string m_ttl1Container;

    const CaloLVL1_ID* m_TT_ID;
    const TileTBID*    m_tileTBID;
};

#endif
