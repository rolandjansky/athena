/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONCOMMON_ISOLATIONCOMMON_H
#define ISOLATIONCOMMON_ISOLATIONCOMMON_H

#include "xAODPrimitives/IsolationCorrection.h"

#include <vector>

namespace xAOD {
  
  struct CaloCorrection {
    Iso::IsolationCaloCorrectionBitset calobitset;
  };

  struct TrackCorrection {
    Iso::IsolationTrackCorrectionBitset trackbitset;
  };

  struct CaloIsolation {
    // bit mask of corrections applied to returned isolation
    CaloCorrection corrlist;
    // core corrections
    std::map<Iso::IsolationCaloCorrection,std::map<Iso::IsolationCorrectionParameter,float>> coreCorrections;
    // pt/pileup corrections (map index: IsolationCaloCorrection; vector index: cone size)
    std::map<Iso::IsolationCaloCorrection,std::vector<float>> noncoreCorrections;
    // isolation energy with corrections
    std::vector<float> etcones;
  };

  struct TrackIsolation {
    // bit mask of corrections applied to returned isolation
    TrackCorrection corrlist;
    std::vector<float> ptcones;
    std::vector<float> ptvarcones_10GeVDivPt;
    // sum of pT subtracted from the track isolation
    std::map<Iso::IsolationTrackCorrection,float> coreCorrections;
  };

}


#endif 
