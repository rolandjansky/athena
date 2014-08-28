/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// TopMuonMapCnv_p1.h 
// Header file for class TopMuonMapCnv_p1
/////////////////////////////////////////////////////////////////// 
#ifndef TOPINPUTSTPCNV_TOPMUONMAPCNV_P1_H 
#define TOPINPUTSTPCNV_TOPMUONMAPCNV_P1_H 

#include "TopInputsTPCnv/TopInputMapTriggerCnv_p1.h"
#include "muonEvent/MuonContainer.h"
#include "TopInputs/TopMuonMap.h"

class TopMuonMapCnv_p1 : public TopInputMapTriggerCnv_p1<Analysis::MuonContainer>{
  public:
    TopMuonMapCnv_p1();
    ~TopMuonMapCnv_p1(){;}
};

#endif // not TOPINPUTSTPCNV_TOPMUONMAPCNV_P1_H 
