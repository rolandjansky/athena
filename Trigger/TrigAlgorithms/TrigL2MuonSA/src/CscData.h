/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_CSCDATA_H
#define  TRIGL2MUONSA_CSCDATA_H

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

  struct CscHitData
  {
    unsigned int StationName;
    int StationEta;
    int StationPhi;
    int ChamberLayer;
    int WireLayer;
    int MeasuresPhi;
    int Strip;
    int Chamber;
    uint32_t StripId;
    //
    double eta;
    double phi;
    double r;
    double z;
    int    charge;
    double time;
    float resolution;
    //
    double Residual;
    int    isOutlier;
};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

typedef std::vector<CscHitData>  CscHits;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

}

#endif  // TRIGL2MUONSA_CSCDATA_H
