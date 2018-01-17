/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileMuToNtuple.h
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
#ifndef TILEMUID_TILEMUTONTUPLE_H
#define TILEMUID_TILEMUTONTUPLE_H

// Tile includes
#include "TileEvent/TileContainer.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"

// Gaudi includes
#include "GaudiKernel/NTuple.h"


#include <string>


class TileMuToNtuple: public AthAlgorithm {
  public:
    //Constructor
    TileMuToNtuple(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileMuToNtuple();

    //Gaudi Hooks
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

    NTuple::Tuple* m_ntuplePtr;
    int m_ntupleID;
    int m_maxNtag;
    int m_close;
    std::string m_ntupleLoc;

    NTuple::Item<int> m_ntag;
    NTuple::Array<float> m_eta;
    NTuple::Array<float> m_phi;
    NTuple::Matrix<float> m_energy;
    NTuple::Array<float> m_quality;

    std::string m_tileMuContainer;

    SG::ReadHandleKey<TileMuContainer> m_muContainerKey{this,"TileMuContainer",
                                                        "TileMuObj",
                                                        "Input Tile mu container key"};

};

#endif
