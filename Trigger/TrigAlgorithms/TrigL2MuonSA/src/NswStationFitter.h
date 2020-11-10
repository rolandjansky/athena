/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_NSWSTATIONFITTER_H
#define  TRIGL2MUONSA_NSWSTATIONFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

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

    StatusCode superPointFitter(const LVL1::RecMuonRoI* p_roi,
				TrigL2MuonSA::TrackPattern& trackPattern) const;

    StatusCode selectStgcHits(const LVL1::RecMuonRoI* p_roi,
			      TrigL2MuonSA::StgcHits& stgcHits) const;

    StatusCode selectMmHits(const LVL1::RecMuonRoI* p_roi,
			    TrigL2MuonSA::MmHits& mmHits) const;

    StatusCode calcWeightedSumHit(TrigL2MuonSA::TrackPattern& trackPattern) const;

  private :

  };

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_NSWSTATIONFITTER_H
