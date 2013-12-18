/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBEXTENDEDTRACKOVERLAP_H
#define MUONCALIBEXTENDEDTRACKOVERLAP_H


namespace MuonCalib {
  
  struct MuonCalibExtendedTrackOverlap {
    MuonCalibExtendedTrackOverlap() : sharedMdtWrongSign(0) {}

    struct TechnologyOverlap {
      TechnologyOverlap() : shared(0),first(0),second(0) {}
      unsigned int shared;
      unsigned int first;
      unsigned int second;
    };

    unsigned int      sharedMdtWrongSign;
    TechnologyOverlap mdt;
    TechnologyOverlap rpcEta;
    TechnologyOverlap rpcPhi;
    TechnologyOverlap tgcEta;
    TechnologyOverlap tgcPhi;
    TechnologyOverlap cscEta;
    TechnologyOverlap cscPhi;

    unsigned int sharedPrecisionHits() const { return mdt.shared + cscEta.shared; }
    unsigned int sharedEtaTriggerHits() const { return rpcEta.shared + tgcEta.shared; }
    unsigned int sharedPhiHits() const { return rpcPhi.shared + tgcPhi.shared + cscPhi.shared; }

    unsigned int firstPrecisionHits() const { return mdt.first + cscEta.first + sharedPrecisionHits(); }
    unsigned int firstEtaTriggerHits() const { return rpcEta.first + tgcEta.first + sharedEtaTriggerHits(); }
    unsigned int firstPhiHits() const { return rpcPhi.first + tgcPhi.first + cscPhi.first + sharedPhiHits(); }

    unsigned int secondPrecisionHits() const { return mdt.second + cscEta.second  + sharedPrecisionHits(); }
    unsigned int secondEtaTriggerHits() const { return rpcEta.second + tgcEta.second  + sharedEtaTriggerHits(); }
    unsigned int secondPhiHits() const { return rpcPhi.second + tgcPhi.second + cscPhi.second + sharedPhiHits(); }

    double overlapFracionPrecision() const;
    double overlapFracionEtaTrigger() const;
    double overlapFracionPhi() const;
  };

  inline double MuonCalibExtendedTrackOverlap::overlapFracionPrecision() const {
    unsigned int shortest = firstPrecisionHits() < secondPrecisionHits() ? firstPrecisionHits() : secondPrecisionHits();
    if( shortest == 0 ) return 0;
    return (double)sharedPrecisionHits()/(double)shortest;
  }

  inline double MuonCalibExtendedTrackOverlap::overlapFracionEtaTrigger() const {
    unsigned int shortest = firstEtaTriggerHits() < secondEtaTriggerHits() ? firstEtaTriggerHits() : secondEtaTriggerHits();
    if( shortest == 0 ) return 0;
    return (double)sharedEtaTriggerHits()/(double)shortest;
  }

  inline double MuonCalibExtendedTrackOverlap::overlapFracionPhi() const {
    unsigned int shortest = firstPhiHits() < secondPhiHits() ? firstPhiHits() : secondPhiHits();
    if( shortest == 0 ) return 0;
    return (double)sharedPhiHits()/(double)shortest;
  }


}

#endif
