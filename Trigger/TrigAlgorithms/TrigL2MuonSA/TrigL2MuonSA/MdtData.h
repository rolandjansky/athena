/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MDTDATA_H
#define  TRIGL2MUONSA_MDTDATA_H
#include "Identifier/Identifier.h"

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

  struct MdtHitData
  {
    unsigned int name;
    int isEndcap;
    int StationEta;
    int StationPhi;
    int Multilayer;
    int Layer;
    int TubeLayer;
    int Tube;
    int Chamber;
    //
    double cYmid;
    double cXmid;
    double cAmid;
    double cPhip;
    double cInCo;
    double cPhi0;
    char   cType[4];
    //
    double Z;
    double R;
    double DriftTime;
    double DriftSpace;
    double DriftSigma;
    uint32_t OnlineId;
    uint16_t Adc;
    uint16_t LeadingCoarseTime;
    uint16_t LeadingFineTime;
    uint16_t TrailingCoarseTime;
    uint16_t TrailingFineTime;
    //
    double Residual;
    int    isOutlier;
    Identifier Id;
};
  
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

typedef std::vector<MdtHitData>  MdtHits;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

}

#endif  // TRIGL2MUONSA_MDTDATA_H
