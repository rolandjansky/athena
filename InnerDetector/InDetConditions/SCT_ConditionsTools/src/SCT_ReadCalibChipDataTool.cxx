/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibChipDataTool.cxx Implementation file for SCT_ReadCalibChipDataTool.
    @author Per Johansson (23/03/09), Shaun Roe (17/2/2010)
*/

#include "SCT_ReadCalibChipDataTool.h"

// Include Athena stuff
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "SCT_ConditionsData/SCT_ModuleCalibParameter.h"
#include "SCT_ConditionsData/SCT_ModuleGainCalibData.h"
#include "SCT_ConditionsData/SCT_ModuleNoiseCalibData.h"
#include "SCT_ConditionsTools/SCT_ReadCalibChipUtilities.h"

using namespace SCT_ConditionsData;
using namespace SCT_ReadCalibChipUtilities;

//----------------------------------------------------------------------
SCT_ReadCalibChipDataTool::SCT_ReadCalibChipDataTool (const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent),
  m_id_sct{nullptr}
  {
    declareProperty("NoiseLevel",          m_noiseLevel = 1800.0,        "Noise Level for isGood if ever used");
  }

//----------------------------------------------------------------------
StatusCode 
SCT_ReadCalibChipDataTool::initialize() {
  // Get SCT helper
  ATH_CHECK(detStore()->retrieve(m_id_sct, "SCT_ID"));

  // Read Cond Handle Key
  ATH_CHECK(m_condKeyGain.initialize());
  ATH_CHECK(m_condKeyNoise.initialize());

  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataTool::initialize()

//----------------------------------------------------------------------
StatusCode
SCT_ReadCalibChipDataTool::finalize() {
  // Print where you are
  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataTool::finalize()

//----------------------------------------------------------------------
//Can only report good/bad at side level
bool
SCT_ReadCalibChipDataTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return (h==InDetConditions::SCT_SIDE);
}

//----------------------------------------------------------------------
// Returns a bool summary of the data
bool
SCT_ReadCalibChipDataTool::isGood(const IdentifierHash& elementHashId, const EventContext& ctx) const {
  // Retrieve SCT_NoiseCalibData pointer
  const SCT_NoiseCalibData* condDataNoise{getCondDataNoise(ctx)};
  if (condDataNoise==nullptr) {
    ATH_MSG_ERROR("In isGood, SCT_NoiseCalibData cannot be retrieved");
    return true;
  }

  const unsigned int moduleIdx{elementHashId/SIDES_PER_MODULE};
  // Retrieve defect data from map
  const SCT_ModuleNoiseCalibData& noiseOccData{(*condDataNoise)[moduleIdx]};

  // Retrieve the data
  const int i{noiseOccIndex("NoiseByChip")};
  if (i<0) {
    ATH_MSG_ERROR("This NoiseOccupancy noise data does not exist");
    return true;
  }
  const SCT_ModuleCalibParameter& moduleNoiseData{noiseOccData[i]};

  // Calcuate module status
  // For now just simple check NO mean noise level
  // Chip could be 0 if bypassed, need to check
  const int side{static_cast<int>(elementHashId%2)};
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
  const float meanNoiseValue{sum/nChips};
  ATH_MSG_DEBUG("Module mean noise: " << meanNoiseValue);
  return (meanNoiseValue < m_noiseLevel);
} //SCT_ReadCalibChipDataTool::summary()

bool
SCT_ReadCalibChipDataTool::isGood(const IdentifierHash& elementHashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(elementHashId, ctx);
}

//----------------------------------------------------------------------
// Returns a bool summary of the data
bool
SCT_ReadCalibChipDataTool::isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  if (h==InDetConditions::SCT_SIDE) { //Could do by chip too
    const IdentifierHash elementIdHash{m_id_sct->wafer_hash(elementId)};
    return isGood(elementIdHash, ctx);
  } else{
    // Not applicable for Calibration data
    ATH_MSG_WARNING("summary(): " << h << "good/bad is not applicable for Calibration data");
    return true;
  }
}

bool
SCT_ReadCalibChipDataTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(elementId, ctx, h);
}

//----------------------------------------------------------------------
std::vector<float> 
SCT_ReadCalibChipDataTool::getNPtGainData(const Identifier& moduleId, const int side, const std::string& datatype, const EventContext& ctx) const {
  // Print where you are
  ATH_MSG_DEBUG("in getNPtGainData()");
  std::vector<float> waferData;

  // Retrieve SCT_GainCalibData pointer
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
    const int dataIdx{nPtGainIndex(datatype)};
    if (dataIdx<0) {
      ATH_MSG_ERROR("This N-point gain data: " << datatype << " does not exist");
      return waferData;
    }
    const SCT_ModuleCalibParameter& moduleGains{wantedNPGData[dataIdx]};
    const int startOffset{side*CHIPS_PER_SIDE};
    const int endOffset{CHIPS_PER_SIDE+startOffset};
    SCT_ModuleCalibParameter::const_iterator it{moduleGains.begin() + startOffset};
    SCT_ModuleCalibParameter::const_iterator end{moduleGains.begin() + endOffset};
    // Returns the data for the wanted wafer
    if (*it != *it) return waferData;
    //could use signaling NaN here and catch the exception instead, would be quicker: NO! 
    //see: http://stackoverflow.com/questions/235386/using-nan-in-c
    waferData.assign(it, end);
    return waferData;
  } catch (const std::out_of_range& e) {
    return waferData;
  }
} //SCT_ReadCalibChipDataTool::getNPtGainData()

std::vector<float> 
SCT_ReadCalibChipDataTool::getNPtGainData(const Identifier& moduleId, const int side, const std::string& datatype) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getNPtGainData(moduleId, side, datatype, ctx);
}

//----------------------------------------------------------------------
std::vector<float>
SCT_ReadCalibChipDataTool::getNoiseOccupancyData(const Identifier& moduleId, const int side, const std::string& datatype, const EventContext& ctx) const {
  // Print where you are
  ATH_MSG_DEBUG("in getNoiseOccupancyData()");
  std::vector<float> waferData;

  // Retrieve SCT_NoiseCalibData pointer
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
    const int dataIdx{noiseOccIndex(datatype)};
    if (dataIdx<0) {
      ATH_MSG_ERROR("This Noise Occupancy data: " << datatype << " does not exist");
      return waferData;
    }
    const SCT_ModuleCalibParameter& moduleNoise{wantedNoiseData[dataIdx]};
    const int startOffset{side*CHIPS_PER_SIDE};
    const int endOffset{CHIPS_PER_SIDE+startOffset};
    SCT_ModuleCalibParameter::const_iterator it{moduleNoise.begin() + startOffset};
    SCT_ModuleCalibParameter::const_iterator end{moduleNoise.begin() + endOffset};
    // Returns the data for the wanted wafer
    if (*it != *it) return waferData;
    waferData.assign(it, end);
    return waferData;
  } catch (const std::out_of_range& e) {
    return waferData;
  }
} // SCT_ReadCalibChipDataTool::getNoiseOccupancyData()

std::vector<float>
SCT_ReadCalibChipDataTool::getNoiseOccupancyData(const Identifier& moduleId, const int side, const std::string& datatype) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getNoiseOccupancyData(moduleId, side, datatype, ctx);
}

int
SCT_ReadCalibChipDataTool::nPtGainIndex(const std::string& dataName) const {
  int i{N_NPTGAIN};
  while (i--) if (dataName==nPtGainParameterNames[i]) break;
  return i;
}

int
SCT_ReadCalibChipDataTool::noiseOccIndex(const std::string& dataName) const {
  int i{N_NOISEOCC};
  while (i--) if (dataName==noiseOccParameterNames[i]) break;
  return i;
}

const SCT_GainCalibData*
SCT_ReadCalibChipDataTool::getCondDataGain(const EventContext& ctx) const {
  SG::ReadCondHandle<SCT_GainCalibData> condData{m_condKeyGain, ctx};
  return condData.retrieve();
}

const SCT_NoiseCalibData*
SCT_ReadCalibChipDataTool::getCondDataNoise(const EventContext& ctx) const {
  SG::ReadCondHandle<SCT_NoiseCalibData> condData{m_condKeyNoise, ctx};
  return condData.retrieve();
}
