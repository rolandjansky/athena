 /*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibChipDataSvc.cxx Implementation file for SCT_ReadCalibChipDataSvc.
    @author Per Johansson (23/03/09), Shaun Roe (17/2/2010)
*/

#include "SCT_ReadCalibChipDataSvc.h"

// Include Athena stuff
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "StoreGate/StoreGateSvc.h"
#include "SCT_ConditionsData/SCT_ModuleCalibParameter.h"
#include "SCT_ConditionsData/SCT_ModuleGainCalibData.h"
#include "SCT_ConditionsData/SCT_ModuleNoiseCalibData.h"
#include "SCT_ReadCalibChipUtilities.h"

// Include Gaudi stuff
#include "GaudiKernel/StatusCode.h"

using namespace SCT_ConditionsServices;
using namespace SCT_ReadCalibChipUtilities;

//----------------------------------------------------------------------
SCT_ReadCalibChipDataSvc::SCT_ReadCalibChipDataSvc (const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_detStoreSvc{"DetectorStore", name},
  m_id_sct{nullptr},
  m_mutex{},
  m_cacheGain{},
  m_cacheNoise{},
  m_condDataGain{},
  m_condDataNoise{},
  m_condKeyGain{"SCT_GainCalibData"},
  m_condKeyNoise{"SCT_NoiseCalibData"}
  {
    declareProperty("NoiseLevel",          m_noiseLevel = 1800.0,        "Noise Level for isGood if ever used");
  }

//----------------------------------------------------------------------
SCT_ReadCalibChipDataSvc::~SCT_ReadCalibChipDataSvc() { 
  //nop
}

//----------------------------------------------------------------------

StatusCode 
SCT_ReadCalibChipDataSvc::initialize() {
  // Get SCT helper
  if (m_detStoreSvc->retrieve(m_id_sct, "SCT_ID").isFailure()) {
    ATH_MSG_FATAL("Failed to get SCT helper");
    return StatusCode::FAILURE;
  }

  // Read Cond Handle Key
  ATH_CHECK(m_condKeyGain.initialize());
  ATH_CHECK(m_condKeyNoise.initialize());

  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataSvc::initialize()

//----------------------------------------------------------------------
StatusCode
SCT_ReadCalibChipDataSvc::finalize() {
  // Print where you are
  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataSvc::finalize()

//----------------------------------------------------------------------
// Query the interfaces.
StatusCode
SCT_ReadCalibChipDataSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (ISCT_ReadCalibChipDataSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = this;
  } else if (ISCT_ConditionsSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
//Can only report good/bad at side level
bool
SCT_ReadCalibChipDataSvc::canReportAbout(InDetConditions::Hierarchy h) {
  return (h==InDetConditions::SCT_SIDE);
}


//----------------------------------------------------------------------
// Returns ok if fillData worked properly
bool
SCT_ReadCalibChipDataSvc::filled() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_NoiseCalibData* condDataNoise{getCondDataNoise(ctx)};
  if (condDataNoise==nullptr) return false;
  const SCT_GainCalibData* condDataGain{getCondDataGain(ctx)};
  if (condDataGain==nullptr) return false;

  return true;
} //SCT_ReadCalibChipDataSvc::filled()


//----------------------------------------------------------------------
// Fill the data structures from a callback
StatusCode
SCT_ReadCalibChipDataSvc::fillData(int& /*i*/, std::list<std::string>& /*l*/) {
  return StatusCode::SUCCESS;
} //SCT_ReadCalibChipDataSvc::fillData()

//----------------------------------------------------------------------
// Returns a bool summary of the data
bool
SCT_ReadCalibChipDataSvc::isGood(const IdentifierHash& elementHashId) {
  // Retrieve SCT_NoiseCalibData pointer
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_NoiseCalibData* condDataNoise{getCondDataNoise(ctx)};
  if (condDataNoise==nullptr) {
    ATH_MSG_ERROR("In isGood, SCT_NoiseCalibData cannot be retrieved");
    return true;
  }

  const unsigned int moduleIdx{elementHashId/SIDES_PER_MODULE};
  // Retrieve defect data from map
  const SCT_ModuleNoiseCalibData& noiseOccData{(*condDataNoise)[moduleIdx]};

  // Retrieve the data
  int i{noiseOccIndex("NoiseByChip")};
  if (i<0) {
    ATH_MSG_ERROR("This NoiseOccupancy noise data does not exist");
    return true;
  }
  const SCT_ModuleCalibParameter& moduleNoiseData{noiseOccData[i]};

  // Calcuate module status
  // For now just simple check NO mean noise level
  // Chip could be 0 if bypassed, need to check
  int side{static_cast<int>(elementHashId%2)};
  int chip{side*CHIPS_PER_SIDE};
  const int endChip{CHIPS_PER_SIDE+chip};
  int nChips{0};
  float sum{0.0};
  for (; chip!=endChip; ++chip) {
    float chipNoise{moduleNoiseData[chip]};
    if (chipNoise!=0.0) {
      sum+=chipNoise;
      ++nChips;
    }
  }
  float meanNoiseValue{sum/nChips};
  ATH_MSG_DEBUG("Module mean noise: " << meanNoiseValue);
  return (meanNoiseValue < m_noiseLevel);
} //SCT_ReadCalibChipDataSvc::summary()

//----------------------------------------------------------------------
// Returns a bool summary of the data
bool
SCT_ReadCalibChipDataSvc::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) {
  if (h==InDetConditions::SCT_SIDE) { //Could do by chip too
    const IdentifierHash elementIdHash{m_id_sct->wafer_hash(elementId)};
    return isGood(elementIdHash);
  } else{
    // Not applicable for Calibration data
    ATH_MSG_WARNING("summary(): " << h << "good/bad is not applicable for Calibration data");
    return true;
  }
}

//----------------------------------------------------------------------
std::vector<float> 
SCT_ReadCalibChipDataSvc::getNPtGainData(const Identifier& moduleId, const int side, const std::string& datatype) {
  // Print where you are
  ATH_MSG_DEBUG("in getNPtGainData()");
  std::vector<float> waferData;

  // Retrieve SCT_GainCalibData pointer
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_GainCalibData* condDataGain{getCondDataGain(ctx)};
  if (condDataGain==nullptr) {
    ATH_MSG_ERROR("In getNPtGainData, SCT_GainCalibData cannot be retrieved");
    return waferData;
  }

  //find hash
  const IdentifierHash hashId{m_id_sct->wafer_hash(moduleId)};
  //make index
  const unsigned int idx{hashId/SIDES_PER_MODULE};
  //Retrieve defect data from map
  try {
    const SCT_ModuleGainCalibData& wantedNPGData{condDataGain->at(idx)};
    //find the correct index for the required data
    int dataIdx{nPtGainIndex(datatype)};
    if (dataIdx<0) {
      ATH_MSG_ERROR("This N-point gain data: " << datatype << " does not exist");
      return waferData;
    }
    const SCT_ModuleCalibParameter& moduleGains{wantedNPGData[dataIdx]};
    int startOffset{side*CHIPS_PER_SIDE};
    int endOffset{CHIPS_PER_SIDE+startOffset};
    SCT_ModuleCalibParameter::const_iterator it{moduleGains.begin() + startOffset};
    SCT_ModuleCalibParameter::const_iterator end{moduleGains.begin() + endOffset};
    // Returns the data for the wanted wafer
    if (*it != *it) return waferData;
    //could use signaling NaN here and catch the exception instead, would be quicker: NO! 
    //see: http://stackoverflow.com/questions/235386/using-nan-in-c
    waferData.assign(it, end);
    return waferData;
  } catch (std::out_of_range e) {
    return waferData; 
  }
} //SCT_ReadCalibChipDataSvc::getNPtGainData()

//----------------------------------------------------------------------
std::vector<float>
SCT_ReadCalibChipDataSvc::getNoiseOccupancyData(const Identifier& moduleId, const int side, const std::string& datatype) {
  // Print where you are
  ATH_MSG_DEBUG("in getNoiseOccupancyData()");
  std::vector<float> waferData;

  // Retrieve SCT_NoiseCalibData pointer
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_NoiseCalibData* condDataNoise{getCondDataNoise(ctx)};
  if (condDataNoise==nullptr) {
    ATH_MSG_ERROR("In getNPtNoiseData, SCT_NoiseCalibData cannot be retrieved");
    return waferData;
  }

  //find hash
  const IdentifierHash hashId{m_id_sct->wafer_hash(moduleId)};
  //make index
  const unsigned int idx{hashId/SIDES_PER_MODULE};
  try {
    //Retrieve defect data from array
    const SCT_ModuleNoiseCalibData& wantedNoiseData{condDataNoise->at(idx)};

    //find the correct index for the required data
    int dataIdx{noiseOccIndex(datatype)};
    if (dataIdx<0) {
      ATH_MSG_ERROR("This Noise Occupancy data: " << datatype << " does not exist");
      return waferData;
    }
    const SCT_ModuleCalibParameter& moduleNoise{wantedNoiseData[dataIdx]};
    int startOffset{side*CHIPS_PER_SIDE};
    int endOffset{CHIPS_PER_SIDE+startOffset};
    SCT_ModuleCalibParameter::const_iterator it{moduleNoise.begin() + startOffset};
    SCT_ModuleCalibParameter::const_iterator end{moduleNoise.begin() + endOffset};
    // Returns the data for the wanted wafer
    if (*it != *it) return waferData;
    waferData.assign(it, end); 
    return waferData;
  } catch (std::out_of_range e) {
    return waferData; 
  }
} // SCT_ReadCalibChipDataSvc::getNoiseOccupancyData()

int
SCT_ReadCalibChipDataSvc::nPtGainIndex(const std::string& dataName) const {
  int i{N_NPTGAIN};
  while (i--) if (dataName==nPtGainParameterNames[i]) break;
  return i;
}

int
SCT_ReadCalibChipDataSvc::noiseOccIndex(const std::string& dataName) const {
  int i{N_NOISEOCC};
  while (i--) if (dataName==noiseOccParameterNames[i]) break;
  return i;
}

const SCT_GainCalibData*
SCT_ReadCalibChipDataSvc::getCondDataGain(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  std::lock_guard<std::mutex> lock{m_mutex};
  if (slot>=m_cacheGain.size()) {
    m_cacheGain.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cacheGain[slot]!=evt) {
    SG::ReadCondHandle<SCT_GainCalibData> condData{m_condKeyGain};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKeyGain.key());
    }
    m_condDataGain.set(*condData);
    m_cacheGain[slot] = evt;
  }
  return m_condDataGain.get();
}

const SCT_NoiseCalibData*
SCT_ReadCalibChipDataSvc::getCondDataNoise(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  std::lock_guard<std::mutex> lock{m_mutex};
  if (slot>=m_cacheNoise.size()) {
    m_cacheNoise.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cacheNoise[slot]!=evt) {
    SG::ReadCondHandle<SCT_NoiseCalibData> condData{m_condKeyNoise};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKeyNoise.key());
    }
    m_condDataNoise.set(*condData);
    m_cacheNoise[slot] = evt;
  }
  return m_condDataNoise.get();
}
