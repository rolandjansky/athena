/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_TGCRAWDATAPROVIDER_H
#define MUONBYTESTREAM_TGCRAWDATAPROVIDER_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "IRegionSelector/IRegSelTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

namespace Muon
{

class TgcRawDataProvider : public AthReentrantAlgorithm
{
public:

    //! Constructor.
    TgcRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

    //! Initialize
    virtual StatusCode initialize();

    //! Execute
    virtual StatusCode execute(const EventContext& ctx) const;

    //! Destructur
    ~TgcRawDataProvider()=default;

private:

    ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataTool{this,"ProviderTool","Muon::TGC_RawDataProviderToolMT/TgcRawDataProviderTool"};

    /// Handle for region selector service
    ToolHandle<IRegSelTool> m_regsel_tgc{this, "RegionSelectionTool", "RegSelTool/RegSelTool_TGC", "TGC Region Selector Tool"};

    /// Property to decide whether or not to do RoI based decoding
    Gaudi::Property< bool > m_seededDecoding { this, "DoSeededDecoding", false, "If true do decoding in RoIs"};

    /// ReadHandle for the input RoIs
    SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{ this, "RoIs", "OutputRoIs",  "Name of RoI collection to read in" };

};
} // ns end

#endif



