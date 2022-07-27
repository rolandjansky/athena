/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCRDOTOSTGCPREPDATA_H
#define STGCRDOTOSTGCPREPDATA_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelTool.h"

class StgcRdoToStgcPrepData : public AthAlgorithm {

public:

  StgcRdoToStgcPrepData(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();

private:
  ToolHandle<Muon::IMuonRdoToPrepDataTool> m_decoderTool{this,"DecodingTool","Muon::sTgcRdoToPrepDataTool/STGC_PrepDataProviderTool"};
  SG::WriteHandleKey<Muon::sTgcPrepDataContainer> m_prdContainer;

  // Handle for region selector service
  ToolHandle<IRegSelTool> m_regsel_stgc{this, "RegionSelectorTool", "RegSelTool/RegSelTool_STGC", "sTGC Region Selector Tool"};

  /// Property to decide whether or not to do RoI based decoding
  Gaudi::Property< bool > m_seededDecoding { this, "DoSeededDecoding", false, "If true do decoding in RoIs"};

  /// ReadHandle for the input RoIs
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{ this, "RoIs", "OutputRoIs",  "Name of RoI collection to read in" };
};

#endif /// TGCRDOTOSTGCPREPDATA_H


