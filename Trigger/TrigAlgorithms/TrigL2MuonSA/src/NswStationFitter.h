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
				TrigL2MuonSA::TrackPattern& trackPattern) const;

    StatusCode selectStgcHits(const TrigRoiDescriptor* p_roids,
			      TrigL2MuonSA::StgcHits& stgcHits) const;

    StatusCode selectMmHits(const TrigRoiDescriptor* p_roids,
			    TrigL2MuonSA::MmHits& mmHits) const;

    StatusCode calcWeightedSumHit(TrigL2MuonSA::TrackPattern& trackPattern) const;

  private :

  };

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_NSWSTATIONFITTER_H
