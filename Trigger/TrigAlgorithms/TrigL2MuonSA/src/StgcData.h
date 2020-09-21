/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_STGCDATA_H
#define  TRIGL2MUONSA_STGCDATA_H

namespace TrigL2MuonSA {

  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------

  class StgcHitData
  {
  public:
  StgcHitData() :
    eta(0),
      phi(0),
      r(0),
      z(0),
      stationEta(0),
      stationPhi(0),
      stationName(0),
      ResidualR(999.),
      ResidualPhi(999.),
      isOutlier(0),
      layerNumber(999),
      channelType(-1)
	{};

  public:
    double eta;
    double phi;
    double r;
    double z;
    int    stationEta;
    int    stationPhi;
    int    stationName;
    double ResidualR;
    double ResidualPhi;
    int    isOutlier;
    unsigned int layerNumber;
    int channelType;
  };

  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------

  typedef std::vector<StgcHitData>  StgcHits;

  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------

}

#endif  // TRIGL2MUONSA_STGCDATA_H
