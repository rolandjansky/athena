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
  , m_readKeyState{"/SCT/DCS/CHANSTAT"}
  , m_writeKeyState{"SCT_DCSStatCondData", "SCT_DCSStatCondData"}
  , m_condSvc{"CondSvc", name}
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
  declareProperty("ReadKeyState", m_readKeyState, "Key of input (raw) State conditions folder");
  declareProperty("WriteKeyState", m_writeKeyState, "Key of output (derived) State conditions folder");
}

SCT_DCSConditionsCondAlg::~SCT_DCSConditionsCondAlg() {
}

StatusCode SCT_DCSConditionsCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());

  if (m_returnHVTemp) {
    // Read Cond Handle (HV)
    //    ATH_CHECK(m_readKeyHV.initialize());
  }

  if ((m_readAllDBFolders and m_returnHVTemp) or (not m_readAllDBFolders and not m_returnHVTemp)) {
    // Read Cond Handle (state)
    ATH_CHECK(m_readKeyState.initialize());
    // Write Cond Handle
    ATH_CHECK(m_writeKeyState.initialize());
    // Register write handle
    if(m_condSvc->regHandle(this, m_writeKeyState, m_writeKeyState.dbKey()).isFailure()) {
      ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKeyState.fullKey() << " with CondSvc");
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

StatusCode SCT_DCSConditionsCondAlg::execute()
{
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<SCT_DCSStatCondData> writeHandle{m_writeKeyState};

  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    return StatusCode::SUCCESS;
  }

  // Construct the output Cond Object and fill it in
  SCT_DCSStatCondData* writeCdo{new SCT_DCSStatCondData()};

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKeyState};
  const CondAttrListCollection* readCdo{*readHandle};
  if(readCdo==nullptr) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Size of CondAttrListCollection readCdo->size()= " << readCdo->size());

  // Fill Write Cond Handle
  static const unsigned int defectListIndex{7};
  CondAttrListCollection::const_iterator iter{readCdo->begin()};
  CondAttrListCollection::const_iterator last{readCdo->end()};
  for(; iter!=last; ++iter) {
    // const AthenaAttributeList& list{iter->second};
    // if(list.size()>defectListIndex) {
    //   writeCdo->insert(iter->first, list[defectListIndex].data<std::string>());
    // }
  }

  // Define validity of the output cond obbject and record it
  EventIDRange rangeW;
  if(not readHandle.range(rangeW)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }
  if(writeHandle.record(rangeW, writeCdo).isFailure()) {
    ATH_MSG_ERROR("Could not record SCT_DCSStatCondData " << writeHandle.key()
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_DCSConditionsCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
