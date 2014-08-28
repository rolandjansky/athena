/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// TopJetMapCnv_p1.h 
// Header file for class TopJetMapCnv_p1
/////////////////////////////////////////////////////////////////// 
#ifndef TOP_JET_MAP_CNV_P1_H 
#define TOP_JET_MAP_CNV_P1_H 

#include "TopInputsTPCnv/TopInputMapCnv_p1.h"
#include "JetEvent/JetCollection.h"

class TopJetMapCnv_p1:public TopInputMapCnv_p1<JetCollection>{
 public:
    TopJetMapCnv_p1();
    ~TopJetMapCnv_p1(){;}
};

#endif // TOP_JET_MAP_CNV_P1_H 
