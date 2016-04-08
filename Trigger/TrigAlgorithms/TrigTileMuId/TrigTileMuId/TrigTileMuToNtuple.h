/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TrigTileMuToNtuple.h
// 
// create ntuple with tile mu tag information  
//
//  JobOption Parameters
//
//    TileMuTagContainer     string   key value of the TileMuContainer in TDS 
//    NtupleLoc              string   pathname of ntuple file
//    NtupleID               int      ID of ntuple
//
//****************************************************************************
#ifndef TRIGTILEMUID_TRIGTILEMUTONTUPLE_H
#define TRIGTILEMUID_TRIGTILEMUTONTUPLE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"
#include <string>

class TrigTileMuToNtuple : public AthAlgorithm {
public:
    //Constructor
    TrigTileMuToNtuple(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TrigTileMuToNtuple();                         
    
    //Gaudi Hooks
    StatusCode initialize();    
    StatusCode execute();
    StatusCode finalize();

private:
    NTuple::Tuple* m_ntuplePtr;
    int m_ntupleID;
    int max_ntag;
    int m_close;
    std::string m_ntupleLoc;
    
    NTuple::Item<long> m_ntag;
    NTuple::Array<float> m_eta;
    NTuple::Array<float> m_phi;
    NTuple::Matrix<float> m_energy;
    NTuple::Array<float> m_quality;

    std::string m_TileMuContainer;

 };

#endif
