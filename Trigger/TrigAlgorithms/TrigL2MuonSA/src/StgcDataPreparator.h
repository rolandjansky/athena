/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_STGCDATAPREPARATOR_H
#define  TRIGL2MUONSA_STGCDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "IRegionSelector/IRegSelTool.h"
#include "StgcData.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

namespace TrigL2MuonSA {

  class StgcDataPreparator: public AthAlgTool
  {
  public:

    StgcDataPreparator(const std::string& type,
		       const std::string& name,
		       const IInterface*  parent);

    virtual StatusCode initialize() override;

    StatusCode prepareData(const TrigRoiDescriptor* p_roids,
			   TrigL2MuonSA::StgcHits& stgcHits) const;

    void setRoIBasedDataAccess(bool use_RoIBasedDataAccess) {m_use_RoIBasedDataAccess = use_RoIBasedDataAccess;};

  private:

    ToolHandle<IRegSelTool> m_regionSelector;
    // handles to the RoI driven data access
    SG::ReadHandleKey<Muon::sTgcPrepDataContainer> m_stgcPrepContainerKey{
      this, "StgcPrepDataContainer", "STGC_Measurements", "Name of the STGCContainer to read in"};

    bool m_use_RoIBasedDataAccess;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  };

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_STGCDATAPREPARATOR_H
