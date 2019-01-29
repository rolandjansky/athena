/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibDataTool.cxx Implementation file for SCT_ReadCalibDataTool.
    @author Per Johansson, 17/05/08, based on SCT_ReadCalibDataTool.
*/

#include "SCT_ReadCalibDataTool.h"

// Include Athena stuff
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

//----------------------------------------------------------------------
SCT_ReadCalibDataTool::SCT_ReadCalibDataTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent),
  m_mutex{},
  m_cacheGain{},
  m_cacheNoise{},
  m_cacheInfo{},
  m_condDataGain{},
  m_condDataNoise{},
  m_condDataInfo{},
  m_id_sct{nullptr} {
  }

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibDataTool::initialize() {
  // Print where you are
  ATH_MSG_DEBUG("in initialize()");

  // Get SCT helper
  ATH_CHECK(detStore()->retrieve(m_id_sct, "SCT_ID"));

  // Retrieve SCT Cabling tool
  ATH_CHECK(m_cabling.retrieve());

  // Read Cond Handle Key
  ATH_CHECK(m_condKeyGain.initialize());
  ATH_CHECK(m_condKeyNoise.initialize());
  ATH_CHECK(m_condKeyInfo.initialize());
  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  return StatusCode::SUCCESS;
} // SCT_ReadCalibDataTool::initialize()

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibDataTool::finalize() {
  return StatusCode::SUCCESS;
} // SCT_ReadCalibDataTool::finalize()

//----------------------------------------------------------------------
//Can only report good/bad at strip level
bool SCT_ReadCalibDataTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return (h==InDetConditions::SCT_STRIP);
}

//----------------------------------------------------------------------
// Returns a bool summary of the data
bool SCT_ReadCalibDataTool::isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  // Status of the compId
  bool status{true};
  // Extract the moduleId from the comp identifier
  Identifier moduleId{m_id_sct->module_id(elementId)};
  switch (h) {
  case InDetConditions::SCT_MODULE:
    {
      // Not applicable for Calibration data
      ATH_MSG_WARNING("summary(): Module good/bad is not applicable for Calibration data");
      break;
    }
  case InDetConditions::SCT_SIDE:
    {
      // Not applicable for Calibration data
      ATH_MSG_WARNING("summary(): Wafer good/bad is not applicable for Calibration data");
      break;
    }
  case InDetConditions::SCT_CHIP:
    {
      // Not applicable for Calibration data
      ATH_MSG_WARNING("summary(): Chip good/bad is not applicable for Calibration data");
      break;
    }
  case InDetConditions::SCT_STRIP:
    {
      // Get hashId
      IdentifierHash hashIdx{m_id_sct->wafer_hash(moduleId)};
      int side{m_id_sct->side(elementId)};
      if (side==1) m_id_sct->get_other_side(hashIdx, hashIdx);
      unsigned int waferIdx{hashIdx};
      //unsigned int waferIdx=hashIdx+side; //uhm
      // Get strip on wafer to check
      int strip{m_id_sct->strip(elementId)};
      // Retrieve isGood Wafer data

      const SCT_AllGoodStripInfo* condDataInfo{getCondDataInfo(ctx)};
      if (condDataInfo==nullptr) {
        ATH_MSG_ERROR("In isGood, SCT_AllGoodStripInfo cannot be retrieved");
        return false;
      }
      // Set value
      status = (*condDataInfo)[waferIdx][strip];
      break;
    }
  default:
    {
      status = true ;
      ATH_MSG_INFO("Unknown component has been asked for, should be Module/Wafer/Chip or Strip; returning 'good' and continuing");
    }    
  } //end of switch structure
  
  // Print status  
  return status;
} //SCT_ReadCalibDataTool::summary()

bool SCT_ReadCalibDataTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(elementId, ctx, h);
}

//----------------------------------------------------------------------
// Returns a defect summary of a defect strip, scan, type and value
SCT_ReadCalibDataTool::CalibDefectType SCT_ReadCalibDataTool::defectType(const Identifier& stripId, InDetConditions::Hierarchy h) const {
  // Print where you are
  ATH_MSG_DEBUG("in defectType()");

  // Create the calibDefectSummary
  CalibDefectType theseSummaryDefects;

  // Retrieve defect data
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_CalibDefectData* condDataGain{getCondDataGain(ctx)};
  if (condDataGain==nullptr) {
    ATH_MSG_ERROR("In defectType, SCT_CalibDefectData (gain) cannot be retrieved.");
    return theseSummaryDefects;
  }
  const SCT_CalibDefectData* condDataNoise{getCondDataNoise(ctx)};
  if (condDataNoise==nullptr) {
    ATH_MSG_ERROR("In defectType, SCT_CalibDefectData (noise) cannot be retrieved.");
    return theseSummaryDefects;
  }

  // Extract the moduleId from the comp identifier
  Identifier moduleId{m_id_sct->module_id(stripId)};
  ATH_MSG_DEBUG("Summary wanted for component: " << stripId << " on module: " << moduleId);

  // Create the CalibDataDefect objects
  SCT_CalibDefectData::CalibModuleDefects wantedNPGDefects{condDataGain->findModule(moduleId)};
  SCT_CalibDefectData::CalibModuleDefects wantedNODefects{condDataNoise->findModule(moduleId)};

  switch (h) {
  case InDetConditions::SCT_MODULE:
    {
      // Not applicable for Calibration data
      ATH_MSG_WARNING("summary(): Module defect summary is not applicable for Calibration data");
      break;
    }

  case InDetConditions::SCT_SIDE:
    {
      // Not applicable for Calibration data
      ATH_MSG_WARNING("summary(): Wafer defect summary is not applicable for Calibration data");
      break;
    }

  case InDetConditions::SCT_CHIP:
    {
      // Not applicable for Calibration data
      ATH_MSG_WARNING("summary(): Chip defect summary is not applicable for Calibration data");
      break;
    }
  case InDetConditions::SCT_STRIP:
    {
      // Get the strip/channel number to check
      int side{m_id_sct->side(stripId)};
      int strip{m_id_sct->strip(stripId)};
      const Identifier waferId{m_id_sct->wafer_id(stripId)};
      const IdentifierHash waferHash{m_id_sct->wafer_hash(waferId)};
      unsigned int stripNum;
      const InDetDD::SiDetectorElement* p_element{getDetectorElement(waferHash, ctx)};
      if (p_element->swapPhiReadoutDirection()) {
        if (side == 0) {
          stripNum =   STRIPS_PER_WAFER-1 - strip;
        } else {
          stripNum = 2*STRIPS_PER_WAFER-1 - strip;
        }      
      } else {
        stripNum = side * STRIPS_PER_WAFER + strip;
      }
      
      // Find the bad strip and fill calibDefectSummary
      if (wantedNPGDefects.begDefects.empty()) {
        ATH_MSG_VERBOSE("No NPtGain defects in this module");
      } else {
        for (unsigned int i{0}; i<wantedNPGDefects.begDefects.size(); ++i) {
          if (stripNum>=wantedNPGDefects.begDefects[i] and stripNum<=wantedNPGDefects.endDefects[i]) {
            theseSummaryDefects.scan.push_back("NPtGain");
            theseSummaryDefects.defect.push_back(wantedNPGDefects.typeOfDefect[i]);
            theseSummaryDefects.value.push_back(wantedNPGDefects.parValue[i]);
            ATH_MSG_VERBOSE("NPtGain defect summary for strip " << stripNum << " filled");
          }
        }
      }

      if (wantedNODefects.begDefects.empty()) {
        ATH_MSG_VERBOSE("No NoiseOccupancy defects in this module");
      } else {
        for (unsigned int i{0}; i != wantedNODefects.begDefects.size(); ++i) {
          if (stripNum>=wantedNODefects.begDefects[i] and stripNum <= wantedNODefects.endDefects[i]) {
            theseSummaryDefects.scan.push_back("NoiseOccupancy");
            theseSummaryDefects.defect.push_back(wantedNODefects.typeOfDefect[i]);
            theseSummaryDefects.value.push_back(wantedNODefects.parValue[i]);
            ATH_MSG_VERBOSE("NoiseOccupancy defect summary for strip " << stripNum << "  filled");
          }
        }
      } 
      if (theseSummaryDefects.scan.empty()) {
        ATH_MSG_VERBOSE("defectSummary(): can't retrieve the defects for this strip: " <<  stripNum << " since strip good");
      }     
      break;
    }
  default:
    {
      ATH_MSG_INFO("Unknown component requested, should be one of Module/Side/Chip or Strip");
      return theseSummaryDefects;
    }

  }//end of switch structure

  return theseSummaryDefects;
} //SCT_ReadCalibDataTool::defectType()

//----------------------------------------------------------------------
// Returns a summary of all defects on a module for a given scan
SCT_CalibDefectData::CalibModuleDefects SCT_ReadCalibDataTool::defectsSummary(const Identifier& moduleId, const std::string& scan) const {
  // Create pointer to the CalibDataDefect object 
  SCT_CalibDefectData::CalibModuleDefects wantedDefects;

  const EventContext& ctx{Gaudi::Hive::currentContext()};

  // Retrieve the correct defect map
  if (scan == "NPtGain") {
    const SCT_CalibDefectData* condDataGain{getCondDataGain(ctx)};
    if (condDataGain==nullptr) {
      ATH_MSG_ERROR("In defectType, SCT_CalibDefectData (gain) cannot be retrieved.");
    } else {
      wantedDefects = condDataGain->findModule(moduleId);
    }
  } else if (scan == "NoiseOccupancy") {
    const SCT_CalibDefectData* condDataNoise{getCondDataNoise(ctx)};
    if (condDataNoise==nullptr) {
      ATH_MSG_ERROR("In defectType, SCT_CalibDefectData (noise) cannot be retrieved.");
    } else {
      wantedDefects = condDataNoise->findModule(moduleId);
    }
  } else {
    ATH_MSG_ERROR("defectsSummary(): Module defects for scan" << scan << " does not exist (only NPtGain or NoiseOccupancy).");
  }

  return wantedDefects;
} //SCT_ReadCalibDataTool::defectsSummary()

//---------------------------------------------------------------------- 
//----------------------------------------------------------------------
// Returns a list of all strips with a certain defects
std::list<Identifier> SCT_ReadCalibDataTool::defectList(const std::string& defect) const {
  std::list<Identifier> defectList;

  // Retrieve defect data
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_CalibDefectData* condDataGain{getCondDataGain(ctx)};
  if (condDataGain==nullptr) {
    ATH_MSG_ERROR("In defectType, SCT_CalibDefectData (gain) cannot be retrieved.");
    return defectList;
  }
  const SCT_CalibDefectData* condDataNoise{getCondDataNoise(ctx)};
  if (condDataNoise==nullptr) {
    ATH_MSG_ERROR("In defectType, SCT_CalibDefectData (noise) cannot be retrieved.");
    return defectList;
  }

  // Create pointer to the CalibDataDefect object 
  SCT_CalibDefectData::CalibModuleDefects wantedDefects;
  
  //Check which scan the defect belongs
  bool npDefect{false};
  bool noDefect{false};
  if (defect=="NO_HI" or defect=="BAD_OPE" or defect=="DOUBTR_HI") {
    noDefect = true;
  } else {
    npDefect = true;
  }
  
  //Loop over all wafers using hashIds from the cabling service
  std::vector<boost::uint32_t> listOfRODs;
  m_cabling->getAllRods(listOfRODs, ctx);
  std::vector<boost::uint32_t>::iterator rodIter{listOfRODs.begin()};
  std::vector<boost::uint32_t>::iterator rodEnd{listOfRODs.end()};
  for (; rodIter!=rodEnd; ++rodIter) {
    std::vector<IdentifierHash> listOfHashes;
    m_cabling->getHashesForRod(listOfHashes, *rodIter, ctx);
    std::vector<IdentifierHash>::iterator hashIt{listOfHashes.begin()};
    std::vector<IdentifierHash>::iterator hashEnd{listOfHashes.end()};
    for (; hashIt != hashEnd; ++hashIt) {
      Identifier waferId{m_id_sct->wafer_id(*hashIt)};
      int side{m_id_sct->side(waferId)};
      //Only use the hashid for side 0, since data saved per module basis
      if (side!=0) continue;
      Identifier moduleId{m_id_sct->module_id(waferId)};
      if (npDefect) {
        wantedDefects = condDataGain->findModule(moduleId);
      }  else if (noDefect) {
        wantedDefects = condDataNoise->findModule(moduleId);
      }
      if (!wantedDefects.begDefects.empty()) {
        for (unsigned int i{0}; i < wantedDefects.begDefects.size(); ++i) {
          if (wantedDefects.typeOfDefect[i] == defect) {
            // Create identifier for all strips inside begin to end
            int strip_beg{static_cast<int>(wantedDefects.begDefects[i])};
            int strip_end{static_cast<int>(wantedDefects.endDefects[i])};
            // In DB: strip from 0-1535, need to convert to side and 0-767 and take into account phiSwaps
            for (int strip{strip_beg}; strip<strip_end+1; strip++) {
              int nside{(strip<STRIPS_PER_WAFER) ? 0 : 1};
              int strip_cor;
              const InDetDD::SiDetectorElement* p_element;
              if (nside==1) { // if side 1 need waferId of side 1 to get phiswap and correct stripId
                IdentifierHash hashSide1;
                m_id_sct->get_other_side(*hashIt, hashSide1);
                waferId = m_id_sct->wafer_id(hashSide1);
                p_element = (getDetectorElement(hashSide1, ctx));
              } else {
                p_element = (getDetectorElement(*hashIt, ctx));
              }
              if (p_element->swapPhiReadoutDirection()) {
                if (nside == 0) {
                  strip_cor =   STRIPS_PER_WAFER-1 - strip;
                } else {
                  strip_cor = 2*STRIPS_PER_WAFER-1 - strip;
                }      
              } else {
                strip_cor = strip - nside * STRIPS_PER_WAFER;
              }
              Identifier stripId{m_id_sct->strip_id(waferId,strip_cor)};
              defectList.push_back(stripId);
            }
          }
        }
      }
    }
  }
  return defectList;
} //SCT_ReadCalibDataTool::defects()
//---------------------------------------------------------------------- 

const SCT_CalibDefectData*
SCT_ReadCalibDataTool::getCondDataGain(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  if (slot>=m_cacheGain.size()) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_cacheGain.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cacheGain[slot]!=evt) {
    std::lock_guard<std::mutex> lock{m_mutex};
    SG::ReadCondHandle<SCT_CalibDefectData> condData{m_condKeyGain};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKeyGain.key());
    }
    m_condDataGain.set(*condData);
    m_cacheGain[slot] = evt;
  }
  return m_condDataGain.get();
}

//----------------------------------------------------------------------

const SCT_CalibDefectData*
SCT_ReadCalibDataTool::getCondDataNoise(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  if (slot>=m_cacheNoise.size()) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_cacheNoise.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cacheNoise[slot]!=evt) {
    std::lock_guard<std::mutex> lock{m_mutex};
    SG::ReadCondHandle<SCT_CalibDefectData> condData{m_condKeyNoise};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKeyNoise.key());
    }
    m_condDataNoise.set(*condData);
    m_cacheNoise[slot] = evt;
  }
  return m_condDataNoise.get();
}

//----------------------------------------------------------------------

const SCT_AllGoodStripInfo*
SCT_ReadCalibDataTool::getCondDataInfo(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  if (slot>=m_cacheInfo.size()) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_cacheInfo.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cacheInfo[slot]!=evt) {
    std::lock_guard<std::mutex> lock{m_mutex};
    SG::ReadCondHandle<SCT_AllGoodStripInfo> condData{m_condKeyInfo};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKeyInfo.key());
    }
    m_condDataInfo.set(*condData);
    m_cacheInfo[slot] = evt;
  }
  return m_condDataInfo.get();
}

//----------------------------------------------------------------------

const InDetDD::SiDetectorElement*
SCT_ReadCalibDataTool::getDetectorElement(const IdentifierHash& waferHash, const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  if (slot>=m_cacheElements.size()) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_cacheElements.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cacheElements[slot]!=evt) {
    std::lock_guard<std::mutex> lock{m_mutex};
    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> condData{m_SCTDetEleCollKey};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_SCTDetEleCollKey.key());
    }
    m_detectorElements.set(*condData);
    m_cacheElements[slot] = evt;
  }
  return m_detectorElements->getDetectorElement(waferHash);
}

//----------------------------------------------------------------------
