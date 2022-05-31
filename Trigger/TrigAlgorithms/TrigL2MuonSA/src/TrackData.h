/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TRACKDATA_H
#define  TRIGL2MUONSA_TRACKDATA_H

#include "MdtData.h"
#include "MmData.h"
#include "StgcData.h"
#include "SuperPointData.h"

namespace TrigL2MuonSA {

class TrackPattern
{
 public:
 TrackPattern() :
   pt(0),
   charge(0),
   etaVtx(99999.),
   phiVtx(99999.),
   deltaPt(99999.),
   deltaEtaVtx(99999.),
   deltaPhiVtx(99999.),
   s_address(99999),
   phiMS(99999.),
   phiMSDir(99999.),
   etaMap(99999.),
   phiMap(99999.),
   etaBin(99999),
   phiBin(99999),
   phiBinEE(99999),
   barrelRadius(0),
   barrelSagitta(99999.),
   endcapAlpha(99999.),
   endcapBeta(99999.),
   endcapRadius(0),
   endcapRadius3P(0),
   cscGamma(0),
   slope(0),
   intercept(0),
   deltaR(0),
   ptEndcapAlpha(0),
   ptEndcapBeta(0),
   ptEndcapRadius(0),
   ptCSC(0),
   isRpcFailure(false),
   isTgcFailure(false),
   large_dPhidZ(false),
   outerCorFactor(0),
   hashID_CSC(999)
   {};
   
 public:
    static constexpr int s_NCHAMBER = 11;
    TrigL2MuonSA::MdtHits    mdtSegments[s_NCHAMBER]; // MDT hits associated with the track
    TrigL2MuonSA::MmHits     mmSegment; // MM hits associated with the track
    TrigL2MuonSA::StgcHits   stgcSegment; // sTGC hits associated with the track
    TrigL2MuonSA::SuperPoint superPoints[s_NCHAMBER]; // Super points at each station

    double pt;
    double charge;

    double etaVtx;
    double phiVtx;

    double deltaPt;
    double deltaEtaVtx;
    double deltaPhiVtx;

    int    s_address;

    double phiMS;
    double phiMSDir;

    double etaMap;
    double phiMap;  

    int    etaBin;
    int    phiBin;  
    int    phiBinEE; 

    double barrelRadius;
    double barrelSagitta;

    double endcapAlpha;
    double endcapBeta;
    double endcapRadius;
    double endcapRadius3P;
    double cscGamma;

    double slope;
    double intercept;
    double deltaR;

    double ptEndcapAlpha;
    double ptEndcapBeta;
    double ptEndcapRadius;
    double ptCSC;
    
    bool   isRpcFailure;
    bool   isTgcFailure;

    //relevant to CSC pt
    bool large_dPhidZ;
    double outerCorFactor;
    int hashID_CSC;
};
 
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

}

#endif  // TRIGL2MUONSA_TRACKDATA_H
