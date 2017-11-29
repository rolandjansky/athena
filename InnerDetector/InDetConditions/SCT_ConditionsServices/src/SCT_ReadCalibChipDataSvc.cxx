 /*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibChipDataSvc.cxx Implementation file for SCT_ReadCalibChipDataSvc.
    @author Per Johansson (23/03/09), Shaun Roe (17/2/2010)
*/

#include "SCT_ReadCalibChipDataSvc.h"

// Include Athena stuff
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "Identifier/Identifier.h"
#include "Identifier/Identifier32.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h" 
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SCT_SlhcIdConverter.h"
#include "StoreGate/StoreGateSvc.h"
#include "SCT_ConditionsData/SCT_ModuleCalibParameter.h"
#include "SCT_ReadCalibChipUtilities.h"

// Include Gaudi stuff
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/StatusCode.h"

// Include STL stuff
#include <algorithm>

using namespace SCT_ConditionsServices;
using namespace SCT_ReadCalibChipUtilities;

//----------------------------------------------------------------------
SCT_ReadCalibChipDataSvc::SCT_ReadCalibChipDataSvc (const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_storeGateSvc{"StoreGateSvc", name},
  m_detStoreSvc{"DetectorStore", name},
  m_IOVDbSvc{"IOVDbSvc", name},
  m_SCTdetMgr{nullptr},
  m_attrListColl{nullptr},
  m_id_sct{nullptr},
  m_dataFilled{false},
  m_printCalibDataMaps{false}
  {
    declareProperty("PrintCalibDataMaps",  m_printCalibDataMaps = false, "Print data read from the Calib Data map?");
    declareProperty("AttrListCollFolders", m_atrcollist,                 "List of calibration data folder?"); 
    declareProperty("NoiseLevel",          m_noiseLevel = 1800.0,        "Noise Level for isGood if ever used");
    std::vector<std::string> names{2};
    names[0] = nPtGainFolder;
    names[1] = noiseOccFolder;
    m_atrcollist.setValue(names);
    //initialize boost arrays
    SCT_ModuleCalibParameter oneModuleGain;
    oneModuleGain.assign(0);
    SCT_ModuleCalibParameter oneModuleNoise;
    oneModuleNoise.assign(0);
    //
    SCT_ModuleGainCalibData oneModuleGainParameters;
    oneModuleGainParameters.assign(oneModuleGain);
    SCT_ModuleNoiseCalibData oneModuleNoiseParameters;
    oneModuleNoiseParameters.assign(oneModuleNoise);
    //
    m_nPtGainData.assign(oneModuleGainParameters);
    m_noiseOccData.assign(oneModuleNoiseParameters);
  }

//----------------------------------------------------------------------
SCT_ReadCalibChipDataSvc::~SCT_ReadCalibChipDataSvc() { 
  //nop
}

//----------------------------------------------------------------------

StatusCode 
SCT_ReadCalibChipDataSvc::initialize() {
  // Print where you are
  // Get SCT detector manager
  if (m_detStoreSvc->retrieve(m_SCTdetMgr, "SCT").isFailure()) {
    ATH_MSG_FATAL("Failed to get SCT detector manager");
    return StatusCode::FAILURE;
  }
  // Get SCT helper
  if (m_detStoreSvc->retrieve(m_id_sct, "SCT_ID").isFailure()) {
    ATH_MSG_FATAL("Failed to get SCT helper");
    return StatusCode::FAILURE;
  }
  // Retrieve IOVDb service
  if (m_IOVDbSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve IOVDbSvc ");
    return StatusCode::FAILURE;
  }
  //Register callbacks for CalibData folders using a vector of keys defined in jobOpt
  std::vector<std::string>::const_iterator itr{m_atrcollist.value().begin()};
  std::vector<std::string>::const_iterator end{m_atrcollist.value().end()};
  for (; itr!=end; ++itr) {
    m_key = *itr;
    if (m_key == nPtGainFolder) {
      if (m_detStoreSvc->regFcn(&SCT_ReadCalibChipDataSvc::fillData, this, m_coolGainData, m_key).isFailure()) {
        ATH_MSG_ERROR("Cannot register callbacks function for key " << m_key);
        return StatusCode::FAILURE;
      }
    }
    if (m_key == noiseOccFolder) {
      if (m_detStoreSvc->regFcn(&SCT_ReadCalibChipDataSvc::fillData, this, m_coolNoiseData, m_key).isFailure()) {
        ATH_MSG_ERROR("Cannot register callbacks function for key " << m_key);
        return StatusCode::FAILURE;
      }
    }
  }
  const float errVal{std::numeric_limits<float>::quiet_NaN()};
  //double check: initialize arrays to NaN
  for (int m{0}; m!=NUMBER_OF_MODULES; ++m) {
    for (int p{0}; p!=N_NPTGAIN; ++p) {
      for (int c{0}; c!=CHIPS_PER_MODULE; ++c) {
        m_nPtGainData[m][p][c]=errVal;
      }
    } 
    for (int p{0}; p!=N_NOISEOCC; ++p) {
      for (int c{0}; c!=CHIPS_PER_MODULE; ++c) {
        m_noiseOccData[m][p][c]=errVal;
      }
    }
  }
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
  return m_dataFilled;
} //SCT_ReadCalibChipDataSvc::filled()


//----------------------------------------------------------------------
// Fill the data structures from a callback
StatusCode
SCT_ReadCalibChipDataSvc::fillData(int& /*i*/, std::list<std::string>& l) {
  std::list<std::string>::const_iterator itr{l.begin()}, end{l.end()};
  // Print where you are
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "fillData has been triggered by: ";
    for (; itr!=end; ++itr) {
      msg() << *itr << " ";
    }
    msg() << endmsg;
  }
  StatusCode sc0{SCT_ReadCalibChipDataSvc::fillCalibData(l)};
  // No longer need the conditions folder as stored locally
  for (itr=l.begin(); itr!=end; ++itr) {
    m_IOVDbSvc->dropObject(*itr, true); 
  }
  if (sc0==StatusCode::SUCCESS) {
    m_dataFilled = true;
    ATH_MSG_INFO("Calib Data maps filled ok");
    return StatusCode::SUCCESS;
  } else {
    m_dataFilled = false;
    ATH_MSG_ERROR("fillData failed");
    return StatusCode::FAILURE;
  }

} //SCT_ReadCalibChipDataSvc::fillData()

//----------------------------------------------------------------------
// Callback for Calib data
StatusCode
SCT_ReadCalibChipDataSvc::fillCalibData(std::list<std::string>& keys) {
  // Retrieve CondAttrListCollection
  if (std::find(keys.begin(), keys.end(), noiseOccFolder) != keys.end()) {
    if (m_detStoreSvc->retrieve(m_coolNoiseData, noiseOccFolder).isFailure()) {
      ATH_MSG_ERROR("Could not retrieve CondAttrListCollection for: " << noiseOccFolder);
      return StatusCode::FAILURE;
    }
    // loop over collection
    CondAttrListCollection::const_iterator itLoop{m_coolNoiseData->begin()};
    CondAttrListCollection::const_iterator itLoop_end{m_coolNoiseData->end()};
    for (; itLoop!=itLoop_end; ++itLoop) {
      CondAttrListCollection::ChanNum chanNum{itLoop->first};
      const coral::AttributeList& anAttrList{itLoop->second};
      // Convert chanNum=offlineID into identifier
      Identifier32 moduleId{chanNum};
      //find the corresponding hash
      const IdentifierHash hashId{m_id_sct->wafer_hash(moduleId)};
      //find the index to the module (hash is for each side), to use as index into array
      const unsigned int moduleIdx{hashId/SIDES_PER_MODULE};
      SCT_ModuleNoiseCalibData& theseCalibData{m_noiseOccData[moduleIdx]};
      insertNoiseOccFolderData(theseCalibData, anAttrList);
    }
  }
  if (std::find(keys.begin(), keys.end(), nPtGainFolder) != keys.end()) {
    if (m_detStoreSvc->retrieve(m_coolGainData, nPtGainFolder).isFailure()) {
      ATH_MSG_ERROR("Could not retrieve CondAttrListCollection for: " << nPtGainFolder);
      return StatusCode::FAILURE;
    }
    // loop over collection
    CondAttrListCollection::const_iterator itLoop{m_coolGainData->begin()};
    CondAttrListCollection::const_iterator itLoop_end{m_coolGainData->end()};
    for (; itLoop!=itLoop_end; ++itLoop) {
      CondAttrListCollection::ChanNum chanNum{itLoop->first};
      coral::AttributeList anAttrList{itLoop->second};
      // Convert chanNum=offlineID into identifier
      Identifier32 moduleId{chanNum};
      //find the corresponding hash
      const IdentifierHash hashId{m_id_sct->wafer_hash(moduleId)};
      //find the index to the module (hash is for each side), to use as index into array
      const unsigned int moduleIdx{hashId/SIDES_PER_MODULE};
      SCT_ModuleGainCalibData& theseCalibData{m_nPtGainData[moduleIdx]};
      insertNptGainFolderData(theseCalibData, anAttrList);
    }
  }
  return StatusCode::SUCCESS;
} //SCT_ReadCalibChipDataSvc::fillCalibData()


//----------------------------------------------------------------------
// Returns a bool summary of the data
bool
SCT_ReadCalibChipDataSvc::isGood(const IdentifierHash& elementHashId) {
  const unsigned int moduleIdx{elementHashId/SIDES_PER_MODULE};
  // Retrieve defect data from map
  SCT_ModuleNoiseCalibData& noiseOccData{m_noiseOccData[moduleIdx]};

  // Retrieve the data
  int i{noiseOccIndex("NoiseByChip")};
  if (i<0) {
    ATH_MSG_ERROR("This NoiseOccupancy noise data does not exist");
    return true;
  }
  SCT_ModuleCalibParameter& moduleNoiseData{noiseOccData[i]};

  // Calcuate module status
  // For now just simple check NO mean noise level
  // Chip could be 0 if bypassed, need to check
  //int side = m_id_sct->side(waferId);
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
  //find hash
  const IdentifierHash hashId{m_id_sct->wafer_hash(moduleId)};
  //make index
  const unsigned int idx{hashId/SIDES_PER_MODULE};
  //Retrieve defect data from map
  try {
    SCT_ModuleGainCalibData& wantedNPGData{m_nPtGainData.at(idx)};
    //find the correct index for the required data
    int dataIdx{nPtGainIndex(datatype)};
    if (dataIdx<0) {
      ATH_MSG_ERROR("This N-point gain data: " << datatype << " does not exist");
      return waferData;
    }
    SCT_ModuleCalibParameter& moduleGains{wantedNPGData[dataIdx]};
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
  //find hash
  const IdentifierHash hashId{m_id_sct->wafer_hash(moduleId)};
  //make index
  const unsigned int idx{hashId/SIDES_PER_MODULE};
  try {
    //Retrieve defect data from array
    SCT_ModuleNoiseCalibData& wantedNoiseData{m_noiseOccData.at(idx)};

    //find the correct index for the required data
    int dataIdx{noiseOccIndex(datatype)};
    if (dataIdx<0) {
      ATH_MSG_ERROR("This Noise Occupancy data: " << datatype << " does not exist");
      return waferData;
    }
    SCT_ModuleCalibParameter& moduleNoise{wantedNoiseData[dataIdx]};
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
//---------------------------------------------------------------------- 
//fill appropriate folder
void 
SCT_ReadCalibChipDataSvc::insertNptGainFolderData(SCT_ModuleGainCalibData& theseCalibData, const coral::AttributeList& folderData) {
  for (int i{0}; i!=N_NPTGAIN; ++i) {
    SCT_ModuleCalibParameter& datavec{theseCalibData[i]};
    std::string dbData{((folderData)[nPtGainDbParameterNames[i]]).data<std::string>()};
    fillFromString(dbData, datavec);
  }
}

void 
SCT_ReadCalibChipDataSvc::insertNoiseOccFolderData(SCT_ModuleNoiseCalibData& theseCalibData, const coral::AttributeList& folderData) {
  for (int i{0}; i!=N_NOISEOCC; ++i) {
    SCT_ModuleCalibParameter& datavec{theseCalibData[i]};
    std::string dbData{((folderData)[noiseOccDbParameterNames[i]]).data<std::string>()};
    fillFromString(dbData, datavec);
  }
}

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
