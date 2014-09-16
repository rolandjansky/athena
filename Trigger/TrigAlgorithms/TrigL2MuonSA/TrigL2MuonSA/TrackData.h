/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TRACKDATA_H
#define  TRIGL2MUONSA_TRACKDATA_H

#include "TrigL2MuonSA/MdtData.h"
#include "TrigL2MuonSA/SuperPointData.h"

#define NCHAMBER 8

namespace TrigL2MuonSA {

class TrackPattern
{
 public:
 TrackPattern() :
   s_address(-1),
   phiDir(0),
   phi(0),
   etaMap(0),
   phiMap(0),
   etaBin(0),
   phiBin(0),
   phiBin24(0),
   smallLarge(-1),
   radius(0),
   sagitta(0),
   alpha(0),
   beta(0),
   endcapRadius(0),
   slope(0),
   intercept(0),
   deltaR(0),
   pt(0),
   ptAlpha(0),
   ptBeta(0),
   ptRadius(0),
   charge(0),
   etaVtx(0),
   phiVtx(0),
   isRpcFailure(false),
   isTgcFailure(false)
     {};
   
    ~TrackPattern() {};
    
 public:
    TrigL2MuonSA::MdtHits    mdtSegments[NCHAMBER]; // MDT hits associated with the track
    TrigL2MuonSA::SuperPoint superPoints[NCHAMBER]; // Super points at each station
    
    int    s_address;

    double phiDir;
    double phi;

    double etaMap;
    double phiMap;  

    int    etaBin;
    int    phiBin;  
    int    phiBin24; 
    int    smallLarge;

    double radius;
    double sagitta;

    double alpha;
    double beta;
    double endcapRadius;

    double slope;
    double intercept;
    double deltaR;

    double pt;
    double ptAlpha;
    double ptBeta;
    double ptRadius;
    double charge;
    
    double etaVtx;
    double phiVtx;

    bool   isRpcFailure;
    bool   isTgcFailure;
};
 
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

}

#endif  // TRIGL2MUONSA_TRACKDATA_H
