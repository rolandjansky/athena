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
  m_IOVDbSvc{"IOVDbSvc", name},
  m_dataFilled{false},
  m_status{0},
  m_readAllDBFolders{true},
  m_returnHVTemp{true},
  m_dropFolder{true},
  m_barrel_correction{-3.7},
  m_ecInner_correction{-13.1},
  m_ecOuter_correction{-15.5},
  m_hvLowLimit{0.0},
  m_hvUpLimit{1000000.0},
  m_pBadModules{nullptr},
  m_pModulesHV{nullptr},
  m_pModulesTemp0{nullptr},
  m_pModulesTemp1{nullptr},
  m_condKeyState{"SCT_DCSStatCondData"},
  m_condKeyHV{"SCT_DCSHVCondData"},
  m_condKeyTemp0{"SCT_DCSTemp0CondData"},
  m_condKeyTemp1{"SCT_DCSTemp1CondData"},
  m_pHelper{nullptr}, 
  m_folderPrefix{"/SCT/DCS"},
  m_chanstatCut{"NORM"},
  m_useHV{false},
  m_useHVLowLimit{19.},
  m_useHVUpLimit{1000000.0},
  m_useHVChanCut{"LOOSE"} { 
    //declare variables which will be filled by jobOptions
    declareProperty("DetectorStore", m_detStore);
    declareProperty("AttrListCollFolders", m_par_atrcollist);
    declareProperty("ReadAllDBFolders", m_readAllDBFolders);
    declareProperty("ReturnHVTemp", m_returnHVTemp);
    declareProperty("DropDCSFolders", m_dropFolder);
    declareProperty("TempBarrelCorrection", m_barrel_correction);
    declareProperty("TempEcInnerCorrection", m_ecInner_correction);
    declareProperty("HVCutLow", m_hvLowLimit);
    declareProperty("HVCutUp", m_hvUpLimit);
    declareProperty("TempEcOuterCorrection", m_ecOuter_correction);
    declareProperty("FolderLocation", m_folderPrefix);
    declareProperty("StateCut", m_chanstatCut);
    declareProperty("UseDefaultHV", m_useHV);
    declareProperty("useHVLow", m_useHVLowLimit);
    declareProperty("useHVUp", m_useHVUpLimit);
    declareProperty("useHVChan", m_useHVChanCut);
  }

StatusCode SCT_DCSConditionsSvc::initialize() {
  if (AthService::initialize().isFailure()) return StatusCode::FAILURE;
  if (m_detStore.retrieve().isFailure()) {
    ATH_MSG_ERROR(" Cannot retrieve detector store ");
    return StatusCode::FAILURE;
  }
  // if (m_readAllDBFolders and m_returnHVTemp) {
  //   std::vector<std::string> names{3};
  //   names[0] = m_folderPrefix+std::string("/HV");
  //   names[1] = m_folderPrefix+std::string("/MODTEMP");
  //   names[2] = m_folderPrefix+std::string("/CHANSTAT");
  //   m_par_atrcollist.setValue(names);
  // } else if (m_returnHVTemp) {
  //   std::vector<std::string> names{2};
  //   names[0] = m_folderPrefix+std::string("/HV");
  //   names[1] = m_folderPrefix+std::string("/MODTEMP");
  //   m_par_atrcollist.setValue(names);
  // } else if (not m_readAllDBFolders and not m_returnHVTemp) {
  //   std::vector<std::string> names{1};
  //   names[0] = m_folderPrefix+std::string("/CHANSTAT");
  //   m_par_atrcollist.setValue(names);
  // } else {
  //   ATH_MSG_INFO("Please check jobOptions, SCT_DCSConditionsSvs does nothing as set up.");
  // }
  // ATH_MSG_INFO("Test: How many folders in Coll: " << m_par_atrcollist.value().size());
  if (m_detStore->retrieve(m_pHelper,"SCT_ID").isFailure()) {
    ATH_MSG_ERROR("SCT helper failed to retrieve ");
    return StatusCode::FAILURE;
  }
  
  // if (m_useHV) {
  //   m_hvLowLimit = m_useHVLowLimit;
  //   m_hvUpLimit = m_useHVUpLimit;
  //   m_chanstatCut = m_useHVChanCut;
  //   ATH_MSG_INFO("Using HV and Chanstat"<< m_chanstatCut << " for marking modules bad. >=Hvlow: " <<
  //       	 m_hvLowLimit<< " and <=Hv Up: " <<  m_hvUpLimit << m_par_atrcollist.value().size() <<
  //       	 ". Note: UseHV Overrides hv limit and chanstat values in joboptions!!");
  // }

  // //Register callbacks for folders in CondAttrListCollection using vector of keys (not hard-coded)
  // std::vector<std::string>::const_iterator itr{m_par_atrcollist.value().begin()};
  // std::vector<std::string>::const_iterator end{m_par_atrcollist.value().end()};
  // for (; itr!=end; ++itr) {
  //   std::string key{*itr};
  //   ATH_MSG_INFO(key);
  //   if (key==m_folderPrefix+std::string("/HV")) {
  //     if (StatusCode::SUCCESS==m_detStore->regFcn(&SCT_DCSConditionsSvc::fillData, this, m_DCSData_HV, key)) {
  //       ATH_MSG_INFO("Registered callback for key: " << *itr);
  //     } else {
  //       ATH_MSG_ERROR("Cannot register callback function for key " <<  *itr);
  //     }
  //   } else if (key==m_folderPrefix+std::string("/MODTEMP")) {
  //     if (StatusCode::SUCCESS==m_detStore->regFcn(&SCT_DCSConditionsSvc::fillData, this, m_DCSData_MT, key)) {
  //       ATH_MSG_INFO("Registered callback for key: " << *itr);
  //     } else {
  //       ATH_MSG_ERROR("Cannot register callback function for key " <<  *itr);
  //     }
  //   } else if (key==m_folderPrefix+std::string("/CHANSTAT")) {
  //     if (StatusCode::SUCCESS==m_detStore->regFcn(&SCT_DCSConditionsSvc::fillData, this, m_DCSData_CS, key)) {
  //       ATH_MSG_INFO("Registered callback for key: " << *itr);
  //     } else {
  //       ATH_MSG_ERROR("Cannot register callback function for key " <<  *itr);
  //     }
  //   } else { 
  //     ATH_MSG_INFO("Cannot registered callback for key: " << *itr <<" Missing data handle.");
  //   }
  // }

  // Read Cond Handle Keys
  ATH_CHECK(m_condKeyState.initialize());
  ATH_CHECK(m_condKeyHV.initialize());
  ATH_CHECK(m_condKeyTemp0.initialize());
  ATH_CHECK(m_condKeyTemp1.initialize());

  // m_pBadModules = std::make_unique<SCT_DCSStatCondData>();
  // m_pModulesHV = std::make_unique<SCT_DCSFloatCondData>();
  // m_pModulesTemp0 = std::make_unique<SCT_DCSFloatCondData>();
  // m_pModulesTemp1 = std::make_unique<SCT_DCSFloatCondData>();
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

// //The actual function which puts a error code in a map for each DCS parameter/folder
// StatusCode SCT_DCSConditionsSvc::fillData(int& /* i */, std::list<std::string>& keys) {
 
//   m_dataFilled=false;
//   if (!m_pBadModules) {
//     ATH_MSG_ERROR("Bad modules data object has NULL pointer");
//     return StatusCode::FAILURE;
//   }

//   std::list<std::string>::const_iterator itr_key{keys.begin()};
//   // Retrieve CondAttrListCollection
//   const CondAttrListCollection* attrListCollection;
//   for (; itr_key!=keys.end(); ++itr_key) {
//     const std::string& thisFolder{*itr_key};
//     if (m_detStore->retrieve(attrListCollection,thisFolder).isSuccess()) {
//       // loop over collection
//       CondAttrListCollection::const_iterator attrList{attrListCollection->begin()};
//       CondAttrListCollection::const_iterator end{attrListCollection->end()};
//       //std::ostringstream attrStr1;
//       for (; attrList!=end; ++attrList) {
//         //A CondAttrListCollection is a map of ChanNum and AttributeList
//         CondAttrListCollection::ChanNum channelNumber{(*attrList).first};
//         const CondAttrListCollection::AttributeList& payload{attrList->second};
//         //loop over AttributeListSpecification
//         coral::AttributeList::const_iterator attrspecb{payload.begin()};
//         coral::AttributeList::const_iterator attrspece{payload.end()};
//         for (; attrspecb!=attrspece; ++attrspecb) {
//           std::string param{(*attrspecb).specification().name()};
// 	  //Check if modules was not ok (1 or 16 for HV and LV) and not set manually (3 or 48). Both LV and HV must be either for module to be good.  
//           if (param=="STATE") {
// 	    try {
// 	      unsigned int hvstate{(*attrList).second[param].data<unsigned int>() bitand 240};
// 	      unsigned int lvstate{(*attrList).second[param].data<unsigned int>() bitand 15};
// 	      if (   ( (m_chanstatCut=="NORM")  and not ((hvstate==16 or hvstate==48)                                and (lvstate==1 or lvstate==3)) )
// 		 or ( (m_chanstatCut=="NSTBY") and not ((hvstate==16 or hvstate==48 or hvstate==32)                 and (lvstate==1 or lvstate==3 or lvstate==2)) )
//        	         or ( (m_chanstatCut=="LOOSE") and not ((hvstate==16 or hvstate==48 or hvstate==32 or hvstate==128) and (lvstate==1 or lvstate==3 or lvstate==2 or lvstate==8)) )
// 		 ) {  //Option for no cut is m_chanstatCut == 'NONE'
// 		m_pBadModules->fill(channelNumber, param);
// 	      } else {
// 		m_pBadModules->remove(channelNumber, param);
// 	      }
// 	    } catch(...) {
// 	      ATH_MSG_DEBUG("Exception caught while trying to access STATE");
// 	    }
// 	  } else if (param=="CCtagON_Name" or param=="CCtagON_Send" or param=="CCtagON_Recv" or param=="LVCHSTAT_RECV" or param=="HVCHCURR_RECV") {
// 	    //Set to ignore all parameters, except HV and LV
// 	    continue;
// 	  } 

// 	  // if we are asked to return the HV and temp, then we need to fill these strucures. 
// 	  if (param=="HVCHVOLT_RECV") {
// 	    try {
// 	      float hvval{(*attrList).second[param].data<float>()};
// 	      if ((hvval<m_hvLowLimit) or (hvval>m_hvUpLimit)) {
// 		m_pBadModules->fill(channelNumber, param);
// 	      } else {
// 		m_pBadModules->remove(channelNumber, param);
// 	      }
// 	      if (m_returnHVTemp) {
// 		m_pModulesHV->setValue(channelNumber, hvval);
// 	      }
// 	    } catch(...) {
// 	      ATH_MSG_DEBUG("Exception caught while trying to access HVCHVOLT_RECV");
// 	    }
// 	  } else if (m_returnHVTemp and param=="MOCH_TM0_RECV") {
// 	    try {
//               m_pModulesTemp0->setValue(channelNumber, (*attrList).second[param].data<float>());
// 	    } catch(...) {
// 	      ATH_MSG_DEBUG("Exception caught while trying to access MOCH_TM0_RECV");
// 	    }
// 	  } else if (m_returnHVTemp and param=="MOCH_TM1_RECV") { //2 temp sensors per module
// 	    try {
// 	      m_pModulesTemp1->setValue(channelNumber, (*attrList).second[param].data<float>());
// 	    } catch(...) {
// 	      ATH_MSG_DEBUG("Exception caught while trying to access MOCH_TM1_RECV");
// 	    }
// 	  }
// 	}

//       }
//     }
//     m_dataFilled=true;
//   }

//   //we are done with the db folders and have the data locally. 
//   if (not m_dropFolder) {
//     std::list<std::string>::const_iterator itr2_key{keys.begin()};
//     std::list<std::string>::const_iterator end2_key{keys.end()};
//     for (; itr2_key!=end2_key; ++itr2_key) {
//       const std::string& aFolder{*itr2_key};
//       // loop over collection
//       m_IOVDbSvc->dropObject(aFolder, false); //"false" here keeps the data in the cache, so SG doesn't have to go back to the DB each time  
//     }
//   }

//   return StatusCode::SUCCESS;
// }

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
  if (!m_pModulesTemp1) {
    SG::ReadCondHandle<SCT_DCSFloatCondData> condData{m_condKeyTemp0};
    if ((not condData.isValid()) or !(*condData)) {
      ATH_MSG_ERROR("Failed to get " << m_condKeyTemp0.key());
      return false;
    }
    m_pModulesTemp0 = *condData;
  }
  return true;
}

bool
SCT_DCSConditionsSvc::getCondDataTemp1() const {
  if (!m_pModulesTemp1) {
    SG::ReadCondHandle<SCT_DCSFloatCondData> condData{m_condKeyTemp1};
    if ((not condData.isValid()) or !(*condData)) {
      ATH_MSG_ERROR("Failed to get " << m_condKeyTemp1.key());
      return false;
    }
    m_pModulesTemp1 = *condData;
  }
  return true;
}
