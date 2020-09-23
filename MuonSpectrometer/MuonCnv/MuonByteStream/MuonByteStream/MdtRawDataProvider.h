/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTRAWDATAPROVIDER_H
#define MUONBYTESTREAM_MDTRAWDATAPROVIDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "IRegionSelector/IRegSelSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

namespace Muon {

class MdtRawDataProvider : public AthAlgorithm
{
public:

  //! Constructor.
  MdtRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  virtual StatusCode initialize();

  //! Execute
  virtual StatusCode execute();

  //! Destructur
  ~MdtRawDataProvider()=default;


private:
  /// Tool handle for raw data provider tool
  ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataTool{this,"ProviderTool","Muon::MDT_RawDataProviderToolMT/MdtRawDataProviderTool"};

  /// Handle for region selector service
  ServiceHandle<IRegSelSvc> m_regionSelector;

  /// Property to decide whether or not to do RoI based decoding
  Gaudi::Property< bool > m_seededDecoding { this, "DoSeededDecoding", false, "If true do decoding in RoIs"};

  /// ReadHandle for the input RoIs
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{ this, "RoIs", "OutputRoIs",  "Name of RoI collection to read in" };

    
};
} // ns end

#endif



