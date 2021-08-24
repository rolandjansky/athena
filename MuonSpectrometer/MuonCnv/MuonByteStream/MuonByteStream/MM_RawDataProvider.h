/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MM_RAWDATAPROVIDER_H
#define MUONBYTESTREAM_MM_RAWDATAPROVIDER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "IRegionSelector/IRegSelTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

namespace Muon {

class MM_RawDataProvider : public AthReentrantAlgorithm
{
public:

  //! Constructor.
  MM_RawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  virtual StatusCode initialize() override;

  //! Execute
  virtual StatusCode execute(const EventContext& ctx) const override;

  //! Destructur
  ~MM_RawDataProvider() = default;


private:
  /// Tool handle for raw data provider tool
  ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataTool{this, "ProviderTool", "Muon::MM_RawDataProviderToolMT/MmRawDataProviderTool"};

  /// Handle for region selector service
  ToolHandle<IRegSelTool> m_regsel_mm{this, "RegionSelectionTool", "RegSelTool/RegSelTool_MM", "MM Region Selector Tool"};

  /// Property to decide whether or not to do RoI based decoding
  Gaudi::Property< bool > m_seededDecoding { this, "DoSeededDecoding", false, "If true do decoding in RoIs"};

  /// ReadHandle for the input RoIs
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{ this, "RoIs", "OutputRoIs",  "Name of RoI collection to read in" };

    
};
} // ns end

#endif



