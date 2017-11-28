/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ReadCalibChipNoiseCondAlg.h"

#include "GaudiKernel/EventIDRange.h"

#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"

#include "SCT_ReadCalibChipUtilities.h"

using namespace SCT_ConditionsServices;
using namespace SCT_ReadCalibChipUtilities;

SCT_ReadCalibChipNoiseCondAlg::SCT_ReadCalibChipNoiseCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_readKey{"/SCT/DAQ/Calibration/ChipNoise"}
  , m_writeKey{"SCT_NoiseCalibData"}
  , m_condSvc{"CondSvc", name}
  , m_id_sct{nullptr}
{
  declareProperty("ReadKey", m_readKey, "Key of input (raw) noise conditions folder");
  declareProperty("WriteKey", m_writeKey, "Key of output (derived) noise conditions folder");
}

SCT_ReadCalibChipNoiseCondAlg::~SCT_ReadCalibChipNoiseCondAlg() {
}

StatusCode SCT_ReadCalibChipNoiseCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());

  // Get SCT helper
  ATH_CHECK(detStore()->retrieve(m_id_sct, "SCT_ID"));

  // Read Cond Handle
  ATH_CHECK(m_readKey.initialize());
  // Write Cond Handle
  ATH_CHECK(m_writeKey.initialize());
  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_ReadCalibChipNoiseCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<SCT_NoiseCalibData> writeHandle{m_writeKey};
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    return StatusCode::SUCCESS; 
  }

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey};
  const CondAttrListCollection* readCdo{*readHandle}; 
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  // Get the validitiy range
  EventIDRange rangeW;
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_INFO("Range of input is " << rangeW);
  
  // Construct the output Cond Object and fill it in
  SCT_NoiseCalibData* writeCdo{new SCT_NoiseCalibData()};

  // loop over collection
  CondAttrListCollection::const_iterator itLoop{readCdo->begin()};
  CondAttrListCollection::const_iterator itLoop_end{readCdo->end()};
  for (; itLoop!=itLoop_end; ++itLoop) {
    CondAttrListCollection::ChanNum chanNum{itLoop->first};
    const coral::AttributeList& anAttrList{itLoop->second};
    // Convert chanNum=offlineID into identifier
    Identifier32 moduleId{chanNum};
    //find the corresponding hash
    const IdentifierHash hashId{m_id_sct->wafer_hash(moduleId)};
    //find the index to the module (hash is for each side), to use as index into array
    const unsigned int moduleIdx{hashId/SIDES_PER_MODULE};
    SCT_ModuleNoiseCalibData& theseCalibData{(*writeCdo)[moduleIdx]};
    insertNoiseOccFolderData(theseCalibData, anAttrList);
  }

  // Record the output cond object
  if (writeHandle.record(rangeW, writeCdo).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_NoiseCalibData " << writeHandle.key() 
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    delete writeCdo;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_ReadCalibChipNoiseCondAlg::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

void 
SCT_ReadCalibChipNoiseCondAlg::insertNoiseOccFolderData(SCT_ModuleNoiseCalibData& theseCalibData, const coral::AttributeList& folderData) {
  for (int i{0}; i!=N_NOISEOCC; ++i) {
    SCT_ModuleCalibParameter& datavec{theseCalibData[i]};
    std::string dbData{((folderData)[noiseOccDbParameterNames[i]]).data<std::string>()};
    fillFromString(dbData, datavec);
  }
}
