/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_CSCDATAPREPARATOR_H
#define  TRIGL2MUONSA_CSCDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "RegionSelector/IRegSelSvc.h"
#include "TgcData.h"
#include "CscData.h"
#include "RecMuonRoIUtils.h"
#include "MuonRoad.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "CscClusterization/ICscClusterBuilder.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonSA {
  
  class CscDataPreparator: public AthAlgTool
  {
  public:

    CscDataPreparator(const std::string& type,
		      const std::string& name,
		      const IInterface*  parent);

    virtual StatusCode initialize() override;
    
  public:

    StatusCode prepareData(const TrigRoiDescriptor* p_roids,
			   TrigL2MuonSA::MuonRoad&  muonRoad,
			   TrigL2MuonSA::CscHits&   cscHits);

    void setRoIBasedDataAccess(bool use_RoIBasedDataAccess) {m_use_RoIBasedDataAccess = use_RoIBasedDataAccess;};

  private:

    double calc_residual(double aw,
			 double bw,
			 double x,
			 double y);

    double calc_residual_phi(double aw, double bw, double phiw, double hitphi, double hitz);

  private:

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    // Region Selector
    ServiceHandle<IRegSelSvc>  m_regionSelector;

    // Tool handles for BS conversion and Rdo to Prep Data conversion
    ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataProviderTool{
      this, "CscRawDataProvider", "Muon::CSC_RawDataProviderTool/CSC_RawDataProviderTool"};

    // CSC PrepDataProvider
    ToolHandle<Muon::IMuonRdoToPrepDataTool> m_cscPrepDataProvider{
      this, "CscPrepDataProvider", "Muon::CscRdoToCscPrepDataTool/CscPrepDataProviderTool"};
    ToolHandle<ICscClusterBuilder> m_cscClusterProvider{
      this, "CscClusterProvider", "CscThresholdClusterBuilderTool"};

    SG::ReadHandleKey<Muon::CscPrepDataContainer> m_cscPrepContainerKey{ this, "CSCPrepDataContainer", "CSC_Clusters", "Name of the CSCContainer to read in"};

    // Flag to decide if we need to run the actual decoding (in MT setup, we can use offline code for this)
    Gaudi::Property<bool> m_doDecoding{ this, "DoDecoding", true, "Flag to decide if we need to do decoding of the CSCs" };

    // Flag to decide whether or not to run BS decoding
    Gaudi::Property< bool > m_decodeBS { this, "DecodeBS", true, "Flag to decide whether or not to run BS->RDO decoding" };

    bool m_use_RoIBasedDataAccess;
  };

} // namespace TrigL2MuonSA

#endif
