/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTRAWDATAPROVIDER_H
#define MUONBYTESTREAM_MDTRAWDATAPROVIDER_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// interface to region selector service
#include "IRegionSelector/IRegSelSvc.h"

// ROI Descriptor classes
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "MuonAlignmentData/CorrContainer.h"

namespace Muon {
class IMuonRawDataProviderTool;

class MdtRawDataProvider : public AthAlgorithm
{
public:

  //! Constructor.
  MdtRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  virtual StatusCode initialize();

  //! Execute
  virtual StatusCode execute();

  //! Finalize
  virtual StatusCode finalize();

  //! Destructur
  ~MdtRawDataProvider();


private:
  /// Tool handle for raw data provider tool
  ToolHandle<Muon::IMuonRawDataProviderTool>  m_rawDataTool;

  /// Handle for region selector service
  ServiceHandle<IRegSelSvc> m_regionSelector;

  /// Property to decide whether or not to do RoI based decoding
  Gaudi::Property< bool > m_seededDecoding { this, "DoSeededDecoding", false, "If true do decoding in RoIs"};

  /// ReadHandle for the input RoIs
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{ this, "RoIs", "OutputRoIs",  "Name of RoI collection to read in" };

  SG::ReadCondHandleKey<ALineMapContainer> m_ALineKey{this, "ALineMapContainer", "ALineMapContainer", "Name of muon alignment ALine condition data"};
    
};
} // ns end

#endif



