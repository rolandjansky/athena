/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDER_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "InDetRawData/InDetTimeCollection.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "IRegionSelector/IRegSelSvc.h" 
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class ISCTRawDataProviderTool;
class ISCT_CablingTool;
class SCT_ID;
class SCT_ByteStreamFractionContainer;

/** 
 * @class SCTRawDataProvider
 *
 * @brief Athena Algorithm to decode the SCT Byte Stream
 *
 * Gets vector of ROBFragments from the ROBDataProviderSvc
 * and uses AlgTools (SCTRawDataProviderTool and SCT_RodDecoder
 * to read the ByteStream and make RDOs.
 * Output is one RDO container (IDC) per event, which contains
 * one Collection per link (8196 in total), each of which contains
 * RDOs for hits (one RDO per strip in expanded mode, one per cluster
 * in condensed mode).
 *
 * Class based on TRT equivalent.
 */
class SCTRawDataProvider : public AthAlgorithm
{
 public:

  /** Constructor */
  SCTRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  /** Destructor */
  virtual ~SCTRawDataProvider() = default;

  /** Initialize */
  virtual StatusCode initialize() override;

  /** Execute */
  virtual StatusCode execute() override;

  // Finalize empty, no need to override

 private:

  ServiceHandle<IRegSelSvc> m_regionSelector;     

  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

  ToolHandle<ISCTRawDataProviderTool> m_rawDataTool{this, 
                                                    "ProviderTool", 
                                                    "SCTRawDataProviderTool", 
                                                    "SCT  Raw Data Provider Tool"};

  ToolHandle<ISCT_CablingTool> m_cabling{this, 
                                         "SCT_CablingTool", 
                                         "SCT_CablingTool", 
                                         "Tool to retrieve SCT Cabling"};

  const SCT_ID* m_sctID; 

  BooleanProperty m_roiSeeded{this, "isRoI_Seeded", false, "Use RoI"};

  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{this, 
                                                                    "RoIs",   
                                                                    "", 
                                                                    "RoIs to read in"};

  SG::WriteHandleKey<SCT_RDO_Container> m_rdoContainerKey{this, 
                                                          "RDOKey", 
                                                          "SCT_RDOs", 
                                                          "SCT RDO key"};

  SG::WriteHandleKey<InDetTimeCollection> m_lvl1CollectionKey{this, 
                                                              "LVL1IDKey", 
                                                              "SCT_LVL1ID", 
                                                              "SCT LVL1ID key"};

  SG::WriteHandleKey<InDetTimeCollection> m_bcIDCollectionKey{this, 
                                                              "BCIDKey", 
                                                              "SCT_BCID", 
                                                              "SCT BCID key"};

  SG::WriteHandleKey<InDetBSErrContainer> m_bsErrContainerKey{this, 
                                                              "ByteStreamErrContainer", 
                                                              "SCT_ByteStreamErrs", 
                                                              "SCT BS error key"};

  SG::WriteHandleKey<SCT_ByteStreamFractionContainer> m_bsFracContainerKey{this, 
                                                                           "ByteStreamFracContainer", 
                                                                           "SCT_ByteStreamFrac", 
                                                                           "SCT BS fraction key"};
  SG::UpdateHandleKey<SCT_RDO_Cache> m_rdoContainerCacheKey;
};

#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDER_H
