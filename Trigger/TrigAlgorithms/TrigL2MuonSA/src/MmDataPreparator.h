/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MMDATAPREPARATOR_H
#define  TRIGL2MUONSA_MMDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "IRegionSelector/IRegSelTool.h"
#include "MmData.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

namespace TrigL2MuonSA {

  class MmDataPreparator: public AthAlgTool
  {
  public:

    MmDataPreparator(const std::string& type,
		     const std::string& name,
		     const IInterface*  parent);

    virtual StatusCode initialize() override;

    StatusCode prepareData(const TrigRoiDescriptor* p_roids,
			   TrigL2MuonSA::MmHits&   mmHits) const;

    void setRoIBasedDataAccess(bool use_RoIBasedDataAccess) {m_use_RoIBasedDataAccess = use_RoIBasedDataAccess;};

  private:

    ToolHandle<IRegSelTool> m_regionSelector;
    // handles to the RoI driven data access
    SG::ReadHandleKey<Muon::MMPrepDataContainer> m_mmPrepContainerKey{
      this, "MmPrepDataContainer", "MM_Measurements", "Name of the MMContainer to read in"};

    bool m_use_RoIBasedDataAccess;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  };

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_MMDATAPREPARATOR_H
