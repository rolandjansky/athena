/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MMDATAPREPARATOR_H
#define  TRIGL2MUONSA_MMDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "IRegionSelector/IRegSelTool.h"
#include "MmData.h"

#include "RecMuonRoIUtils.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
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
			   TrigL2MuonSA::MmHits&   mmHits);

    void setRoIBasedDataAccess(bool use_RoIBasedDataAccess) {m_use_RoIBasedDataAccess = use_RoIBasedDataAccess;};

  private:

    ToolHandle<IRegSelTool> m_regionSelector;
    // handles to the RoI driven data access
    /* ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataProviderTool{ */
    /*   this, "MmRawDataProvider", "Muon::MM_RawDataProviderTool/MM_RawDataProviderTool"}; */

    ToolHandle<Muon::IMuonRdoToPrepDataTool> m_mmPrepDataProvider{
      this, "MmPrepDataProvider", "Muon::MmRdoToPrepDataTool/MmPrepDataProviderTool"};

    SG::ReadHandleKey<Muon::MMPrepDataContainer> m_mmPrepContainerKey{
      this, "MmPrepDataContainer", "MM_Measurements", "Name of the MMContainer to read in"};

    bool m_use_RoIBasedDataAccess;

    // Flag to decide if we need to run the actual decoding (in MT setup, we can use offline code for this)
    Gaudi::Property<bool> m_doDecoding{ this, "DoDecoding", true, "Flag to decide if we need to do decoding of the MDTs" };
    // Flag to decide whether or not to run BS decoding
    Gaudi::Property< bool > m_decodeBS { this, "DecodeBS", true, "Flag to decide whether or not to run BS->RDO decoding" };

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  };

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_MMDATAPREPARATOR_H
