/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_NSWPATTERNFINDER_H
#define  TRIGL2MUONSA_NSWPATTERNFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonRoad.h"
#include "StgcData.h"
#include "MmData.h"
#include "TrackData.h"

namespace TrigL2MuonSA {

 class NswPatternFinder: public AthAlgTool
 {
 public:

 public:

   NswPatternFinder(const std::string& type,
		    const std::string& name,
		    const IInterface*  parent);

   StatusCode findPatterns(const TrigL2MuonSA::MuonRoad& muonRoad,
			   TrigL2MuonSA::StgcHits&       stgcHits,
			   TrigL2MuonSA::MmHits&         mmHits,
			   TrigL2MuonSA::TrackPattern&   trackPattern) const;

 private :
   double calc_residualR(const double aw,const double bw,const double x,const double y) const;
   double calc_dPhi(const double hitphi, const double roadphi) const;

   StatusCode outlierMm  (const TrigL2MuonSA::MuonRoad& muonRoad, TrigL2MuonSA::TrackPattern& trackPattern, TrigL2MuonSA::MmHits& mmHits) const;
   StatusCode outlierStgc(const TrigL2MuonSA::MuonRoad& muonRoad, TrigL2MuonSA::TrackPattern& trackPattern, TrigL2MuonSA::StgcHits& stgcHits) const;


 };

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_NSWPATTERNFINDER_H
