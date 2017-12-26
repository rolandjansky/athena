/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileCellVerify.h
// Author   : UC-ATLAS TileCal group
// Created  : May 2002
//
// DESCRIPTION
//    to verify the cells reconstructed from different route or data source.
//    For example, hit->cell, hit->rawchannel->cell, 
//    hit->digits->rawchannel->cell, rawcell->cell, digits->rawchannel->cel
//    etc.
//    Every time it can only compare the results from two routes.
//    Print out the different on the console.
// 
// Properties (JobOption Parameters):
//
//    TileCellContainer1   string   Name of CaloCellContainer created by the
//                                  first route for read
//    TileCellContainer2   string   Name of CaloCellContainer created by the
//                                  second route for read
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILERECALGS_TILECELLVERIFY_H
#define TILERECALGS_TILECELLVERIFY_H

// Calo includes
#include "CaloEvent/CaloCellContainer.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"

#include <string>

class TileID;

class TileCellVerify : public AthAlgorithm {
public:
    // Constructor
    TileCellVerify(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileCellVerify();                         
    
    //Gaudi Hooks
    StatusCode initialize();    
    StatusCode execute();
    StatusCode finalize();

private:

    SG::ReadHandleKey<CaloCellContainer> m_cellContainer1Key{this,"TileCellContainer1",
                                                              "TileCellContainer1",
                                                              "Input Calo cell container 1 key"};

    SG::ReadHandleKey<CaloCellContainer> m_cellContainer2Key{this,"TileCellContainer2",
                                                              "TileCellContainer1",
                                                              "Input Calo cell container 2 key"};


    const TileID* m_tileID;
  
    bool m_dumpCells;
    bool m_sortFlag;
};

#endif // TILERECALGS_TILECELLVERIFY_H

