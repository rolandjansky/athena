/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// TopPhotonMapCnv_p1.h 
// Header file for class TopPhotonMapCnv_p1
/////////////////////////////////////////////////////////////////// 
#ifndef TOP_PHOTON_MAP_CNV_P1_H 
#define TOP_PHOTON_MAP_CNV_P1_H 

#include "TopInputsTPCnv/TopInputMapCnv_p1.h"
#include "egammaEvent/PhotonContainer.h"

class TopPhotonMapCnv_p1:public TopInputMapCnv_p1<PhotonContainer>{
 public:
    TopPhotonMapCnv_p1();
    ~TopPhotonMapCnv_p1(){;}
};

#endif // TOP_PHOTON_MAP_CNV_P1_H 
