/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
/// Top algorithm to decode the SCT BS
/// copied from TRT equivalent
/// Gets vector of ROBFragments from the ROBDataProviderSvc
/// and uses AlgTools (SCTRawDataProviderTool and SCT_RodDecoder
/// to read the ByteStream and make RDOs.
/// Output is one RDO container (IDC) per event, which contains
/// one Collection per link (8196 in total), each of which contains
/// RDOs for hits (one RDO per strip in expanded mode, one per cluster
/// in condensed mode).
///////////////////////////////////////////////////////////////////

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDER_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDER_H

/** Base class */
#include "AthenaBaseComps/AthAlgorithm.h"

/** Gaudi */
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
/** other */
#include "InDetRawData/InDetTimeCollection.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "IRegionSelector/IRegSelSvc.h" 
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

/** forward declarations */
class ISCTRawDataProviderTool;
class ISCT_CablingSvc;
class SCT_ID;

class SCTRawDataProvider : public AthAlgorithm
{
 public:

  //! Constructor.
  SCTRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  //! Destructur
  ~SCTRawDataProvider() {
  }

  //! Initialize
  virtual StatusCode initialize() override;

  //! Execute
  virtual StatusCode execute() override;

  //! Finalize empty, no need to override

 private:

  ServiceHandle<IRegSelSvc> m_regionSelector;     
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  ToolHandle<ISCTRawDataProviderTool> m_rawDataTool{this, "ProviderTool", "SCTRawDataProviderTool", "SCT  Raw Data Provider Tool"};
  ServiceHandle<ISCT_CablingSvc> m_cabling;
  const SCT_ID* m_sct_id; 
  bool m_roiSeeded;
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
  SG::WriteHandleKey<SCT_RDO_Container> m_rdoContainerKey;
  SG::WriteHandleKey<InDetTimeCollection> m_lvl1CollectionKey;
  SG::WriteHandleKey<InDetTimeCollection> m_bcidCollectionKey;
  SG::WriteHandleKey<InDetBSErrContainer> m_bsErrContainerKey;
  SG::UpdateHandleKey<SCT_RDO_Cache> m_rdoContainerCacheKey;
};

#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDER_H
