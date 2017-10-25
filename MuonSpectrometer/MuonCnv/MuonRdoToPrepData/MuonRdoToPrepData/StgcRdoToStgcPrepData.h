/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCRDOTOSTGCPREPDATA_H
#define STGCRDOTOSTGCPREPDATA_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"

class StgcRdoToStgcPrepData : public AthAlgorithm {

public:

  StgcRdoToStgcPrepData(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  bool m_seededDecoding;
  ToolHandle< Muon::IMuonRdoToPrepDataTool >    m_tool; //!< Tool used to do actual decoding.
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
  ServiceHandle<IRegSelSvc> m_regionSelector; //<! pointer to RegionSelectionSvc
  SG::WriteHandleKey<Muon::sTgcPrepDataContainer> m_prdContainer;
};

#endif /// TGCRDOTOSTGCPREPDATA_H


