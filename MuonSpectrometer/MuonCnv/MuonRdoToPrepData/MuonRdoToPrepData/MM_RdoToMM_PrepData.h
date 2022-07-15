/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_RDOTOMM_PREPDATA_H
#define MM_RDOTOMM_PREPDATA_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelTool.h"


class MM_RdoToMM_PrepData : public AthAlgorithm {

public:

  MM_RdoToMM_PrepData(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();

private:

  ToolHandle<Muon::IMuonRdoToPrepDataTool> m_tool{this,"DecodingTool","Muon::MmRdoToPrepDataTool/MM_PrepDataProviderTool"};
  
  /// Handle for region selector service
  ToolHandle<IRegSelTool> m_regsel_mm{this, "RegionSelectorTool", "RegSelTool/RegSelTool_MM", "MM Region Selector Tool"};

  /// Property to decide whether or not to do RoI based decoding
  Gaudi::Property< bool > m_seededDecoding { this, "DoSeededDecoding", false, "If true do decoding in RoIs"};

  /// ReadHandle for the input RoIs
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{ this, "RoIs", "OutputRoIs",  "Name of RoI collection to read in" };

  bool                                    m_print_inputRdo; //!<< If true, will dump information about the input RDOs.
  bool                                    m_print_prepData; //!<< If true, will dump information about the resulting PRDs.

};

#endif /// CSCRDOTOCSCPREPDATA_H

