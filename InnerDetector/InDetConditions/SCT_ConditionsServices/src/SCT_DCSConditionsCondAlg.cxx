/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_DCSConditionsCondAlg.h"

#include "Identifier/IdentifierHash.h"
#include "SCT_Cabling/SCT_OnlineId.h"

#include "GaudiKernel/EventIDRange.h"

SCT_DCSConditionsCondAlg::SCT_DCSConditionsCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_readKeyHV{"/SCT/DCS/HV"}
  , m_writeKeyHV{"SCT_DCSHVCondData", "SCT_DCSHVCondData"}
  , m_rangeHV{}
  , m_readKeyTemperature{"/SCT/DCS/MODTEMP"}
  , m_writeKeyTemperature0{"SCT_DCSTemperature0CondData", "SCT_DCSTemperature0CondData"}
  , m_writeKeyTemperature1{"SCT_DCSTemperature1CondData", "SCT_DCSTemperature1CondData"}
  , m_rangeTemperature{}
  , m_readKeyStatus{"/SCT/DCS/CHANSTAT"}
  , m_writeKeyStatus{"SCT_DCSStatCondData", "SCT_DCSStatCondData"}
  , m_writeCdoStatus{nullptr}
  , m_rangeStatus{}
  , m_condSvc{"CondSvc", name}
  , m_cablingSvc{"SCT_CablingSvc", name}
  , m_readAllDBFolders{true}
  , m_returnHVTemp{true}
  , m_chanstatCut{"NORM"}
  , m_hvLowLimit{0.0}
  , m_hvUpLimit{1000000.0}
  , m_useHV{false}
  , m_useHVLowLimit{19.}
  , m_useHVUpLimit{1000000.0}
  , m_useHVChanCut{"LOOSE"}
{
  declareProperty("ReadAllDBFolders", m_readAllDBFolders);
  declareProperty("ReturnHVTemp", m_returnHVTemp);
  declareProperty("HVCutLow", m_hvLowLimit);
  declareProperty("HVCutUp", m_hvUpLimit);
  declareProperty("StateCut", m_chanstatCut);
  declareProperty("UseDefaultHV", m_useHV);
  declareProperty("useHVLow", m_useHVLowLimit);
  declareProperty("useHVUp", m_useHVUpLimit);
  declareProperty("useHVChan", m_useHVChanCut);
  
  declareProperty("ReadKeyHV", m_readKeyHV, "Key of input (raw) HV conditions folder");
  declareProperty("WriteKeyHV", m_writeKeyHV, "Key of output (derived) HV conditions folder");
  declareProperty("ReadKeyTemperature", m_readKeyTemperature, "Key of input (raw) temperature conditions folder");
  declareProperty("WriteKeyTemperature0", m_writeKeyTemperature0, "Key of output (derived) temperature (side 0) conditions folder");
  declareProperty("WriteKeyTemperature1", m_writeKeyTemperature1, "Key of output (derived) temperature (side 1) conditions folder");
  declareProperty("ReadKeyStatus", m_readKeyStatus, "Key of input (raw) Status conditions folder");
  declareProperty("WriteKeyStatus", m_writeKeyStatus, "Key of output (derived) Status conditions folder");
}

SCT_DCSConditionsCondAlg::~SCT_DCSConditionsCondAlg() {
}

StatusCode SCT_DCSConditionsCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());
  // SCT cabling service
  ATH_CHECK(m_cablingSvc.retrieve());

  if (m_returnHVTemp) {
    // HV
    // Read Cond Handle
    ATH_CHECK(m_readKeyHV.initialize());
    // Write Cond Handle
    ATH_CHECK(m_writeKeyHV.initialize());
    // Register write handle
    if(m_condSvc->regHandle(this, m_writeKeyHV, m_writeKeyHV.dbKey()).isFailure()) {
      ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyHV.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }

    // Temperature
    // Read Cond Handle
    ATH_CHECK(m_readKeyTemperature.initialize());
    // Write Cond Handle
    ATH_CHECK(m_writeKeyTemperature0.initialize());
    ATH_CHECK(m_writeKeyTemperature1.initialize());
    // Register write handle
    if (m_condSvc->regHandle(this, m_writeKeyTemperature0, m_writeKeyTemperature0.dbKey()).isFailure()) {
      ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyTemperature0.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }
    if (m_condSvc->regHandle(this, m_writeKeyTemperature1, m_writeKeyTemperature1.dbKey()).isFailure()) {
      ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyTemperature1.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }
  }

  if ((m_readAllDBFolders and m_returnHVTemp) or (not m_readAllDBFolders and not m_returnHVTemp)) {
    // Status
    // Read Cond Handle
    ATH_CHECK(m_readKeyStatus.initialize());
    // Write Cond Handle
    ATH_CHECK(m_writeKeyStatus.initialize());
    if(m_condSvc->regHandle(this, m_writeKeyStatus, m_writeKeyStatus.dbKey()).isFailure()) {
      ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyStatus.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }
  }

  if (m_useHV) {
    m_hvLowLimit = m_useHVLowLimit;
    m_hvUpLimit = m_useHVUpLimit;
    m_chanstatCut = m_useHVChanCut;
    ATH_MSG_INFO("Using HV and Chanstat"<< m_chanstatCut << " for marking modules bad. >=Hvlow: "
                 << m_hvLowLimit<< " and <=Hv Up: " <<  m_hvUpLimit <<
		 ". Note: UseHV Overrides hv limit and chanstat values in joboptions!!");
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_DCSConditionsCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  bool doStatus{(m_readAllDBFolders and m_returnHVTemp) or (not m_readAllDBFolders and not m_returnHVTemp)};

  if (doStatus) {
    ATH_CHECK(executeStatus());
  }

  if (m_returnHVTemp) {
    ATH_CHECK(executeFloat(HV));
    ATH_CHECK(executeFloat(TEMPERATURE0));
    ATH_CHECK(executeFloat(TEMPERATURE1));
  }

  if (doStatus) {
    ATH_CHECK(recordStatus());
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_DCSConditionsCondAlg::executeStatus() {
  // Write Cond Handle
  SG::WriteCondHandle<SCT_DCSStatCondData> writeHandle{m_writeKeyStatus};

  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_INFO("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    return StatusCode::SUCCESS; 
  }

  // Construct the output Cond Object and fill it in
  m_writeCdoStatus = new SCT_DCSStatCondData();

  // Read Cond Handle 
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKeyStatus};
  const CondAttrListCollection* readCdo{*readHandle}; 
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    delete m_writeCdoStatus;
    m_writeCdoStatus = nullptr;
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  
  std::string param{"STATE"};
  CondAttrListCollection::const_iterator attrList{readCdo->begin()};
  CondAttrListCollection::const_iterator end{readCdo->end()};
  // CondAttrListCollection doesn't support C++11 type loops, no generic 'begin'
  for (; attrList!=end; ++attrList) {
    // A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum channelNumber{attrList->first};
    CondAttrListCollection::AttributeList payload{attrList->second};
    if (payload.exists(param)) {
      unsigned int val{payload[param].data<unsigned int>()};
      unsigned int hvstate{val bitand 240};
      unsigned int lvstate{val bitand 15};
      if (   ( (m_chanstatCut=="NORM")  and not ((hvstate==16 or hvstate==48)                                and (lvstate==1 or lvstate==3))                             )
          or ( (m_chanstatCut=="NSTBY") and not ((hvstate==16 or hvstate==48 or hvstate==32)                 and (lvstate==1 or lvstate==3 or lvstate==2))               )
          or ( (m_chanstatCut=="LOOSE") and not ((hvstate==16 or hvstate==48 or hvstate==32 or hvstate==128) and (lvstate==1 or lvstate==3 or lvstate==2 or lvstate==8)) )
	 ) {
        m_writeCdoStatus->fill(channelNumber, param);
      } else {
        m_writeCdoStatus->remove(channelNumber, param);
      }
    } else {
      ATH_MSG_WARNING(param << " does not exist for ChanNum " << channelNumber);
    }
  }

  // Define validity of the output cond object
  if (not readHandle.range(m_rangeStatus)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    delete m_writeCdoStatus;
    m_writeCdoStatus = nullptr;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_DCSConditionsCondAlg::recordStatus() {
  // Write Cond Handle
  SG::WriteCondHandle<SCT_DCSStatCondData> writeHandle{m_writeKeyStatus};

  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_INFO("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    //    delete m_writeCdoStatus;
    m_writeCdoStatus = nullptr;
    return StatusCode::SUCCESS; 
  }

  // Record the output cond object
  if (writeHandle.record(m_rangeStatus, m_writeCdoStatus).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_DCSStatCondData " << writeHandle.key() 
		  << " with EventRange " << m_rangeStatus
		  << " into Conditions Store");
    //    delete m_writeCdoStatus;
    m_writeCdoStatus = nullptr;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << m_rangeStatus << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_DCSConditionsCondAlg::executeFloat(FloatKey key) {
  const SG::ReadCondHandleKey<CondAttrListCollection>* readKey{nullptr};
  const SG::WriteCondHandleKey<SCT_DCSFloatCondData>* writeKey{nullptr};
  EventIDRange* rangeW{nullptr};
  std::string param;
  if (key==HV) {
    readKey = &m_readKeyHV;
    writeKey = &m_writeKeyHV;
    rangeW = &m_rangeHV;
    param = "HVCHVOLT_RECV";
  } else if (key==TEMPERATURE0) {
    readKey = &m_readKeyTemperature;
    writeKey = &m_writeKeyTemperature0;
    rangeW = &m_rangeTemperature;
    param = "MOCH_TM0_RECV";
  } else if (key==TEMPERATURE1) {
    readKey = &m_readKeyTemperature;
    writeKey = &m_writeKeyTemperature1;
    rangeW = &m_rangeTemperature;
    param = "MOCH_TM1_RECV";
  }

  // Write Cond Handle
  SG::WriteCondHandle<SCT_DCSFloatCondData> writeHandle{*writeKey};

  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_INFO("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    return StatusCode::SUCCESS; 
  }

  // Construct the output Cond Object and fill it in
  SCT_DCSFloatCondData* writeCdo{new SCT_DCSFloatCondData()};
  // clear structures before filling
  writeCdo->clear();

  // Read Cond Handle 
  SG::ReadCondHandle<CondAttrListCollection> readHandle{*readKey};
  const CondAttrListCollection* readCdo{*readHandle}; 
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    delete writeCdo;
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  
  CondAttrListCollection::const_iterator attrList{readCdo->begin()};
  CondAttrListCollection::const_iterator end{readCdo->end()};
  // CondAttrListCollection doesn't support C++11 type loops, no generic 'begin'
  for (; attrList!=end; ++attrList) {
    // A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum channelNumber{attrList->first};
    CondAttrListCollection::AttributeList payload{attrList->second};
    if (payload.exists(param)) {
      float val{payload[param].data<float>()};
      writeCdo->setValue(channelNumber, val);
    } else {
      ATH_MSG_WARNING(param << " does not exist for ChanNum " << channelNumber);
    }
  }

  // Define validity of the output cond obbject and record it
  if (not readHandle.range(*rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    delete writeCdo;
    return StatusCode::FAILURE;
  }

  if (writeHandle.record(*rangeW, writeCdo).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_DCSFloatCondData " << writeHandle.key() 
		  << " with EventRange " << *rangeW
		  << " into Conditions Store");
    delete writeCdo;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << *rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_DCSConditionsCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
