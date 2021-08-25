/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_STGCRAWDATAPROVIDER_H
#define MUONBYTESTREAM_STGCRAWDATAPROVIDER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "IRegionSelector/IRegSelTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

namespace Muon
{
  class sTgcRawDataProvider : public AthReentrantAlgorithm
  {
    public:
      //! Constructor.
      sTgcRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

      //! Initialize
      virtual StatusCode initialize() override;

      //! Execute
      virtual StatusCode execute(const EventContext& ctx) const override;

      //! Destructor
      ~sTgcRawDataProvider( ) =default;

    private:

      /// Handle for the RawDataProviderTool
      ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataTool{this, "ProviderTool", "Muon::STGC_RawDataProviderToolMT/sTgcRawDataProviderTool"};

      /// Handle for region selector service
      ToolHandle<IRegSelTool> m_regsel_stgc{this, "RegionSelectionTool", "RegSelTool/RegSelTool_STGC", "sTGC Region Selector Tool"};

      /// Property to decide whether or not to do RoI based decoding
      Gaudi::Property< bool > m_seededDecoding { this, "DoSeededDecoding", false, "If true do decoding in RoIs"};

      /// ReadHandle for the input RoIs
      SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{ this, "RoIs", "OutputRoIs",  "Name of RoI collection to read in" };
  };
} // namespace end

#endif



