/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// TopMuonMapCnv_p2.h 
// Header file for class TopMuonMapCnv_p2
/////////////////////////////////////////////////////////////////// 
#ifndef TOPINPUTSTPCNV_TOPMUONMAPCNV_P2_H 
#define TOPINPUTSTPCNV_TOPMUONMAPCNV_P2_H 

#include "TopInputsTPCnv/TopInputMapTriggerCnv_p2.h"
#include "muonEvent/MuonContainer.h"
#include "TopInputs/TopMuonMap.h"

class TopMuonMapCnv_p2:public TopInputMapTriggerCnv_p2<Analysis::MuonContainer>{
  public:
    TopMuonMapCnv_p2();
    ~TopMuonMapCnv_p2(){;}
};

#endif // not TOPINPUTSTPCNV_TOPMUONMAPCNV_P2_H
