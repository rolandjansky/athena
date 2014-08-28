/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// TopTrackMapCnv_p1.h 
// Header file for class TopTrackMapCnv_p1
/////////////////////////////////////////////////////////////////// 
#ifndef TOP_TRACK_MAP_CNV_P1_H 
#define TOP_TRACK_MAP_CNV_P1_H 

#include "TopInputsTPCnv/TopInputMapCnv_p1.h"
#include "Particle/TrackParticleContainer.h"

class TopTrackMapCnv_p1:public TopInputMapCnv_p1<Rec::TrackParticleContainer>{
 public:
    TopTrackMapCnv_p1();
    ~TopTrackMapCnv_p1(){;}
};

#endif // TOP_TRACK_MAP_CNV_P1_H 
