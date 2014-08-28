/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// TopTauJetMapCnv_p1.h 
// Header file for class TopTauJetMapCnv_p1
/////////////////////////////////////////////////////////////////// 
#ifndef TOP_TAU_JET_MAP_CNV_P1_H 
#define TOP_TAU_JET_MAP_CNV_P1_H 

#include "TopInputsTPCnv/TopInputMapCnv_p1.h"
#include "tauEvent/TauJetContainer.h"

class TopTauJetMapCnv_p1:public TopInputMapCnv_p1<Analysis::TauJetContainer>{
 public:
    TopTauJetMapCnv_p1();
    ~TopTauJetMapCnv_p1(){;}
};

#endif // TOP_TAU_JET_MAP_CNV_P1_H 
