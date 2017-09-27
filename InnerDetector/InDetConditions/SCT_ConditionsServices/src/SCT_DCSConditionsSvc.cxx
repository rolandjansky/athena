/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// New SCT_DCSConditions Service, based on existing tool in SCT_ConditionsAlgs
// A. R-Veronneau 26/02/08

#include "SCT_DCSConditionsSvc.h"
#include "SCT_SlhcIdConverter.h"
#include "InDetIdentifier/SCT_ID.h"

using SCT_ConditionsServices::castId;

const Identifier SCT_DCSConditionsSvc::s_invalidId;
const float SCT_DCSConditionsSvc::s_defaultHV{-30.}; 
const float SCT_DCSConditionsSvc::s_defaultTemperature{-40.};

SCT_DCSConditionsSvc::SCT_DCSConditionsSvc(const std::string& name,
                                           ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_detStore{"DetectorStore", name},
  m_dataFilled{false},
  m_readAllDBFolders{true},
  m_returnHVTemp{true},
  m_barrel_correction{-3.7},
  m_ecInner_correction{-13.1},
  m_ecOuter_correction{-15.5},
  m_pBadModules{nullptr},
  m_pModulesHV{nullptr},
  m_pModulesTemp0{nullptr},
  m_condKeyState{"SCT_DCSStatCondData"},
  m_condKeyHV{"SCT_DCSHVCondData"},
  m_condKeyTemp0{"SCT_DCSTemp0CondData"},
  m_pHelper{nullptr},
  m_folderPrefix{"/SCT/DCS"}
{ 
    //declare variables which will be filled by jobOptions
    declareProperty("DetectorStore", m_detStore);
    declareProperty("AttrListCollFolders", m_par_atrcollist);
    declareProperty("ReadAllDBFolders", m_readAllDBFolders);
    declareProperty("ReturnHVTemp", m_returnHVTemp);
    declareProperty("TempBarrelCorrection", m_barrel_correction);
    declareProperty("TempEcInnerCorrection", m_ecInner_correction);
    declareProperty("TempEcOuterCorrection", m_ecOuter_correction);
    declareProperty("FolderLocation", m_folderPrefix);
}

StatusCode SCT_DCSConditionsSvc::initialize() {
  if (AthService::initialize().isFailure()) return StatusCode::FAILURE;
  if (m_detStore.retrieve().isFailure()) {
    ATH_MSG_ERROR(" Cannot retrieve detector store ");
    return StatusCode::FAILURE;
  }
  if (m_detStore->retrieve(m_pHelper,"SCT_ID").isFailure()) {
    ATH_MSG_ERROR("SCT helper failed to retrieve ");
    return StatusCode::FAILURE;
  }
  if (m_readAllDBFolders and m_returnHVTemp) {
    std::vector<std::string> names{3};
    names[0] = m_folderPrefix+std::string("/HV");
    names[1] = m_folderPrefix+std::string("/MODTEMP");
    names[2] = m_folderPrefix+std::string("/CHANSTAT");
    m_par_atrcollist.setValue(names);
  } else if (m_returnHVTemp) {
    std::vector<std::string> names{2};
    names[0] = m_folderPrefix+std::string("/HV");
    names[1] = m_folderPrefix+std::string("/MODTEMP");
    m_par_atrcollist.setValue(names);
  } else if (not m_readAllDBFolders and not m_returnHVTemp) {
    std::vector<std::string> names{1};
    names[0] = m_folderPrefix+std::string("/CHANSTAT");
    m_par_atrcollist.setValue(names);
  } else {
    ATH_MSG_INFO("Please check jobOptions, SCT_DCSConditionsSvs does nothing as set up.");
  }
  ATH_MSG_INFO("Test: How many folders in Coll: " << m_par_atrcollist.value().size());
  
  // These callbacks are still needed for SiLonrentzAngleSvc (2017-09-29)
  //Register callbacks for folders in CondAttrListCollection using vector of keys (not hard-coded)
  std::vector<std::string>::const_iterator itr{m_par_atrcollist.value().begin()};
  std::vector<std::string>::const_iterator end{m_par_atrcollist.value().end()};
  for (; itr!=end; ++itr) {
    std::string key{*itr};
    ATH_MSG_INFO(key);
    if (key==m_folderPrefix+std::string("/HV")) {
      if (StatusCode::SUCCESS==m_detStore->regFcn(&SCT_DCSConditionsSvc::fillData, this, m_DCSData_HV, key)) {
        ATH_MSG_INFO("Registered callback for key: " << *itr);
      } else {
        ATH_MSG_ERROR("Cannot register callback function for key " <<  *itr);
      }
    } else if (key==m_folderPrefix+std::string("/MODTEMP")) {
      if (StatusCode::SUCCESS==m_detStore->regFcn(&SCT_DCSConditionsSvc::fillData, this, m_DCSData_MT, key)) {
        ATH_MSG_INFO("Registered callback for key: " << *itr);
      } else {
        ATH_MSG_ERROR("Cannot register callback function for key " <<  *itr);
      }
    } else if (key==m_folderPrefix+std::string("/CHANSTAT")) {
      if (StatusCode::SUCCESS==m_detStore->regFcn(&SCT_DCSConditionsSvc::fillData, this, m_DCSData_CS, key)) {
        ATH_MSG_INFO("Registered callback for key: " << *itr);
      } else {
        ATH_MSG_ERROR("Cannot register callback function for key " <<  *itr);
      }
    } else { 
      ATH_MSG_INFO("Cannot registered callback for key: " << *itr <<" Missing data handle.");
    }
  }

  // Read Cond Handle Keys
  ATH_CHECK(m_condKeyState.initialize());
  ATH_CHECK(m_condKeyHV.initialize());
  ATH_CHECK(m_condKeyTemp0.initialize());

  return StatusCode::SUCCESS;
}

StatusCode SCT_DCSConditionsSvc::finalize() { 
  return StatusCode::SUCCESS;
}

//////////////////////////////
// From s.binet
// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode SCT_DCSConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (ISCT_DCSConditionsSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = this;
  } else if (ISCT_ConditionsSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//returns if fillData worked correctly
bool SCT_DCSConditionsSvc::filled() const {
  return m_dataFilled;
}

//Can report about the module as a whole or the wafer
bool SCT_DCSConditionsSvc::canReportAbout(InDetConditions::Hierarchy h) {
  return (h==InDetConditions::SCT_MODULE or h==InDetConditions::SCT_SIDE or h==InDetConditions::SCT_STRIP);
}   

//returns the module ID (int), or returns -1 if not able to report
Identifier SCT_DCSConditionsSvc::getModuleID(const Identifier& elementId, InDetConditions::Hierarchy h) {
  if (not canReportAbout(h)) return s_invalidId;  

  if (h==InDetConditions::SCT_MODULE) {
    m_moduleId = elementId;
  } else if (h==InDetConditions::SCT_SIDE) {
    m_moduleId = m_pHelper->module_id(elementId); 
  } else if (h==InDetConditions::SCT_STRIP) {
    m_waferId = m_pHelper->wafer_id(elementId);
    m_moduleId = m_pHelper->module_id(m_waferId); 
  }
  return m_moduleId;
}

//Returns if element Id is good or bad
bool SCT_DCSConditionsSvc::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) {
  m_moduleId=getModuleID(elementId, h);
  if (not m_moduleId.is_valid()) return true; // not canreportabout
  else if (not getCondDataState()) return false; // no cond data
  else if (m_pBadModules->output(castId(m_moduleId))==0) return true; //No params are listed as bad
  else return false;
}

//Does the same for hashIds
bool SCT_DCSConditionsSvc::isGood(const IdentifierHash& hashId) {
  m_waferId = m_pHelper->wafer_id(hashId);
  m_moduleId = m_pHelper->module_id(m_waferId);
  return isGood(m_moduleId, InDetConditions::SCT_MODULE);
}

/////////////////////////////////// 

// some lame helper methods: 
// returns HV (s_defaultHV(-30) if there is no information)
float SCT_DCSConditionsSvc::modHV(const Identifier& elementId, InDetConditions::Hierarchy h) {
  m_moduleId = getModuleID(elementId, h);
  if (not m_moduleId.is_valid()) return s_defaultHV; // not canreportabout, return s_defaultHV(-30)

  if (not getCondDataHV()) return s_defaultHV; // no cond data

  float hvval{s_defaultHV};
  if (m_pModulesHV->getValue(castId(m_moduleId), hvval) and isGood(elementId, h)) {
    return hvval;
  }
  return s_defaultHV; //didn't find the module, return s_defaultHV(-30)
}

//Does the same for hashIds
float SCT_DCSConditionsSvc::modHV(const IdentifierHash& hashId) {
  m_waferId = m_pHelper->wafer_id(hashId);
  m_moduleId = m_pHelper->module_id(m_waferId);
  return modHV(m_moduleId,InDetConditions::SCT_MODULE);
} 

//Returns temp0 (s_defaultTemperature(-40) if there is no information)
float SCT_DCSConditionsSvc::hybridTemperature(const Identifier& elementId, InDetConditions::Hierarchy h) {
  m_moduleId = getModuleID(elementId, h);
  if (not m_moduleId.is_valid()) return s_defaultTemperature; // not canreportabout

  if (not getCondDataTemp0()) return s_defaultTemperature; // no cond data

  float temperature{s_defaultTemperature};
  if (m_pModulesTemp0->getValue(castId(m_moduleId), temperature) and isGood(elementId, h)) {
    return temperature;
  }
  return s_defaultTemperature;//didn't find the module, return -40. 
} 

//Does the same for hashIds
float SCT_DCSConditionsSvc::hybridTemperature(const IdentifierHash& hashId) {
  m_waferId = m_pHelper->wafer_id(hashId);
  m_moduleId = m_pHelper->module_id(m_waferId);
  return hybridTemperature(m_moduleId, InDetConditions::SCT_MODULE);
}

//Returns temp0 + correction for Lorentz angle calculation (s_defaultTemperature(-40) if there is no information)
float SCT_DCSConditionsSvc::sensorTemperature(const Identifier& elementId, InDetConditions::Hierarchy h) {
  m_moduleId = getModuleID(elementId, h);
  if (not m_moduleId.is_valid()) return s_defaultTemperature; // not canreportabout

  if (not getCondDataTemp0()) return s_defaultTemperature; // no cond data

  float temperature{s_defaultTemperature};
  if (m_pModulesTemp0->getValue(castId(m_moduleId), temperature) and isGood(elementId, h)) {
    int bec{m_pHelper->barrel_ec(m_moduleId)};
    if (bec==0) { // Barrel
      return ( temperature + m_barrel_correction);  //return the temp+correction
    } else { // Endcaps
      int modeta{m_pHelper->eta_module(m_moduleId)};
      if (modeta==2) {
      	return (temperature + m_ecInner_correction);  //return the temp+correction
      } else {
	return (temperature + m_ecOuter_correction);  //return the temp+correction
      }
    }
  }
  return s_defaultTemperature;  //didn't find the module, return s_defaultTemperature(-40).
} 

//Does the same for hashIds
float SCT_DCSConditionsSvc::sensorTemperature(const IdentifierHash& hashId) {
  m_waferId = m_pHelper->wafer_id(hashId);
  m_moduleId = m_pHelper->module_id(m_waferId);
  return sensorTemperature(m_moduleId, InDetConditions::SCT_MODULE);
}

///////////////////////////////////

// This is kept for SiLorentzAngleSvc callback of SCT_SiliconConditionsSvc and SCT_DCSConditionsSvc
// (2017-09-29)
StatusCode SCT_DCSConditionsSvc::fillData(int& /* i */, std::list<std::string>& /*keys*/) {
   return StatusCode::SUCCESS;
}

bool
SCT_DCSConditionsSvc::getCondDataState() const {
  if (!m_pBadModules) {
    SG::ReadCondHandle<SCT_DCSStatCondData> condData{m_condKeyState};
    if((not condData.isValid()) or !(*condData)) {
      ATH_MSG_ERROR("Failed to get " << m_condKeyState.key());
      return false;
    }
    m_pBadModules = *condData;
  }
  return true;
}

bool
SCT_DCSConditionsSvc::getCondDataHV() const {
  if(!m_pModulesHV) {
    SG::ReadCondHandle<SCT_DCSFloatCondData> condData{m_condKeyHV};
    if ((not condData.isValid()) or !(*condData)) {
      ATH_MSG_ERROR("Failed to get " << m_condKeyHV.key());
      return false;
    }
    m_pModulesHV = *condData;
  }
  return true;
}

bool
SCT_DCSConditionsSvc::getCondDataTemp0() const {
  if (!m_pModulesTemp0) {
    SG::ReadCondHandle<SCT_DCSFloatCondData> condData{m_condKeyTemp0};
    if ((not condData.isValid()) or !(*condData)) {
      ATH_MSG_ERROR("Failed to get " << m_condKeyTemp0.key());
      return false;
    }
    m_pModulesTemp0 = *condData;
  }
  return true;
}
