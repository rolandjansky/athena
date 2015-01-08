/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************************************************************
// Filename : TileMuonReceiverDecisionToNtuple.h
// Author   : Joao Gentil Saraiva <jmendes@lxplus.cern.ch>
// Created  : April 2014
//
// DESCRIPTION
// 
//    To create Ntuple file from TileMuonReceiverContainer
//
// Properties (JobOption Parameters):
//
//    TileMuRcvContainer    string   key value of Tilecal decision for the tile-d L1Muon Trigger in TDS 
//    NtupleLoc             string   pathname of ntuple file
//    NtupleID              int      ID of ntuple
//
// BUGS:
//  
// History:
//  
//  
//*********************************************************************************************************

#ifndef TileMuonReceiverDecisionToNtuple_H
#define TileMuonReceiverDecisionToNtuple_H

#include "GaudiKernel/NTuple.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class TileTBID;
class TileID;

#include <string>

class TileMuonReceiverDecisionToNtuple : public AthAlgorithm {
public:
    //Constructor
    TileMuonReceiverDecisionToNtuple(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileMuonReceiverDecisionToNtuple();                         
    
    //Gaudi Hooks
    StatusCode initialize();    
    StatusCode execute();
    StatusCode finalize();

private:
    NTuple::Tuple* m_ntuplePtr;
    std::string    m_ntupleID;
    std::string    m_ntupleLoc;

    int m_nMaxTrigInputs; 
    int m_nBits;          
//    int m_nTrigInput;
    
    NTuple::Item<int>     m_nTrigInput;    
    NTuple::Array<int>    m_partition;
    NTuple::Array<int>    m_module;

    NTuple::Array<float>  m_energy_D5andD6; 
    NTuple::Array<float>  m_energy_D6;
    NTuple::Array<float>  m_time_D5andD6;
    NTuple::Array<float>  m_time_D6;

    NTuple::Item<float>   m_thershold_hi_d5andd6;
    NTuple::Item<float>   m_thershold_lo_d5andd6;
    NTuple::Item<float>   m_thershold_hi_d6;
    NTuple::Item<float>   m_thershold_lo_d6;

    NTuple::Array<bool>   m_trigbit0;
    NTuple::Array<bool>   m_trigbit1;
    NTuple::Array<bool>   m_trigbit2;
    NTuple::Array<bool>   m_trigbit3;

    //const TileTBID*   m_tileTBID;
    //const TileID*     m_tileID;

    std::string m_TileMuRcvContainer;
    std::string m_TileMuRcvContainer_dbg;
};

#endif
