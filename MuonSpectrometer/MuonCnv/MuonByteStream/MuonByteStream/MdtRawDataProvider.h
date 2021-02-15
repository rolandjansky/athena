/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTRAWDATAPROVIDER_H
#define MUONBYTESTREAM_MDTRAWDATAPROVIDER_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "IRegionSelector/IRegSelTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

namespace Muon {

class MdtRawDataProvider : public AthReentrantAlgorithm
{
public:

  //! Constructor.
  MdtRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  virtual StatusCode initialize();

  //! Execute
  virtual StatusCode execute(const EventContext& ctx) const;

  //! Destructur
  ~MdtRawDataProvider()=default;


private:
  /// Tool handle for raw data provider tool
  ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataTool{this,"ProviderTool","Muon::MDT_RawDataProviderToolMT/MdtRawDataProviderTool"};

  /// Handle for region selector service
  ToolHandle<IRegSelTool> m_regsel_mdt{this, "RegionSelectionTool", "RegSelTool/RegSelTool_MDT", "MDT Region Selector Tool"};

  /// Property to decide whether or not to do RoI based decoding
  Gaudi::Property< bool > m_seededDecoding { this, "DoSeededDecoding", false, "If true do decoding in RoIs"};

  /// ReadHandle for the input RoIs
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{ this, "RoIs", "OutputRoIs",  "Name of RoI collection to read in" };

    
};
} // ns end

#endif



