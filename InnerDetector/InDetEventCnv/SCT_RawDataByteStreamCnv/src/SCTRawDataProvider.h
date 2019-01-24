/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDER_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

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
class SCTRawDataProvider : public AthReentrantAlgorithm
{
 public:

  /** Constructor */
  SCTRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  /** Destructor */
  virtual ~SCTRawDataProvider() = default;

  /** Initialize */
  virtual StatusCode initialize() override;

  /** Execute */
  virtual StatusCode execute(const EventContext& ctx) const override;

 private:

  /** Region Selector service for Athena. */
  ServiceHandle<IRegSelSvc> m_regionSelector;     

  /** ROB Data Provider for accessing ROB data. */ 
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

  /** Tool to fill Collections of SCT RDO Containers. */
  ToolHandle<ISCTRawDataProviderTool> m_rawDataTool{this, 
                                                    "ProviderTool", 
                                                    "SCTRawDataProviderTool", 
                                                    "SCT  Raw Data Provider Tool"};

  /** Providing mappings of online and offline identifiers and also serial numbers. */
  ToolHandle<ISCT_CablingTool> m_cabling{this, 
                                         "SCT_CablingTool", 
                                         "SCT_CablingTool", 
                                         "Tool to retrieve SCT Cabling"};

  /** Identifier helper class for the SCT subdetector that creates compact Identifier objects and 
      IdentifierHash or hash IDs. Also allows decoding of these IDs. */ 
  const SCT_ID* m_sctID; 

  /** Boolean to determine if SCT Raw Data Provider should be run in RoI seeded mode. */
  BooleanProperty m_roiSeeded{this, "isRoI_Seeded", false, "Use RoI"};

  /** Read handle for Trigger ROI descriptor collection. */
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{this, 
                                                                    "RoIs",   
                                                                    "", 
                                                                    "RoIs to read in"};

  /** Write handle for SCT RDO container. */
  SG::WriteHandleKey<SCT_RDO_Container> m_rdoContainerKey{this, 
                                                          "RDOKey", 
                                                          "SCT_RDOs", 
                                                          "SCT RDO key"};

  /** Write handle for LVL 1 Inner Detector time collection. */
  SG::WriteHandleKey<InDetTimeCollection> m_lvl1CollectionKey{this, 
                                                              "LVL1IDKey", 
                                                              "SCT_LVL1ID", 
                                                              "SCT LVL1ID key"};

  /** Write handle for BC ID Inner Detector time collection. */
  SG::WriteHandleKey<InDetTimeCollection> m_bcIDCollectionKey{this, 
                                                              "BCIDKey", 
                                                              "SCT_BCID", 
                                                              "SCT BCID key"};

  /** Write handle for Inner Detector ByteStream error container. */
  SG::WriteHandleKey<InDetBSErrContainer> m_bsErrContainerKey{this, 
                                                              "ByteStreamErrContainer", 
                                                              "SCT_ByteStreamErrs", 
                                                              "SCT BS error key"};

  /** Write handle for SCT ByteStream fraction container. */
  SG::WriteHandleKey<SCT_ByteStreamFractionContainer> m_bsFracContainerKey{this, 
                                                                           "ByteStreamFracContainer", 
                                                                           "SCT_ByteStreamFrac", 
                                                                           "SCT BS fraction key"};
  /** Update handle for SCT RDO Cache. */
  SG::UpdateHandleKey<SCT_RDO_Cache> m_rdoContainerCacheKey;
};

#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDER_H
