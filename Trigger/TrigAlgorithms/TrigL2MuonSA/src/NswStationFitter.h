/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_NSWSTATIONFITTER_H
#define  TRIGL2MUONSA_NSWSTATIONFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "StgcData.h"
#include "MmData.h"
#include "TrackData.h"

namespace TrigL2MuonSA {

  class NswStationFitter: public AthAlgTool
  {
  public:

  public:

    NswStationFitter(const std::string& type,
		     const std::string& name,
		     const IInterface*  parent);

    StatusCode superPointFitter(const TrigRoiDescriptor* p_roids,
				TrigL2MuonSA::TrackPattern& trackPattern,
				TrigL2MuonSA::StgcHits& stgcHits,
				TrigL2MuonSA::MmHits& mmHits) const;

    StatusCode selectStgcHits(const TrigRoiDescriptor* p_roids,
			      TrigL2MuonSA::StgcHits& stgcHits) const;

    StatusCode selectMmHits(const TrigRoiDescriptor* p_roids,
			    TrigL2MuonSA::MmHits& mmHits) const;

    StatusCode calcWeightedSumHit(TrigL2MuonSA::TrackPattern& trackPattern) const;

    StatusCode findStgcHitsInSegment(TrigL2MuonSA::StgcHits& stgcHits) const;

    void findSetOfStgcHitIds(TrigL2MuonSA::StgcHits& stgcHits,
                             std::array<std::vector<int>,8> hitIdByLayer,
                             std::vector<std::array<int, 8>>& hitIdsCandidate) const;

    StatusCode findMmHitsInSegment(TrigL2MuonSA::MmHits& mmHits) const;

    void findSetOfMmHitIds(TrigL2MuonSA::MmHits& mmHits,
                           std::array<std::vector<int>,8> hitIdByLayer,
                           std::vector<std::array<int, 8>>& hitIdsCandidate) const;

    StatusCode MakeSegment(TrigL2MuonSA::TrackPattern& trackPattern, 
		                       TrigL2MuonSA::StgcHits& stgcHits) const;
    StatusCode MakeSegment(TrigL2MuonSA::TrackPattern& trackPattern, 
		                       TrigL2MuonSA::MmHits& mmHits) const;
		
    StatusCode calcMergedHit(TrigL2MuonSA::TrackPattern& trackPattern) const;

    void LinearFit(std::vector<double>& x,std::vector<double>& y,
                   double* slope, double* intercept, double* mse) const;
    void LinearFitWeight(std::vector<double>& x,std::vector<double>& y,std::vector<bool>& isStgc,
                   double* slope, double* intercept, double* mse, double eta) const;

    void getNswResolution(double *stgcDeltaR, double *mmDeltaR, unsigned int size) const;

  private :

  };

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_NSWSTATIONFITTER_H
