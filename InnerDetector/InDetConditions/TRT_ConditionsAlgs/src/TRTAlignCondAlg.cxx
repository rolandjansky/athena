/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTAlignCondAlg.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

TRTAlignCondAlg::TRTAlignCondAlg(const std::string& name
				 , ISvcLocator* pSvcLocator )
  : ::AthAlgorithm(name,pSvcLocator)
  , m_readKeyDynamicGlobal("/TRT/AlignL1/TRT")
  , m_readKeyDynamicRegular("/TRT/AlignL2")
  , m_readKeyRegular("/TRT/Align")
  , m_writeKey("TRTAlignmentStore","TRTAlignmentStore")
  , m_condSvc("CondSvc",name)
  , m_detManager(nullptr)
  , m_useDynamicFolders(false)
{
  declareProperty("ReadKeyDynamicGlobal",m_readKeyDynamicGlobal);
  declareProperty("ReadKeyDynamicRegular",m_readKeyDynamicRegular);
  declareProperty("ReadKeyRegular",m_readKeyRegular);
  declareProperty("WriteKey",m_writeKey);
  declareProperty("UseDynamicFolders",m_useDynamicFolders);
}

TRTAlignCondAlg::~TRTAlignCondAlg()
{
}

StatusCode TRTAlignCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );

  // Read Condition Handles
  if(m_useDynamicFolders) {
    ATH_CHECK( m_readKeyDynamicGlobal.initialize() );
    ATH_CHECK( m_readKeyDynamicRegular.initialize() );
    m_readKeyRegular = std::string("");
  }

  else {
    m_readKeyDynamicGlobal = std::string("");
    m_readKeyDynamicRegular = std::string("");
    ATH_CHECK( m_readKeyRegular.initialize() );
  }
  ATH_CHECK( m_writeKey.initialize() );

  // Register write handle
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(detStore()->retrieve(m_detManager,"TRT"));

  return StatusCode::SUCCESS;
}

StatusCode TRTAlignCondAlg::execute()
{
  ATH_MSG_DEBUG("execute " << name());

  // ____________ Construct Write Cond Handle and check its validity ____________
  SG::WriteCondHandle<GeoAlignmentStore> writeHandle{m_writeKey};

  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    // in theory this should never be called in MT
    //writeHandle.updateStore();
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");

    return StatusCode::SUCCESS; 
  }

  // ____________ Construct new Write Cond Object and its range ____________
  GeoAlignmentStore* writeCdo = new GeoAlignmentStore();
  EventIDRange rangeW;

  // ____________ Get Read Cond Objects ____________
  // Container for passing read CDO-s over to the Detector Manager
  InDetDD::RawAlignmentObjects readCdoContainer;

  if(m_useDynamicFolders) {
    // 1. Dynamic folders

    // ** Global
    SG::ReadCondHandle<CondAttrListCollection> readHandleDynamicGlobal{m_readKeyDynamicGlobal};
    // Get CDO and store it into container
    const CondAttrListCollection* readCdoDynamicGlobal{*readHandleDynamicGlobal}; 
    if(readCdoDynamicGlobal==nullptr) {
      ATH_MSG_ERROR("Null pointer to the read conditions object: Dynamic Global");
      return StatusCode::FAILURE;
    }
    readCdoContainer.emplace(m_readKeyDynamicGlobal.key(),readCdoDynamicGlobal);
    // Get range
    EventIDRange rangeDynamicGlobal;
    if(!readHandleDynamicGlobal.range(rangeDynamicGlobal)) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleDynamicGlobal.key());
      return StatusCode::FAILURE;
    }

    // ** Regular
    SG::ReadCondHandle<AlignableTransformContainer> readHandleDynamicRegular{m_readKeyDynamicRegular};
    // Get CDO and store it into container
    const AlignableTransformContainer* readCdoDynamicRegular{*readHandleDynamicRegular}; 
    if(readCdoDynamicRegular==nullptr) {
      ATH_MSG_ERROR("Null pointer to the read conditions object: Dynamic Regular");
      return StatusCode::FAILURE;
    }
    readCdoContainer.emplace(m_readKeyDynamicRegular.key(),readCdoDynamicRegular);
    // Get range
    EventIDRange rangeDynamicRegular;
    if(!readHandleDynamicRegular.range(rangeDynamicRegular)) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleDynamicRegular.key());
      return StatusCode::FAILURE;
    }

    // Create an intersection of input ranges
    rangeW = EventIDRange::intersect(rangeDynamicGlobal,rangeDynamicRegular);
  }
  else {
    // 2. Regular folder
    SG::ReadCondHandle<AlignableTransformContainer> readHandleRegular{m_readKeyRegular};
    // Get CDO and store it into container
    const AlignableTransformContainer* readCdoRegular{*readHandleRegular}; 
    if(readCdoRegular==nullptr) {
      ATH_MSG_ERROR("Null pointer to the read conditions object: Regular");
      return StatusCode::FAILURE;
    }
    readCdoContainer.emplace(m_readKeyRegular.key(),readCdoRegular);
    // Get range
    if(!readHandleRegular.range(rangeW)) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleRegular.key());
      return StatusCode::FAILURE;
    }
  }


  // ____________ Apply alignments to TRT GeoModel ____________
  if(m_detManager->align(readCdoContainer,writeCdo).isFailure()) {
    ATH_MSG_ERROR("Failed to apply alignments to TRT");
    return StatusCode::FAILURE;
  }

  // Record the resulting CDO
  if(writeHandle.record(rangeW,writeCdo).isFailure()) {
    ATH_MSG_ERROR("Could not record GeoAlignmentStore " << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode TRTAlignCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
